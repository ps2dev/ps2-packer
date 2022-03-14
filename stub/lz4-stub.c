/* This is the lz4 uncompression stub for ps2-packer */

#include <kernel.h>
#include "lz4.h"

void Decompress(u8 *dest, const u8 *src, u32 dst_size, u32 src_size) {
    LZ4_decompress_safe((const char *)src, (char *)dest, src_size, dst_size);
}

