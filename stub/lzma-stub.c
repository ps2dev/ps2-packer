/* This is the lzma uncompression stub for ps2-packer */

#include <kernel.h>
#include "packer-stub.h"
#include "Alloc.h"
#include "LzmaDec.h"

void Decompress(u8 *dest, const u8 *src, u32 dst_size, u32 src_size) {
    u32 unpack_size, srcLen;
    ELzmaStatus status;
    SRes res;

    unpack_size = dst_size;
    srcLen = src_size - LZMA_PROPS_SIZE;

    res = LzmaDecode(
        &dest[0], &unpack_size,
        &src[LZMA_PROPS_SIZE], &srcLen,
        &src[0], LZMA_PROPS_SIZE,
        LZMA_FINISH_END, &status, &g_Alloc);

    if (res != SZ_OK) {
#ifdef DEBUG
    switch (res) {
    case SZ_ERROR_DATA:
    printf("Got SZ_ERROR_DATA\n");
    break;
    case SZ_ERROR_MEM:
    printf("Got SZ_ERROR_MEM\n");
    break;
    case SZ_ERROR_UNSUPPORTED:
    printf("Got SZ_ERROR_UNSUPPORTED\n");
    break;
    case SZ_ERROR_INPUT_EOF:
    printf("Got SZ_ERROR_INPUT_EOF\n");
    break;
    default:
    printf("Got unknown error %d\n", res);
    }
#endif
    SleepThread();
    }
    else if (status != LZMA_STATUS_FINISHED_WITH_MARK) {
#ifdef DEBUG
    switch (status) {
    case LZMA_STATUS_NOT_FINISHED:
    printf("Got LZMA_STATUS_NOT_FINISHED\n");
    break;
    case LZMA_STATUS_MAYBE_FINISHED_WITHOUT_MARK:
    printf("Got LZMA_STATUS_MAYBE_FINISHED_WITHOUT_MARK\n");
    break;
    default:
    printf("Got unknown status %d\n", status);
    }
#endif
    SleepThread();
    }
}
