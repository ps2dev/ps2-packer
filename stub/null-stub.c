/* This is the null "uncompression" stub for ps2-packer */

#include <string.h>
#include "packer-stub.h"

/* We don't have to decompress anything. However, we need to put the data
   at the right place. */
void Decompress(u8 *dest, const u8 *src, u32 dst_size, u32 src_size) {
    memcpy(dest, src, src_size);
}
