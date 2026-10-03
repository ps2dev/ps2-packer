/* This is the main stub file for ps2-packer, using kernel mode */

#include <tamtypes.h>
#include <kernel.h>

#ifdef DEBUG
#include <sifrpc.h>
#endif

#include "packer-stub.h"

/* Zero exactly [ptr, ptr + size). ptr and the loop flag are written by the asm, so they are output
   operands, and the flag is early-clobber so it cannot share a register with end. */
static void fast_memzero(u8 * ptr, u32 size) {
    u8 * end = ptr + size;
    u32 more;
    if (!size)
	return;
    __asm__ volatile ("\n"
	"\t.set      push\n"
	"\t.set      noreorder\n"
	"1:\n"
	"\tnop\n"
	"\tnop\n"
	"\tnop\n"
	"\tsb        $0, 0(%0)\n"
	"\taddiu     %0, %0, 1\n"
	"\tsltu      %1, %0, %2\n"
	"\tbnez      %1, 1b\n"
	"\tnop\n"
	"\t.set      pop\n"
	: "+r" (ptr), "=&r" (more) : "r" (end) : "memory"
    );
}

/* Code highly inspired from sjeep's sjcrunch */

/* That variable comes from the crt0.s file. */
extern packed_Header * PackedELF;

typedef int (*main_ptr)(int argc, char ** argv);

int main(int argc, char ** argv) {
    u8 * compressedData;
    int i;


    compressedData = ((u8 *) PackedELF) + sizeof(packed_Header);

    
    DIntr();
    ee_kmode_enter();

    for (i = 0; i < PackedELF->numSections; i++) {
    packed_SectionHeader * sectionHeader;
	sectionHeader = (packed_SectionHeader *) compressedData;
	compressedData += sizeof(packed_SectionHeader);
	Decompress((u8 *) sectionHeader->virtualAddr, compressedData, sectionHeader->originalSize, sectionHeader->compressedSize);
	if(sectionHeader->zeroByteSize)
    	    fast_memzero((void *)(sectionHeader->virtualAddr + sectionHeader->originalSize), sectionHeader->zeroByteSize);
	compressedData += sectionHeader->compressedSize;
	if (((u32) compressedData) & 3)
	    compressedData = (u8 *) ((((u32)compressedData) | 3) + 1);
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
