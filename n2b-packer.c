/*
 *  PS2-Packer
 *  Copyright (C) 2004 Nicolas "Pixel" Noble
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
#include <ucl/ucl.h>

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
    ucl_uint packed_size;
    u8 * packed;
    
    packed_size = source_size * 1.2 + 2048;
    packed = (u8 *) malloc(packed_size);
    
    if (ucl_nrv2b_99_compress(source, source_size, packed, &packed_size, NULL, 10, NULL, NULL) != UCL_E_OK) {
	printe("Error during ucl_nrv2b_99_compress.\n");
    }
    
    packed = realloc(packed, packed_size);

    *dest = packed;
    
    return packed_size;
}

u32 signature() {
    return 0x3142324e;
}
