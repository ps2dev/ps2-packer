/* n2e_d.c -- implementation of the NRV2E decompression algorithm

   This file is part of the UCL data compression library.

   Copyright (C) 1996-2004 Markus Franz Xaver Johannes Oberhumer
   All Rights Reserved.

   The UCL library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   The UCL library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the UCL library; see the file COPYING.
   If not, write to the Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   Markus F.X.J. Oberhumer
   <markus@oberhumer.com>
   http://www.oberhumer.com/opensource/ucl/
 */

/*
 *
 * Hand-modified for ps2-packer.
 *
 */

#include "ucl_conf.h"
#include "getbit.h"
#define getbit(bb)      getbit_8(bb,src,ilen)

UCL_PUBLIC(int)
ucl_nrv2e_decompress_8          ( const ucl_bytep src, ucl_uint  src_len,
                                        ucl_bytep dst, ucl_uintp dst_len)
{
    ucl_uint32 bb = 0;
    ucl_uint ilen = 0, olen = 0, last_m_off = 1;

    for (;;)
    {
        ucl_uint m_off, m_len;

        while (getbit(bb))
        {
            dst[olen++] = src[ilen++];
        }
        m_off = 1;
        for (;;)
        {
            m_off = m_off*2 + getbit(bb);
            if (getbit(bb)) break;
            m_off = (m_off-1)*2 + getbit(bb);
        }
        if (m_off == 2)
        {
            m_off = last_m_off;
            m_len = getbit(bb);
        }
        else
        {
            m_off = (m_off-3)*256 + src[ilen++];
            if (m_off == UCL_UINT32_C(0xffffffff))
                break;
            m_len = (m_off ^ UCL_UINT32_C(0xffffffff)) & 1;
            m_off >>= 1;
            last_m_off = ++m_off;
        }
        if (m_len)
            m_len = 1 + getbit(bb);
        else if (getbit(bb))
            m_len = 3 + getbit(bb);
        else
        {
            m_len++;
            do {
                m_len = m_len*2 + getbit(bb);
            } while (!getbit(bb));
            m_len += 3;
        }
        m_len += (m_off > 0x500);
        {
            const ucl_bytep m_pos;
            m_pos = dst + olen - m_off;
            dst[olen++] = *m_pos++;
            do dst[olen++] = *m_pos++; while (--m_len > 0);
        }
    }
    *dst_len = olen;
    return ilen == src_len ? UCL_E_OK : (ilen < src_len ? UCL_E_INPUT_NOT_CONSUMED : UCL_E_INPUT_OVERRUN);
}
