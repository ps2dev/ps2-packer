/* This is the lzo uncompression stub for ps2-packer */

#include <kernel.h>
#include "packer-stub.h"
#include "minilzo.h"

void Decompress(u8 *dest, const u8 *src, u32 dst_size, u32 src_size) {
    int r, unpack_size;
    
    r = lzo1x_decompress(src, src_size, dest, &unpack_size, NULL);
    
    if (r != LZO_E_OK) {
#ifdef DEBUG
	if (r != LZO_E_OK) {
	    switch (r) {
	    case LZO_E_ERROR:
		printf("Got LZO_E_ERROR.\n");
		break;
	    case LZO_E_OUT_OF_MEMORY:
		printf("Got LZO_OUT_OF_MEMORY.\n");
		break;
	    case LZO_E_NOT_COMPRESSIBLE:
		printf("Got LZO_E_NOT_COMPRESSIBLE.\n");
		break;
	    case LZO_E_INPUT_OVERRUN:
		printf("Got LZO_E_INPUT_OVERRUN.\n");
		break;
	    case LZO_E_OUTPUT_OVERRUN:
		printf("Got LZO_E_OUTPUT_OVERRUN.\n");
		break;
	    case LZO_E_LOOKBEHIND_OVERRUN:
		printf("Got LZO_E_LOOKBEHIND_OVERRUN.\n");
		break;
	    case LZO_E_EOF_NOT_FOUND:
		printf("Got LZO_E_EOF_NOT_FOUND.\n");
		break;
	    case LZO_E_INPUT_NOT_CONSUMED:
		printf("Got LZO_E_INPUT_NOT_CONSUMED.\n");
		break;
	    default:
		printf("Got unknown error.\n");
	    }
	}
	if (unpack_size != dst_size)
	    printf("Decompression error: unpack_size = %08X, whereas dst_size = %08X\n", unpack_size, dst_size);
#endif
	SleepThread();
    }
}
