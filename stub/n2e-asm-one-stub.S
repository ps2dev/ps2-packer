.global _start
.global PackedELF
.global Signature

	.extern _signature

#include "ucl/n2e.S"

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
	
	lw	$s1, 12($s2)  // virtual address == destination
	addiu	$s0, $s2, 16 // skip entry point, original size, zero byte size, virtual address, == source
	lw	$s3, 4($s2)
	addu	$s4, $s3, $s1
	bal	n2e_decompress
	
	lw	$s0, 8($s2)

	.set noreorder
loop:
	sb	$0, 0($s4)
	addiu   $s4, 1
	bnez	$s0, loop
	addiu	$s0, -1
	
	.set reorder
	
	lw	$s2, 0($s2)  // entry point
	jr	$s2

	.end _start
