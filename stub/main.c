/* This is the main stub file for ps2-packer */

#include <tamtypes.h>
#include <kernel.h>

#ifdef DEBUG
#include <sifrpc.h>
#endif

#include "packer-stub.h"

#if 1
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

// With this line we disabled the patched functionalities
DISABLE_PATCHED_FUNCTIONS();
// With this line we disabled the extra functionalities for timers
DISABLE_EXTRA_TIMERS_FUNCTIONS();

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
