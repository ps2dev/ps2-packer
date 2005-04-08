/* This is the main stub file for ps2-packer, using kernel mode */

#include <tamtypes.h>
#include <kernel.h>

#ifdef DEBUG
#include <sifrpc.h>
#endif

#include "packer-stub.h"

static void fast_memzero(u8 * ptr, u32 size) {
    u32 t;
    __asm__ volatile ("\n"
	"\taddu      %0, %1, %2\n"
	"1:\n"
	"\tnop\n"
	"\tnop\n"
	"\tnop\n"
	"\tsb        $0, 0(%1)\n"
	"\tsltu      %2, %0, %1\n"
	"\taddiu     %1, 1\n"
	"\tbnez      %2, 1b\n"
	: "=r" (t) : "r" (ptr), "r" (size)
    );
}

/* Code highly inspired from sjeep's sjcrunch */

/* That variable comes from the crt0.s file. */
extern packed_Header * PackedELF;

typedef int (*main_ptr)(int argc, char ** argv);

int main(int argc, char ** argv) {
    u8 * compressedData;
    packed_SectionHeader * sectionHeader;
    int i;


    compressedData = ((u8 *) PackedELF) + sizeof(packed_Header);

    
    DIntr();
    ee_kmode_enter();

    for (i = 0; i < PackedELF->numSections; i++) {
	sectionHeader = (packed_SectionHeader *) compressedData;
	compressedData += sizeof(packed_SectionHeader);
	Decompress((u8 *) sectionHeader->virtualAddr, compressedData, sectionHeader->originalSize, sectionHeader->compressedSize);
	if(sectionHeader->zeroByteSize)
    	    fast_memzero((void *)(sectionHeader->virtualAddr + sectionHeader->originalSize), sectionHeader->zeroByteSize);
	compressedData += sectionHeader->compressedSize;
	if (((u32) compressedData) & 3)
	    compressedData = (u32 *) ((((u32)compressedData) | 3) + 1);
    }

    ee_kmode_exit();
    EIntr();
    

    FlushCache(2);
    FlushCache(0);
    

    DIntr();
    ee_kmode_enter();

    ((main_ptr)PackedELF->entryAddr)(argc, argv);
    
    ee_kmode_exit();
    EIntr();

    return 0;
}
