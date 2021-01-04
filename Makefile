PREFIX = $(PS2DEV)

SUBMAKE = $(MAKE) -C
SHELL = /bin/sh
SYSTEM = $(shell uname)
LIBZA = -lz
LIBLZMAA = lzma/lzma.a
LZMA_MT ?= 1
ifeq ($(LZMA_MT),1)
	LIBLZMAA += -lpthread
endif
LZMA_CPPFLAGS = -I common/lzma
VERSION = 1.1.0
BIN2C = $(PS2SDK)/bin/bin2c
CPPFLAGS := -O3 -Wall -I. -DVERSION=\"$(VERSION)\" -DPREFIX=\"$(PREFIX)\" $(CPPFLAGS)
INSTALL = install

ifeq ($(SYSTEM),Darwin)
	CPPFLAGS += -D__APPLE__
	SHARED = -dynamiclib
	SHAREDSUFFIX = .dylib
	CC = /usr/bin/gcc
	CPPFLAGS += -I/usr/local/include -L/usr/local/lib
else ifeq ($(OS),Windows_NT)
	SHAREDSUFFIX = .dll
	EXECSUFFIX = .exe
	DIST_PACK_CMD = zip -9
	DIST_PACK_EXT = .zip
	LDFLAGS = #Libdl is built into glibc for both Cygwin and MinGW.
else ifeq ($(findstring BSD, $(SYSTEM)), BSD)
	ifeq ($(SYSTEM),NetBSD)
		CPPFLAGS += -I/usr/pkg/include -L/usr/pkg/lib  -R/usr/pkg/lib
	else
		CPPFLAGS += -I/usr/local/include -L/usr/local/lib
	endif
	CC = cc
	LDFLAGS =
endif

CC ?= gcc
SHARED ?= -shared
SHAREDSUFFIX ?= .so
EXECSUFFIX ?=
DIST_PACK_CMD ?= tar cvfz
DIST_PACK_EXT ?= .tar.gz
LDFLAGS ?= -ldl

PACKERS = zlib-packer lzo-packer lz4-packer lzma-packer null-packer

all: ps2-packer ps2-packer-lite packers stubs

install: all
	$(INSTALL) -d $(DESTDIR)$(PREFIX)/bin
	$(INSTALL) -d $(DESTDIR)$(PREFIX)/share/ps2-packer/module
	$(INSTALL) -d $(DESTDIR)$(PREFIX)/share/ps2-packer/stub
	$(INSTALL) -m 755 ps2-packer $(DESTDIR)$(PREFIX)/bin
	$(INSTALL) -m 755 $(addsuffix $(SHAREDSUFFIX),$(PACKERS)) $(DESTDIR)$(PREFIX)/share/ps2-packer/module
	$(INSTALL) -m 755 ps2-packer-lite $(DESTDIR)$(PREFIX)/bin
	PREFIX=$(PREFIX) $(SUBMAKE) stub install

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/ps2-packer$(EXECSUFFIX) $(DESTDIR)$(PREFIX)/bin/ps2-packer-lite$(EXECSUFFIX)
	rm -rf $(DESTDIR)$(PREFIX)/share/ps2-packer

ps2-packer: ps2-packer.c dlopen.c
	$(CC) $(CPPFLAGS) ps2-packer.c dlopen.c $(LDFLAGS) -o ps2-packer$(EXECSUFFIX)

ps2-packer-lite: ps2-packer.c builtin_stub.o lzma
	$(CC) $(CPPFLAGS) $(LZMA_CPPFLAGS) -DPS2_PACKER_LITE ps2-packer.c lzma-packer.c $(LIBLZMAA) builtin_stub.o $(LDFLAGS) -o ps2-packer-lite$(EXECSUFFIX)

builtin_stub.c: stubs-tag.stamp
	cp stub/lzma-1d00-stub ./b_stub
	$(BIN2C) b_stub builtin_stub.c builtin_stub
	rm b_stub

lzma: lzma-tag.stamp

lzma-tag.stamp:
	$(SUBMAKE) lzma
	touch lzma-tag.stamp

stubs: stubs-tag.stamp

stubs-tag.stamp:
	$(SUBMAKE) stub
	touch stubs-tag.stamp

packers: $(addsuffix $(SHAREDSUFFIX),$(PACKERS))

zlib-packer$(SHAREDSUFFIX): zlib-packer.c
	$(CC) -fPIC $(CPPFLAGS) zlib-packer.c $(SHARED) -o zlib-packer$(SHAREDSUFFIX) $(LIBZA)

lzo-packer$(SHAREDSUFFIX): lzo-packer.c minilzo.c
	$(CC) -fPIC $(CPPFLAGS) lzo-packer.c minilzo.c $(SHARED) -o lzo-packer$(SHAREDSUFFIX)

lz4-packer$(SHAREDSUFFIX): lz4-packer.c stub/lz4/lz4.c stub/lz4/lz4hc.c
	$(CC) -fPIC $(CPPFLAGS) -Istub/lz4 lz4-packer.c stub/lz4/lz4.c stub/lz4/lz4hc.c $(SHARED) -o lz4-packer$(SHAREDSUFFIX)

lzma-packer$(SHAREDSUFFIX): lzma lzma-packer.c
	$(CC) -fPIC $(CPPFLAGS) $(LZMA_CPPFLAGS) lzma-packer.c $(SHARED) -o lzma-packer$(SHAREDSUFFIX) $(LIBLZMAA)

null-packer$(SHAREDSUFFIX): null-packer.c
	$(CC) -fPIC $(CPPFLAGS) null-packer.c $(SHARED) -o null-packer$(SHAREDSUFFIX)


#
# This target will produce stripped stubs loaders.
#
stubs-dist:
	$(SUBMAKE) stub dist

clean:
	rm -f ps2-packer ps2-packer-lite ps2-packer.exe ps2-packer-lite.exe *.zip *.gz *.dll builtin_stub.c *$(SHAREDSUFFIX) *.o
	$(SUBMAKE) lzma clean
	rm -f lzma-tag.stamp
	$(SUBMAKE) stub clean
	rm -f stubs-tag.stamp

rebuild: clean all



#
# Everything below is for me, building the distribution packages.
#

dist: all COPYING stubs-dist README.txt ps2-packer.c $(addsuffix .c,$(PACKERS))
	strip ps2-packer$(EXECSUFFIX) ps2-packer-lite$(EXECSUFFIX) $(addsuffix $(SHAREDSUFFIX),$(PACKERS))
	$(DIST_PACK_CMD) ps2-packer-$(VERSION)$(DIST_PACK_EXT) ps2-packer$(EXECSUFFIX) $(addsuffix $(SHAREDSUFFIX),$(PACKERS)) COPYING stub/*stub README.txt
	$(DIST_PACK_CMD) ps2-packer-lite-$(VERSION)$(DIST_PACK_EXT) ps2-packer-lite$(EXECSUFFIX) COPYING README.txt README-lite.txt
	tar cvfz ps2-packer-$(VERSION)-src.tar.gz *.{c,h} Makefile COPYING stub/{Makefile,crt0.s,linkfile,*.{c,h,S}} stub/{zlib,lzo}/{Makefile,*.{c,h}} README.txt README-lite.txt

redist: clean dist

release: redist
	rm -f /var/www/softwares/ps2-packer/*
	cp *.gz *.zip COPYING README.txt README-lite.txt /var/www/softwares/ps2-packer
