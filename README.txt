PS2-Packer version 0.2.1
========================

Overview
--------

  This (very preliminary) tool is designed to help you create packed ELF to
run on the PS2. It actually only works with zlib, and has a poor design, but
I hope to have a modular system afterward, so people could write "plugins"
with new stubs and new compressors.


Changelog
---------

  2004/08/03: release of version 0.1, first version.
  2004/08/03: release of version 0.1.1, included zlib, removing bloats :)
  2004/08/04: disabled the buggy "fast" memzero in the stub
              worked out an endian independant version.
	      release of version 0.2 :-P (yeah, okay, okay, a bit too fast...)
  2004/08/05: commenting the source, putting it into ps2dev's CVS.


History
-------

  Well, I wrote this piece of junk in one day, because Drakonite said me zlib
would be better than lzo, and that it would be quite a challenge to get it
working for PS2. I wanted to see if he was right :)


Source code and legal stuff
---------------------------

  This code is covered by GPL. Actually, I don't give a shit about licenses
and stuff. If you don't like it covered by GPL, just ask me, and we'll change
it. The only problem is it uses getopt, and the win32 source code for it is
GPL, so...

  This code was inspired by various sources, especially sjcrunch's main.c, and
sjuncrunch. Some idea from mrbrown too :)


How it works
------------

  Usage line: ps2-packer [-l level] [-b base] [-s stub] <in_elf> <out_elf>
  
  Explanation of the options:
    -l level       sets the zlib compression level.
                      9 by default.
    -b base        sets the loading base of the compressed data.
                      0x1b00000 by default.
    -s stub        sets another zlib uncruncher stub.
                      stub/zlib-0088-stub by default.

  Now, you have to understand the mechanism. The output elf will contain two
program sections. The first one will be the uncruncher stub. By default, it's
a zlib stub compiled to be loaded at 0x88000. You may want to compile other
stubs, to get different loading addresses. I provide another stub which loads
at 0x1a00000. Should you produce new stubs, just change the stub/Makefile.
The second section contains the packed data. This basically can be loaded
anywhere. That is the "base" option of the command line.

  So, depending on your needs, just move the data around, to get the desired
results.


Bugs and limitations
--------------------

-) It's hardly hardcoded to work with zlib. Should get around it, but, since
   I am a lazy bastard, this can take ages.
-) It's poorly coded :-P


How to compile
--------------

  My compilation options requires libz.a inside /usr/lib/libz.a (I do that
only to statically link the zlib with the final software, so it will run
everywhere) So, if it doesn't match your system, change that line into the
Makefile. Afterward, a simple "make" should do the trick in order to compile
everything, provided you have the full ps2 toolchain, with the PS2SDK variable
pointing to your ps2sdk directory, and ee-gcc under your path.

  Don't look at the 'dist' target in the Makefile, it's only for me to build
the various packages.


Author
------

  Nicolas "Pixel" Noble <pixel@nobis-crew.org> - http://www.nobis-crew.org


Thanks and greetings
--------------------

  They go to adresd, blackd_wd, drakonite, emoon, gorim, hiryu, jenova, linuzapp
oobles, oopo, mrbrown, nagra, neov, nik, t0mb0la, tyranid

and maybe a few other people I forgot at #ps2dev :)
