#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

int pack_section(const u8 * source, u8 ** dest, u32 source_size);

#endif // __COMMON_H__

