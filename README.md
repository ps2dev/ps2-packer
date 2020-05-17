# ps2-packer

![CI](https://github.com/ps2dev/ps2-packer/workflows/CI/badge.svg)

# Overview

Just like [UPX](http://upx.sourceforge.net/ "UPX"), this tool is designed to help you
create `packed ELF` to run on the `PS2`. 

# Modules
Is being done following a modular design, so anybody can write any kind of module to it. It actually contains:
 - `zlib` module.
 - `lzo` module.
 - `lz4` module.
 - `lzma` module. 
 - Three `ucl` modules (n2b, n2d and n2e)
 - `null` module, for demo purpose only.

# TODO
  - Changing current module design to pass on arguments.
  - Write a proper documentation about "how to write new modules".
  - Add RC4 modules.

# History

Well, I wrote this piece of junk in one day, because Drakonite said me zlib
would be better than lzo, and that it would be quite a challenge to get it
working for PS2. I wanted to see if he was right :)


# Source code and legal stuff

  This code is covered by GPL. Actually, I don't give a shit about licenses
and stuff. If you don't like it covered by GPL, just ask me, and we'll change
it. The only problem is it uses modified version of a lot of GPLed code, so...

  This code was inspired by various sources, especially sjcrunch's main.c, and
sjuncrunch. Some idea from mrbrown too :)

  Beeing a ps2dev.org developper got me banned from ps2ownz.com's website. Thus,
as a special exception to the GPL, since I can not go on their forums, and react
and help people about that software, I to NOT give them the autorization to
mirror these packages on their website, only to link to the homepage of this
project, that is, http://www.nobis-crew.org/ps2-packer nor are they authorized
to support their users on their forums on questions about that software.

  If you want to reach me, and find support about ps2-packer, either ask me
directly, by mail, or by reaching me on IRC (channel #ps2dev on EfNet), or ask
your questions on ps2dev.org's forums and ps2-scene's forums.

  I actually know they won't give a shit about these restrictions, but I felt
like proceeding so. If you want real and *legit* ps2 development, go on the
genuine ps2dev website, that is, http://ps2dev.org


# How it works
```
  Usage: ps2-packer [-v] [-b X] [-p X] [-s X] [-a X] [-r X] <in_elf> <out_elf>
  
  Options description:
    -v             verbose mode.
    -b base        sets the loading base of the compressed data. When activated
                      it will activate the alternative packing way. See below.
    -p packer      sets a packer name. lzma by default.
    -s stub        sets another uncruncher stub. stub/lzma-1d00-stub by default,
                      or stub/lzma-0088-stub when using alternative packing.
    -r reload      sets a reload base of the stub. Beware, that will only works
                      with the special asm stubs.
    -a align       sets section alignment. 16 by default. Any value accepted.
                      However, here is a list of alignments to know:
		1 - no alignment, risky, shouldn't work.
		4 - 32-bits alignment, tricky, should on certain loaders.
	       16 - 128-bits alignment, normal, should work everywhere.
	      128 - 128-bytes alignment, dma-safe.
	     4096 - supra-safe, this is the default alignment of binutils.
```

Now, you have to understand the mechanisms.
  
  In normal mode, the output elf will contain one program section. Its loading
location depends on the selected stub. For example, with a stub loading at
0x1d00000, compressed data will be located *below* that address. I provide

  However, if you specify a base loading address on the command line, the data
will be forced to reside at a certain location. That's the alternative packing
method. The output elf will contain two program sections. The first one will
be the uncruncher stub. The second section contains the packed data, loaded at
the address you specified.

  The reload option is meant to forcibily relink the stub to another address.
This will only work with the asm stubs though; be careful when using it.

  So, depending on your needs, just move the data around, to get the desired
results.


# Examples
```
~$ ./ps2-packer ./ps2menu.elf ./ps2menu-packed.elf
PS2-Packer v0.3 (C) 2004 Nicolas "Pixel" Noble
This is free software with ABSOLUTELY NO WARRANTY.

Loading stub file.
Stub PC = 01D00008
Loaded stub: 0000057C bytes (with 00000204 zeroes) based at 01D00000
Opening packer.
Preparing output elf file.
Packing.
ELF PC = 00100008
Loaded section: 0006B438 bytes (with 0033CB72 zeroes) based at 00100000
Section packed, from 439350 to 162565 bytes, ratio = 63.00%
Final base address: 01CD84E0
Writing stub.
All data written, writing program header.
Done!
```

```
~$ ./ps2-packer -b 0x1b00000 ./ps2menu.elf ./ps2menu-packed-alt.elf
PS2-Packer v0.3 (C) 2004 Nicolas "Pixel" Noble
This is free software with ABSOLUTELY NO WARRANTY.

Using alternative packing method.
Loading stub file.
Stub PC = 00088008
Loaded stub: 0000057C bytes (with 00000204 zeroes) based at 00088000
Opening packer.
Preparing output elf file.
Actual pointer in file = 000005FC
Realigned pointer in file = 00000600
Packing.
ELF PC = 00100008
Loaded section: 0006B438 bytes (with 0033CB72 zeroes) based at 00100000
Section packed, from 439350 to 162565 bytes, ratio = 63.00%
All data written, writing program header.
Done!
```

```
$ ./ps2-packer -p lzo ./ps2menu.elf ./ps2menu-packed-lzo.elf
PS2-Packer v0.3 (C) 2004 Nicolas "Pixel" Noble
This is free software with ABSOLUTELY NO WARRANTY.

Loading stub file.
Stub PC = 01D00008
Loaded stub: 000006FC bytes (with 00000204 zeroes) based at 01D00000
Opening packer.
Preparing output elf file.
Packing.
ELF PC = 00100008
Loaded section: 0006B438 bytes (with 0033CB72 zeroes) based at 00100000
Section packed, from 439350 to 237121 bytes, ratio = 46.03%
Final base address: 01CC61A4
Writing stub.
All data written, writing program header.
Done!
```

# Comparing Results
```
$ ls -l ps2menu.elf ps2menu-packed*
-rw-r--r--    1 pixel    pixel      444240 Aug 12 23:33 ps2menu.elf
-rw-r--r--    1 pixel    pixel      164124 Aug 13 15:07 ps2menu-packed.elf
-rw-r--r--    1 pixel    pixel      164125 Aug 13 15:07 ps2menu-packed-alt.elf
-rw-r--r--    1 pixel    pixel      239064 Aug 13 15:08 ps2menu-packed-lzo.elf
```


# Bugs and limitations

- It's poorly coded :-P
- Stubs have to be in one single program header.


# How to compile

Current compilation options requires `libz.a` and `libucl.a` to reside at
`/usr/lib/libz.a` and `/usr/lib/libucl.a` (I do that only to statically link
the `zlib` and `ucl` with the final software, so it will run everywhere) So, if it
doesn't match your system, change that line into the `Makefile`. Afterward, a
simple `make` should do the trick in order to compile everything, provided you
have the `full ps2 toolchain`, with the `PS2SDK` variable pointing to your `ps2sdk`
directory, and `ee-gcc` under your path.

Don't look at the `dist` target in the `Makefile`, it's only for me to build
the various packages.


# Author
  Nicolas "Pixel" Noble <pixel@nobis-crew.org> - http://www.nobis-crew.org


# Where to find

The "official" webpage for this tool is at on my personal webspace:
  
```
http://www.nobis-crew.org/ps2-packer/
```

However, you can find the latests CVS changes into ps2dev's CVS:
```
http://cvs.ps2dev.org/ps2-packer/
```
  For more informations about it, feel free to go on ps2dev's website located
at http://ps2dev.org/ and be sure to drop by #ps2dev in EfNet.


# Thanks and greetings

  They go to adresd, blackd_wd, drakonite, emoon, gorim, hiryu, herben, jenova,
linuzapp, oobles, oopo, mrbrown, nagra, neov, nik, t0mb0la, tyranid

and maybe a few other people I forgot at #ps2dev :)

  Big special thanks to LavosSpawn who helped me reducing the asm stub ;)
