// Simple n2e decompressor based off n2e_d.c. (c) TyRaNiD 2k4
.set noreorder
.set noat
.global n2e_decompress
.ent n2e_decompress

#define src        $s0
#define dest       $s1

#define bb         $t0
#define last_m_off $t1
#define m_off      $t2
#define m_len      $t3
#define bit_count  $t4
#define m_pos      $t5
#define saved_ra   $t7

n2e_decompress:

   move   saved_ra,  $ra	// Save ra
   move   bit_count, $0		// Set bit_count to 0
   addi   last_m_off, $0, 1  	// Set last_m_off to 1

loop1:				// for(;;) - while(getbit(bb))

   jal    getbit
   addi   m_off, $0, 1		// Use delay slot to set m_off
   beq    $v0, $0, loop2	// Branch to loop2 if getbit(bb) == 0
   lbu    $1, 0(src)		// Use delay slot to load from src
   sb     $1, 0(dest)		// Store in destination
   addi   src, src, 1		// Increment src
   j      loop1			// Branch back to loop
   addi   dest, dest, 1		// Increment dest

loop2:				// for(;;)

   jal   getbit			// getbit(bb)
   sll   m_off, m_off, 1        // m_off = m_off * 2
   jal   getbit			// getbit(bb)
   addu  m_off, m_off, $v0	// m_off += getbit(bb)
   bne   $v0, $0, loop3		// Branch to loop3 if getbit(bb) != 0
   addiu $1, m_off, -2		// If m_off == 2 then $1 == 0 

   jal   getbit			// getbit(bb)
   add   m_off, m_off, $1	// m_off = m_off + (m_off - 2) == (m_off - 1) * 2
   b	 loop2			// Return back to m_off_loop
   add   m_off, m_off, $v0	// m_off = m_off + getbit(bb)

loop3:				// if(m_off == 2)

   beq   $1, $0, loop3_offeq2   // Branch if m_off == 2
   addi  m_off, m_off, -3	// Set m_off -= 3 here, m_off is trashed if the branch
				// is taken anyway

   sll   m_off, m_off, 8	// m_off = m_off << 8 (* 256)
   lbu   $1, 0(src)		// src[ilen]
   add   m_off, m_off, $1	// m_off = m_off + src[ilen++]

   addiu $1, m_off, 1		// $1 == 0 if m_off == 0xFFFFFFFF

   bne   $1, $0, 1f	        // if(m_off == 0xffffffff) break; i.e. if(m_off + 1 == 0)
   addi  src, src, 1	 	// ilen++
   jr    saved_ra
1:
   andi  m_len, $1, 1		// m_len =  (m_off ^ 1) & 1
   srl   m_off, m_off, 1	// m_off >>= 1;
   addi  m_off, m_off, 1	// ++m_off
   j     loop3_end		// Branch to end
   move  last_m_off, m_off	// last_m_off = ++m_off

loop3_offeq2:			
   jal   getbit
   move  m_off, last_m_off	// Move last_m_off to m_off 
   move  m_len, $v0		// m_len = getbit(bb), v0 is next getbit

loop3_end:
   jal   getbit
   sub   m_pos, dest, m_off	// Use delay slot, m_pos = dest - m_off

   bnel  m_len, $0, loop4_end	// if(m_len), if m_len != 0 then branch to end
   addi  m_len, $v0, 1		// m_len = 1 + getbit(bb)
   jal   getbit
   sltiu $1, $v0, 1		// Set $1 to 1 if $v0 == 0
   addi  m_len, m_len, 1

   beql  $1, $0, loop4_end	// if(getbit(bb)) if $1 == 0, or v0 != 0
   addi  m_len, $v0, 3		// m_len = getbit(bb) + 3

   //else 
loop4:
				// m_len already set to m_len++
   sll   m_len, m_len, 1	// m_len = m_len * 2
   jal   getbit			// getbit
   add   m_len, m_len, $v0	// m_len = m_len + getbit(bb) v0 is previous getbit	

   bnel  $v0, $0, loop4_end     // if(getbit != 0) end
   addi  m_len, m_len, 3	// m_len = m_len + 3
   jal   getbit			// Set up next getbit, talk about wasteful
   nop				// I just can't seem to get rid of this :(
   b	 loop4			// branch back to loop
   
loop4_end:
				
   sltiu $1, m_off, 0x501	// m_len += (m_off > 0x500)
   sub   m_len, m_len, $1	// Instead of +1 for > 0x500 we do -1 for < 0x501

copy:				// Da copy loop
   lbu	 $1, 0(m_pos)		// Load from m_pos
   sb	 $1, 0(dest)		// Store in destination
   bgez  m_len, temp_copy	// If we still have data to copy
   addiu dest, dest, 1
   b 	 loop1    

temp_copy:
   addi  m_pos, m_pos, 1
   b     copy
   addi  m_len, m_len, -1

.end n2e_decompress

.ent getbit
getbit:
   bgtzl  bit_count, 1f
   addi   bit_count, bit_count, -1
   lbu    bb, 0(src)
   addi   src, src, 1
   addi   bit_count, $0, 7
1:
   srlv   $v0, bb, bit_count
   jr     $ra
   andi   $v0, $v0, 1
.end getbit
