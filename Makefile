
LIBZA = /usr/lib/libz.a
VERSION = 0.3b
CC = gcc
CPPFLAGS = -O3 -Wall -I. -DVERSION=\"$(VERSION)\"

PACKERS = zlib-packer

all: ps2-packer packers stubs

ps2-packer: ps2-packer.c dlopen.c
	$(CC) $(CPPFLAGS) ps2-packer.c dlopen.c -o ps2-packer -ldl

stubs:
	make -C stub

packers: $(addsuffix .so,$(PACKERS))

zlib-packer.so: zlib-packer.c
	$(CC) -fPIC $(CPPFLAGS) zlib-packer.c -shared -o zlib-packer.so $(LIBZA)

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

ps2-packer.exe: ps2-packer.c dlopen.c
	i586-mingw32msvc-gcc $(CPPFLAGS) ps2-packer.c dlopen.c -o ps2-packer.exe -I mingw-getopt mingw-getopt/getopt*.c

mingw-packers: $(addsuffix .dll,$(PACKERS))

mingw-zlib:
	make -C mingw-zlib

dllinit.o: dllinit.c
	i586-mingw32msvc-gcc -c dllinit.c

zlib-packer.dll: zlib-packer.c mingw-zlib dllinit.o
	i586-mingw32msvc-gcc -c zlib-packer.c -I mingw-zlib
	echo EXPORTS > tmp.def
	i586-mingw32msvc-nm zlib-packer.o dllinit.o | grep '^........ [T] _' | sed 's/[^_]*_//' >> tmp.def
	i586-mingw32msvc-ld --base-file tmp.base --dll -o zlib-packer.dll zlib-packer.o dllinit.o -e _DllMain@12 mingw-zlib/zlib.a -lmingw32 -lkernel32 -lmoldname -lmsvcrt
	i586-mingw32msvc-dlltool --dllname zlib-packer.dll --def tmp.def --base-file tmp.base --output-exp tmp.exp
	i586-mingw32msvc-ld --base-file tmp.base tmp.exp --dll -o zlib-packer.dll zlib-packer.o dllinit.o -e _DllMain@12 mingw-zlib/zlib.a -lmingw32 -lkernel32 -lmoldname -lmsvcrt
	rm tmp.base tmp.exp tmp.def

dist: all mingw COPYING stubs-dist README.txt ps2-packer.c $(addsuffix .c,$(PACKERS))
	strip ps2-packer $(addsuffix .so,$(PACKERS))
	i586-mingw32msvc-strip ps2-packer.exe $(addsuffix .dll,$(PACKERS))
	upx-nrv --force ps2-packer ps2-packer.exe $(addsuffix .dll,$(PACKERS))
	tar cvfz ps2-packer-$(VERSION)-linux.tar.gz ps2-packer $(addsuffix .so,$(PACKERS)) COPYING stub/zlib*stub README.txt
	zip -9 ps2-packer-$(VERSION)-win32.zip ps2-packer.exe $(addsuffix .dll,$(PACKERS)) COPYING stub/zlib*stub README.txt
	tar cvfz ps2-packer-$(VERSION)-src.tar.gz *.{c,h} Makefile COPYING stub/Makefile stub/main.c stub/crt0.s stub/linkfile stub/zlib/Makefile stub/zlib/*.{c,h} README.txt

redist: clean dist

release: redist
	rm -f /var/www/ps2-packer/*
	cp *.gz *.zip COPYING README.txt /var/www/ps2-packer

.PHONY: mingw-zlib
