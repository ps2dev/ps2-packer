PS2-Packer version 0.3b2
========================

Overview
--------

  This tool is designed to help you create packed ELF to run on the PS2.
It has a modular design, so anybody could write any kind of module to it.
It actually has a zlib module, a lzo module, three ucl modules (n2b, n2d and
n2e) and a null module, for demo purpose only.


Changelog
---------

  2004/08/03: release of version 0.1, first version.
  2004/08/03: release of version 0.1.1, included zlib, removing bloats :)
  2004/08/04: disabled the buggy "fast" memzero in the stub
              worked out an endian independant version.
	      release of version 0.2 :-P (yeah, okay, okay, a bit too fast...)
  2004/08/05: commenting the source, putting it into ps2dev's CVS.
  2004/08/10: removing error messages into zlib, saving a few bytes.
  2004/08/12: adding module capability to the whole, moved code into modules.
              adding "null" module as an example.
	      adding "lzo" module.
	      tagging as version 0.3b
	      adding "ucl" modules (n2b, n2d and n2e algos)
	      tagging as version 0.3b2 (yeah, okay, still a bit fast :D)
	      changing alignment of data sections to 0x80 instead of the
	        standard 0x1000. Caution: may break things.
	      added a small code to remove the extra zeroes at the end of the
	        section, moving them to bss.
	      cleaning up ucl's uncrunching source code.
	      changed default to use n2e algorithm instead of zlib.
	      changed "memzero" in the stubs to a small asm version.


Todo
----

  -) Changing current module design to pass on arguments.
  -) Write a proper documentation about "how to write new modules".
  -) Add RNC and RC4 modules.


History
-------

  Well, I wrote this piece of junk in one day, because Drakonite said me zlib
would be better than lzo, and that it would be quite a challenge to get it
working for PS2. I wanted to see if he was right :)


Source code and legal stuff
---------------------------

  This code is covered by GPL. Actually, I don't give a shit about licenses
and stuff. If you don't like it covered by GPL, just ask me, and we'll change
it. The only problem is it uses modified version of a lot of GPLed code, so...

  This code was inspired by various sources, especially sjcrunch's main.c, and
sjuncrunch. Some idea from mrbrown too :)


How it works
------------

  Usage line: ps2-packer [-b base] [-p packer] [-s stub] <in_elf> <out_elf>
  
  Explanation of the options:
    -b base        sets the loading base of the compressed data.
                      0x1b00000 by default.
    -p packer      sets a new packer module.
                      n2e-packer by default.
    -s stub        sets another zlib uncruncher stub.
                      stub/n2e-0088-stub by default.

  Now, you have to understand the mechanism. The output elf will contain two
program sections. The first one will be the uncruncher stub. By default, it's
a ucl's n2e stub compiled to be loaded at 0x88000. You may want to compile
other stubs, to get different loading addresses. I provide another stub which
loads at 0x1a00000. Should you produce new stubs, just change the stub/Makefile.
The second section contains the packed data. This basically can be loaded
anywhere. That is the "base" option of the command line.

  So, depending on your needs, just move the data around, to get the desired
results.


Bugs and limitations
--------------------

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
