
LIBZA = /usr/lib/libz.a
LIBUCLA = /usr/lib/libucl.a
VERSION = 0.4
CC = gcc
CPPFLAGS = -O3 -Wall -I. -DVERSION=\"$(VERSION)\"

PACKERS = zlib-packer lzo-packer n2b-packer n2d-packer n2e-packer null-packer

all: ps2-packer packers stubs

ps2-packer: ps2-packer.c dlopen.c
	$(CC) $(CPPFLAGS) ps2-packer.c dlopen.c -o ps2-packer -ldl

stubs:
	make -C stub

packers: $(addsuffix .so,$(PACKERS))

zlib-packer.so: zlib-packer.c
	$(CC) -fPIC $(CPPFLAGS) zlib-packer.c -shared -o zlib-packer.so $(LIBZA)

lzo-packer.so: lzo-packer.c minilzo.c
	$(CC) -fPIC $(CPPFLAGS) lzo-packer.c minilzo.c -shared -o lzo-packer.so

n2b-packer.so: n2b-packer.c
	$(CC) -fPIC $(CPPFLAGS) n2b-packer.c -shared -o n2b-packer.so $(LIBUCLA)

n2d-packer.so: n2d-packer.c
	$(CC) -fPIC $(CPPFLAGS) n2d-packer.c -shared -o n2d-packer.so $(LIBUCLA)

n2e-packer.so: n2e-packer.c
	$(CC) -fPIC $(CPPFLAGS) n2e-packer.c -shared -o n2e-packer.so $(LIBUCLA)

null-packer.so: null-packer.c
	$(CC) -fPIC $(CPPFLAGS) null-packer.c -shared -o null-packer.so


#
# This target will produce stripped stubs loaders.
#
stubs-dist:
	make -C stub dist

clean:
	rm -f ps2-packer ps2-packer.exe *.zip *.gz *.dll *.so *.o
	make -C stub clean

rebuild: clean all



#
# Everything below is for me, building the distribution packages.
#

mingw: ps2-packer.exe mingw-packers

MINGW_LIBGCC = /usr/lib/gcc-lib/i586-mingw32msvc/2.95.3-7/libgcc.a

ps2-packer.exe: ps2-packer.c dlopen.c
	i586-mingw32msvc-gcc $(CPPFLAGS) ps2-packer.c dlopen.c -o ps2-packer.exe -I mingw-getopt mingw-getopt/getopt*.c

mingw-packers: $(addsuffix .dll,$(PACKERS))

mingw-zlib:
	make -C mingw-zlib

mingw-ucl:
	make -C mingw-ucl

mingw-clean:
	make -C mingw-zlib clean
	make -C mingw-ucl clean

dllinit.o: dllinit.c
	i586-mingw32msvc-gcc -c dllinit.c

zlib-packer.dll: zlib-packer.c mingw-zlib dllinit.o
	i586-mingw32msvc-gcc -c zlib-packer.c -I mingw-zlib
	echo EXPORTS > tmp.def
	i586-mingw32msvc-nm zlib-packer.o dllinit.o | grep '^........ [T] _' | sed 's/[^_]*_//' >> tmp.def
	i586-mingw32msvc-ld --base-file tmp.base --dll -o zlib-packer.dll zlib-packer.o dllinit.o -e _DllMain@12 mingw-zlib/zlib.a -lmingw32 -lkernel32 -lmoldname -lmsvcrt $(MINGW_LIBGCC)
	i586-mingw32msvc-dlltool --dllname zlib-packer.dll --def tmp.def --base-file tmp.base --output-exp tmp.exp
	i586-mingw32msvc-ld --base-file tmp.base tmp.exp --dll -o zlib-packer.dll zlib-packer.o dllinit.o -e _DllMain@12 mingw-zlib/zlib.a -lmingw32 -lkernel32 -lmoldname -lmsvcrt $(MINGW_LIBGCC)
	rm tmp.base tmp.exp tmp.def zlib-packer.o

lzo-packer.dll: lzo-packer.c dllinit.o
	i586-mingw32msvc-gcc -c lzo-packer.c minilzo.c
	echo EXPORTS > tmp.def
	i586-mingw32msvc-nm lzo-packer.o dllinit.o | grep '^........ [T] _' | sed 's/[^_]*_//' >> tmp.def
	i586-mingw32msvc-ld --base-file tmp.base --dll -o lzo-packer.dll lzo-packer.o minilzo.o dllinit.o -e _DllMain@12 -lmingw32 -lkernel32 -lmoldname -lmsvcrt $(MINGW_LIBGCC)
	i586-mingw32msvc-dlltool --dllname zlib-packer.dll --def tmp.def --base-file tmp.base --output-exp tmp.exp
	i586-mingw32msvc-ld --base-file tmp.base tmp.exp --dll -o lzo-packer.dll lzo-packer.o minilzo.o dllinit.o -e _DllMain@12 -lmingw32 -lkernel32 -lmoldname -lmsvcrt $(MINGW_LIBGCC)
	rm tmp.base tmp.exp tmp.def lzo-packer.o minilzo.o

n2b-packer.dll: n2b-packer.c mingw-ucl dllinit.o
	i586-mingw32msvc-gcc -c n2b-packer.c -I mingw-ucl
	echo EXPORTS > tmp.def
	i586-mingw32msvc-nm n2b-packer.o dllinit.o | grep '^........ [T] _' | sed 's/[^_]*_//' >> tmp.def
	i586-mingw32msvc-ld --base-file tmp.base --dll -o n2b-packer.dll n2b-packer.o dllinit.o -e _DllMain@12 mingw-ucl/ucl.a -lmingw32 -lkernel32 -lmoldname -lmsvcrt $(MINGW_LIBGCC)
	i586-mingw32msvc-dlltool --dllname n2b-packer.dll --def tmp.def --base-file tmp.base --output-exp tmp.exp
	i586-mingw32msvc-ld --base-file tmp.base tmp.exp --dll -o n2b-packer.dll n2b-packer.o dllinit.o -e _DllMain@12 mingw-ucl/ucl.a -lmingw32 -lkernel32 -lmoldname -lmsvcrt $(MINGW_LIBGCC)
	rm tmp.base tmp.exp tmp.def n2b-packer.o

n2d-packer.dll: n2d-packer.c mingw-ucl dllinit.o
	i586-mingw32msvc-gcc -c n2d-packer.c -I mingw-ucl
	echo EXPORTS > tmp.def
	i586-mingw32msvc-nm n2d-packer.o dllinit.o | grep '^........ [T] _' | sed 's/[^_]*_//' >> tmp.def
	i586-mingw32msvc-ld --base-file tmp.base --dll -o n2d-packer.dll n2d-packer.o dllinit.o -e _DllMain@12 mingw-ucl/ucl.a -lmingw32 -lkernel32 -lmoldname -lmsvcrt $(MINGW_LIBGCC)
	i586-mingw32msvc-dlltool --dllname n2d-packer.dll --def tmp.def --base-file tmp.base --output-exp tmp.exp
	i586-mingw32msvc-ld --base-file tmp.base tmp.exp --dll -o n2d-packer.dll n2d-packer.o dllinit.o -e _DllMain@12 mingw-ucl/ucl.a -lmingw32 -lkernel32 -lmoldname -lmsvcrt $(MINGW_LIBGCC)
	rm tmp.base tmp.exp tmp.def n2d-packer.o

n2e-packer.dll: n2e-packer.c mingw-ucl dllinit.o
	i586-mingw32msvc-gcc -c n2e-packer.c -I mingw-ucl
	echo EXPORTS > tmp.def
	i586-mingw32msvc-nm n2e-packer.o dllinit.o | grep '^........ [T] _' | sed 's/[^_]*_//' >> tmp.def
	i586-mingw32msvc-ld --base-file tmp.base --dll -o n2e-packer.dll n2e-packer.o dllinit.o -e _DllMain@12 mingw-ucl/ucl.a -lmingw32 -lkernel32 -lmoldname -lmsvcrt $(MINGW_LIBGCC)
	i586-mingw32msvc-dlltool --dllname n2e-packer.dll --def tmp.def --base-file tmp.base --output-exp tmp.exp
	i586-mingw32msvc-ld --base-file tmp.base tmp.exp --dll -o n2e-packer.dll n2e-packer.o dllinit.o -e _DllMain@12 mingw-ucl/ucl.a -lmingw32 -lkernel32 -lmoldname -lmsvcrt $(MINGW_LIBGCC)
	rm tmp.base tmp.exp tmp.def n2e-packer.o

null-packer.dll: null-packer.c dllinit.o
	i586-mingw32msvc-gcc -c null-packer.c
	echo EXPORTS > tmp.def
	i586-mingw32msvc-nm null-packer.o dllinit.o | grep '^........ [T] _' | sed 's/[^_]*_//' >> tmp.def
	i586-mingw32msvc-ld --base-file tmp.base --dll -o null-packer.dll null-packer.o dllinit.o -e _DllMain@12 -lmingw32 -lkernel32 -lmoldname -lmsvcrt $(MINGW_LIBGCC)
	i586-mingw32msvc-dlltool --dllname null-packer.dll --def tmp.def --base-file tmp.base --output-exp tmp.exp
	i586-mingw32msvc-ld --base-file tmp.base tmp.exp --dll -o null-packer.dll null-packer.o dllinit.o -e _DllMain@12 -lmingw32 -lkernel32 -lmoldname -lmsvcrt $(MINGW_LIBGCC)
	rm tmp.base tmp.exp tmp.def null-packer.o

dist: all mingw COPYING stubs-dist README.txt ps2-packer.c $(addsuffix .c,$(PACKERS))
	strip ps2-packer $(addsuffix .so,$(PACKERS))
	i586-mingw32msvc-strip ps2-packer.exe $(addsuffix .dll,$(PACKERS))
	upx-nrv --best ps2-packer ps2-packer.exe $(addsuffix .dll,$(PACKERS))
	tar cvfz ps2-packer-$(VERSION)-linux.tar.gz ps2-packer $(addsuffix .so,$(PACKERS)) COPYING stub/*stub README.txt
	zip -9 ps2-packer-$(VERSION)-win32.zip ps2-packer.exe $(addsuffix .dll,$(PACKERS)) COPYING stub/*stub README.txt
	tar cvfz ps2-packer-$(VERSION)-src.tar.gz *.{c,h} Makefile COPYING stub/{Makefile,crt0.s,linkfile,*.{c,h,S}} stub/ucl/*.S stub/{zlib,lzo,ucl}/{Makefile,*.{c,h}} README.txt

redist: clean mingw-clean dist

release: redist
	rm -f /var/www/ps2-packer/*
	cp *.gz *.zip COPYING README.txt /var/www/ps2-packer

.PHONY: mingw-zlib mingw-ucl
