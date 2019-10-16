/* This is the main stub file for ps2-packer */

#include <tamtypes.h>
#include <kernel.h>

#ifdef DEBUG
#include <sifrpc.h>
#endif

#include "packer-stub.h"

#if 1
static void fast_memzero(u8 * ptr, u32 size) {
    u32 t;
    __asm__ volatile ("\n"
	"\taddu      %0, %1, %2\n"
	"1:\n"
	"\tnop\n"
	"\tnop\n"
	"\tnop\n"
	"\tsb        $0, 0(%1)\n"
	"\tsltu      %2, %1, %0\n"
	"\taddiu     %1, 1\n"
	"\tbnez      %2, 1b\n"
	: "=r" (t) : "r" (ptr), "r" (size)
    );
}
#elif 1
static void fast_memzero(u8 * ptr, u32 size) {
    while (size--) *(ptr++) = 0;
}
#else
#include <string.h>
#define fast_memzero(ptr,size) memset(ptr,0,size)
#endif

/* Code highly inspired from sjeep's sjcrunch */

/* That variable comes from the crt0.s file. */
extern packed_Header * PackedELF;

#ifndef DO_EXECPS2
struct args {
    u32 argc;
    char ** argv;
};

typedef int (*main_ptr)(struct args);
#endif

int main(int argc, char ** argv) {
    u8 * compressedData;
    packed_SectionHeader * sectionHeader;
    int i;

    compressedData = ((u8 *) PackedELF) + sizeof(packed_Header);
    
#ifdef DEBUG
    SifInitRpc(0);
    printf("Init!\n");
    printf("entryAddr = 0x%X\n", PackedELF->entryAddr);
    printf("numSections = %d\n", PackedELF->numSections);
#endif

    for (i = 0; i < PackedELF->numSections; i++) {
	sectionHeader = (packed_SectionHeader *) compressedData;
	compressedData += sizeof(packed_SectionHeader);
#ifdef DEBUG
	printf("section #%d: virtualAddr = 0x%X, originalSize = 0x%X, compressedSize = 0x%X, zeroByteSize = 0x%X\n",
		i, sectionHeader->virtualAddr, sectionHeader->originalSize, sectionHeader->compressedSize, sectionHeader->zeroByteSize);
#endif
	Decompress((u8 *) sectionHeader->virtualAddr, compressedData, sectionHeader->originalSize, sectionHeader->compressedSize);
	if(sectionHeader->zeroByteSize)
    	    fast_memzero((void *)(sectionHeader->virtualAddr + sectionHeader->originalSize), sectionHeader->zeroByteSize);
	compressedData += sectionHeader->compressedSize;
	if (((u32) compressedData) & 3)
	    compressedData = (u8 *) ((((u32)compressedData) | 3) + 1);
    }
    
#ifdef DEBUG
    printf("All done, running!\n");
    SifExitRpc();
#endif
    FlushCache(2);
    FlushCache(0);

#ifdef DO_EXECPS2
    ExecPS2((void *)PackedELF->entryAddr, NULL, argc, argv);
#else
    {
	struct args args;
	args.argc = argc;
	args.argv = argv;
	((main_ptr)PackedELF->entryAddr)(args);
    }
#endif
    return 0;
}
