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
#include <zlib.h>

typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned long int u32;

static void printe(char * fmt, ...) {
    va_list list;
    va_start(list, fmt);
    vfprintf(stderr, fmt, list);
    va_end(list);
    exit(-1);
}

int pack_section(const u8 * source, u8 ** dest, u32 source_size) {
    u32 packed_size;
    u8 * packed;
    z_stream c_stream;
    
    packed_size = source_size * 1.2 + 2048;
    packed = (u8 *) malloc(packed_size);
	
    c_stream.zalloc = (alloc_func)0;
    c_stream.zfree = (free_func)0;
    c_stream.opaque = (voidpf)0;
	
    if (deflateInit(&c_stream, 9) != Z_OK)
        printe("Error during deflateInit.\n");
	
    c_stream.next_in = source;
    c_stream.avail_in = source_size;
    c_stream.next_out = packed;
    c_stream.avail_out = packed_size;

    if (deflate(&c_stream, Z_FINISH) != Z_STREAM_END)
        printe("Error during deflate.\n");
	
    if (deflateEnd(&c_stream) != Z_OK)
        printe("Error during deflateEnd.\n");

    packed_size = c_stream.total_out;

    packed = realloc(packed, packed_size);

    *dest = packed;

    return packed_size;
}

u32 signature() {
    return 0x42494c5a;
}
