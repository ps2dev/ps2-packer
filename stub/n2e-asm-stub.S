.global _start
.global PackedELF
.global Signature

	.extern _signature
	.extern n2e_decompress

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

	.ent _start
_start:
	lw	$s2, PackedELF
	
	lw	$s5, 0($s2)	// entry point
	lw	$s6, 4($s2)	// number of sections
	
	addiu	$s2, 8

loop:
	addiu	$s6, -1		// decrement number of sections
	
	lw	$s1, 8($s2)	// virtual address == destination
	addiu	$s0, $s2, 16	// skip original size, zero byte size, virtual address, compressed size, == source
	lw	$s3, 0($s2)
	addu	$s4, $s3, $s1
	jal	n2e_decompress
	
	lw	$s0, 4($s2)
	lw	$s1, 12($s2)
	addiu	$s2, 16
	addu	$s2, $s1
	andi	$s1, $s2, 3

	.set noreorder
zero_loop:
	sb	$0, 0($s4)
	addiu   $s4, 1
	bnez	$s0, zero_loop
	addiu	$s0, -1
	
	ori	$s2, 3
	bnez	$s1, 1f
	addiu	$s2, 1
	addiu   $s2, -4
	.set reorder
1:
	bnez	$s6, loop
	
	jr	$s5

	.end _start
