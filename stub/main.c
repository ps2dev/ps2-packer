/* This is the zlib uncompression stub for ps2-packer */

#include <tamtypes.h>
#include <kernel.h>

#ifdef DEBUG
#include <sifrpc.h>
#endif

#include "zlib.h"

#if 0
/* This seems to cause problems... :-P
   I think it's because it zeroify too much data...
   Anyway, please, somebody eventually fix it, that
   would free up a little bit of code in the final stub :) */
static void fast_memzero(u8 * ptr, u32 size) {
    u32 t;
    __asm__ volatile ("\n"
	"\taddu      %0, %1, %2\n"
	"1:\n"
	"\tnop\n"
	"\tnop\n"
	"\tnop\n"
	"\tsq        $0, 0(%1)\n"
	"\tsltu      %2, %0, %1\n"
	"\taddiu     %1, 16\n"
	"\tbnez      %2, 1b\n"
	: "=r" (t) : "r" (ptr), "r" (size)
    );
}
#else
#include <string.h>
#define fast_memzero(ptr, size) memset(ptr, 0, size)
#endif

void Decompress(u8 *dest, u8 *src, u32 dst_size, u32 src_size);

/* Code highly inspired from sjeep's sjcrunch */

/*
  The data stream, located at the pointer PackedELF is created that way:
   | packed_Header | packed_SectionHeader | compressed data | packed_SectionHeader | compressed data | ...
*/

typedef struct {
    u32 entryAddr;
    u32 numSections;
} packed_Header;

typedef struct {
    u32 compressedSize;
    u32 originalSize;
    u32 zeroByteSize;
    u32 virtualAddr;
} packed_SectionHeader;

/* That variable comes from the crt0.s file. */
extern packed_Header * PackedELF;

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
    }
    
#ifdef DEBUG
    printf("All done, running!\n");
    SifExitRpc();
    FlushCache(2);
    FlushCache(0);
#endif

    ExecPS2((void *)PackedELF->entryAddr, NULL, argc, argv);
    return 0;
}

void Decompress(u8 *dest, u8 *src, u32 dst_size, u32 src_size) {
    z_stream d_stream;
    
    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;
    
    d_stream.next_in = src;
    d_stream.avail_in = src_size;
    d_stream.next_out = dest;
    d_stream.avail_out = dst_size;
    
    if (inflateInit(&d_stream) != Z_OK) {
#ifdef DEBUG
	printf("Error during inflateInit\n");
#endif
	SleepThread();
    }
    
    if (inflate(&d_stream, Z_NO_FLUSH) != Z_STREAM_END) {
#ifdef DEBUG
	printf("Error during inflate.\n");
#endif
	SleepThread();
    }
    
    if (inflateEnd(&d_stream) != Z_OK) {
#ifdef DEBUG
	printf("Error during inflateEnd.\n");
#endif
	SleepThread();
    }
}
