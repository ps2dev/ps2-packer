/* This is the ucl's n2e uncompression stub for ps2-packer */

#include <kernel.h>
#include "packer-stub.h"
#include "ucl.h"

void Decompress(u8 *dest, const u8 *src, u32 dst_size, u32 src_size) {
    int r, unpack_size;
    
    r = ucl_nrv2e_decompress_8(src, src_size, dest, &unpack_size);
    
    if (r != UCL_E_OK) {
#ifdef DEBUG
	if (unpack_size != dst_size)
	    printf("Decompression error: unpack_size = %08X, whereas dst_size = %08X\n", unpack_size, dst_size);
	else
	    printf("Unknown decompression error.\n");
#endif
	SleepThread();
    }
}
