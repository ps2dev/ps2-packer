.global _start
.global PackedELF
.global Signature

	.extern _signature

	.text

#
# This signature has to be stored right before the PackedELF location. It will
# be used to identify the stubs against the compression modules inside the
# packer. It can also be used by the unpacker stub to see if the compressor is
# matching the stub.
#
Signature:
	.word	_signature

#
# This variable has to be stored right before the _start location. That
# way, the packer will change it so to match the user's options.
#
PackedELF:
	.word	0x1b00000

	#Avoid the hardware restriction that could cause data placed adjacent to instructions to be considered as a CACHE instruction,
	#which may fetch a wrong instruction, destroy data or affect FPU division.
	#This would happen if the data and instructions have a certain bit pattern. Requires 5 or more nops to avoid.
	nop
	nop
	nop
	nop
	nop

	.ent _start
_start:
	lw	$s2, PackedELF
	
	lw	$s1, 12($s2)  // virtual address == destination
	addiu	$s0, $s2, 16 // skip entry point, original size, zero byte size, virtual address, == source
	lw	$s3, 4($s2)
	addu	$s4, $s3, $s1
	jal	n2e_decompress

	lw	$s0, 8($s2)

	.set noreorder
loop:
	sb	$0, 0($s4)
	addiu   $s4, 1
	addiu	$s0, -1
	bnez	$s0, loop
	nop

	.set reorder

	move	$s0, $a0
	move	$s1, $a1

	#FlushCache(0)
	li	$v1, 0x64
	move	$a0, $0
	syscall

	#FlushCache(2)
	li	$v1, 0x64
	li	$a0, 2
	syscall
	
	move	$a0, $s0
	move	$a1, $s1
	
	lw	$s2, 0($s2)  // entry point
	jr	$s2

	.end _start

#include "ucl/n2e.S"
