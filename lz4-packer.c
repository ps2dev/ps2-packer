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
#include "lz4.h"
#include "lz4hc.h"
#include "common.h"

int pack_section(const u8 * source, u8 ** dest, u32 source_size) {
    char *packed;
    int packed_size;
    
    packed_size = LZ4_compressBound(source_size);
    packed = malloc(packed_size);
    packed_size = LZ4_compress_HC((const char *)source, packed, source_size, packed_size, LZ4HC_CLEVEL_MAX);
    *dest = realloc(packed, packed_size);
    
    return packed_size;
}

u32 signature() {
    return 0x314fffff;
}
