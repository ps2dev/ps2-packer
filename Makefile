
CC = gcc
CPPFLAGS = -O3 -Wall
LIBZA = /usr/lib/libz.a

all: ps2-packer stubs

ps2-packer: ps2-packer.c
	$(CC) $(CPPFLAGS) ps2-packer.c -o ps2-packer $(LIBZA)

stubs:
	make -C stub

#
# This target will produce stripped stubs loaders.
#
stubs-dist:
	make -C stub dist

clean:
	rm -f ps2-packer ps2-packer.exe *.zip *.gz
	make -C stub clean

rebuild: clean all



#
# Everything below is for me, building the distribution packages.
#

mingw: ps2-packer.exe

ps2-packer.exe: ps2-packer.c
	i586-mingw32msvc-gcc $(CPPFLAGS) ps2-packer.c -o ps2-packer.exe -I mingw-zlib -I mingw-getopt mingw-zlib/*.c mingw-getopt/getopt*.c

dist: ps2-packer ps2-packer.exe COPYING stubs-dist README.txt ps2-packer.c
	strip ps2-packer
	i586-mingw32msvc-strip ps2-packer.exe
	upx-nrv --force ps2-packer ps2-packer.exe
	tar cvfz ps2-packer-0.2.1-linux.tar.gz ps2-packer COPYING stub/zlib*stub README.txt
	zip -9 ps2-packer-0.2.1-win32.zip ps2-packer.exe COPYING stub/zlib*stub README.txt
	tar cvfz ps2-packer-0.2.1-src.tar.gz ps2-packer.c Makefile COPYING stub/Makefile stub/main.c stub/crt0.s stub/linkfile stub/zlib/Makefile stub/zlib/*.{c,h} README.txt

redist: clean dist

release: redist
	rm -f /var/www/ps2-packer/*
	cp *.gz *.zip COPYING README.txt /var/www/ps2-packer
