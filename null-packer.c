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

#include <stdlib.h>
#include <string.h>

typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned long int u32;

/* We don't have to pack anything. However, we have to return an malloc()ed
   piece of memory. */
int pack_section(const u8 * source, u8 ** dest, u32 source_size) {
    *dest = (u8 *) malloc(source_size);
    memcpy(*dest, source, source_size);
    return source_size;
}

/* The signature of the NULL module. This hexadecimal number means "NULL"
   in little endian. The endianess should be safe. */
u32 signature() {
    return 0x4c4c554e;
}
