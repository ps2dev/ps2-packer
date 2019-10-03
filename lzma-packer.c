/*
 *  PS2-Packer
 *  Copyright (C) 2004-2005 Nicolas "Pixel" Noble
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "Alloc.h"
#include "LzmaEnc.h"
#include "common.h"

static void printe(char * fmt, ...) {
    va_list list;
    va_start(list, fmt);
    vfprintf(stderr, fmt, list);
    va_end(list);
    exit(-1);
}

int pack_section(const u8 * source, u8 ** dest, u32 source_size) {
    size_t propsSize, packed_size;
    u8 * packed;
    CLzmaEncProps props;
    SRes res;

    propsSize = LZMA_PROPS_SIZE;
    packed_size = source_size * 1.2 + 2048;
    packed = (u8 *) malloc(propsSize + packed_size);

    LzmaEncProps_Init(&props);
    props.level = 9;
    props.dictSize = 1 << 22; /* 4194304 Bytes */
    props.writeEndMark = 1;
    props.reduceSize = source_size;

    res = LzmaEncode(
        &packed[LZMA_PROPS_SIZE], &packed_size,
        &source[0], source_size,
        &props, &packed[0], &propsSize, props.writeEndMark,
        NULL, &g_Alloc, &g_BigAlloc);

    if ((res != SZ_OK) || (propsSize != LZMA_PROPS_SIZE))
        printe("Error during LzmaEncode.\n");

    packed = realloc(packed, propsSize + packed_size);

    *dest = packed;

    return propsSize + packed_size;
}

u32 signature() {
    return 0x414d5a4c;
}
