SUBMAKE = $(MAKE) -C

include $(PS2SDK)/Defs.make

EE_CPPFLAGS = -G0 -I $(PS2SDK)/ee/include -I $(PS2SDK)/common/include -I . -I zlib -I lzo -I lz4 -I ../common/lzma -D_EE -O3 -Wall -DDO_EXECPS2
EE_LDFLAGS = -T ./linkfile -L $(PS2SDK)/ee/lib -lc -lkernel-nopatch

TARGETS = \
zlib-1d00-stub zlib-0088-stub \
zlib-kmode-1d00-stub zlib-kmode-0088-stub \
null-1d00-stub null-0088-stub \
null-kmode-1d00-stub null-kmode-0088-stub \
lzo-1d00-stub lzo-0088-stub \
lzo-kmode-1d00-stub lzo-kmode-0088-stub \
lz4-1d00-stub lz4-0088-stub \
lz4-kmode-1d00-stub lz4-kmode-0088-stub \
lzma-1d00-stub lzma-0088-stub \
lzma-kmode-1d00-stub lzma-kmode-0088-stub \

all: $(TARGETS)

install: all
	install -m 644 $(TARGETS) $(DESTDIR)$(PREFIX)/share/ps2-packer/stub

dist: $(TARGETS)
	ee-strip $(TARGETS)

zlib-tag.stamp:
	$(SUBMAKE) zlib
	touch zlib-tag.stamp

lzo-tag.stamp:
	$(SUBMAKE) lzo
	touch lzo-tag.stamp

lz4-tag.stamp:
	$(SUBMAKE) lz4
	touch lz4-tag.stamp

lzma-tag.stamp:
	$(SUBMAKE) lzma
	touch lzma-tag.stamp


ZLIB_SIGNATURE = -Wl,--defsym,_signature=0x42494c5a
NULL_SIGNATURE = -Wl,--defsym,_signature=0x4c4c554e
LZO_SIGNATURE = -Wl,--defsym,_signature=0x314f5a4c
LZ4_SIGNATURE = -Wl,--defsym,_signature=0x314fffff
LZMA_SIGNATURE = -Wl,--defsym,_signature=0x414d5a4c

zlib-0088-stub: main.o zlib-stub.o zlib-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x0088000 -o zlib-0088-stub main.o zlib-stub.o ./zlib/zlib.a $(EE_LDFLAGS) $(ZLIB_SIGNATURE)

zlib-1d00-stub: main.o zlib-stub.o zlib-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x1d00000 -o zlib-1d00-stub main.o zlib-stub.o ./zlib/zlib.a $(EE_LDFLAGS) $(ZLIB_SIGNATURE)


zlib-kmode-0088-stub: main-kmode.o zlib-stub.o zlib-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x0088000 -o zlib-kmode-0088-stub main-kmode.o zlib-stub.o ./zlib/zlib.a $(EE_LDFLAGS) $(ZLIB_SIGNATURE)

zlib-kmode-1d00-stub: main-kmode.o zlib-stub.o zlib-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x1d00000 -o zlib-kmode-1d00-stub main-kmode.o zlib-stub.o ./zlib/zlib.a $(EE_LDFLAGS) $(ZLIB_SIGNATURE)


null-0088-stub: main.o null-stub.o
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x0088000 -o null-0088-stub main.o null-stub.o $(EE_LDFLAGS) $(NULL_SIGNATURE)

null-1d00-stub: main.o null-stub.o
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x1d00000 -o null-1d00-stub main.o null-stub.o $(EE_LDFLAGS) $(NULL_SIGNATURE)


null-kmode-0088-stub: main-kmode.o null-stub.o
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x0088000 -o null-kmode-0088-stub main-kmode.o null-stub.o $(EE_LDFLAGS) $(NULL_SIGNATURE)

null-kmode-1d00-stub: main-kmode.o null-stub.o
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x1d00000 -o null-kmode-1d00-stub main-kmode.o null-stub.o $(EE_LDFLAGS) $(NULL_SIGNATURE)


lzo-0088-stub: main.o lzo-stub.o lzo-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x0088000 -o lzo-0088-stub main.o lzo-stub.o ./lzo/lzo.a $(EE_LDFLAGS) $(LZO_SIGNATURE)

lzo-1d00-stub: main.o lzo-stub.o lzo-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x1d00000 -o lzo-1d00-stub main.o lzo-stub.o ./lzo/lzo.a $(EE_LDFLAGS) $(LZO_SIGNATURE)


lzo-kmode-0088-stub: main-kmode.o lzo-stub.o lzo-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x0088000 -o lzo-kmode-0088-stub main-kmode.o lzo-stub.o ./lzo/lzo.a $(EE_LDFLAGS) $(LZO_SIGNATURE)

lzo-kmode-1d00-stub: main-kmode.o lzo-stub.o lzo-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x1d00000 -o lzo-kmode-1d00-stub main-kmode.o lzo-stub.o ./lzo/lzo.a $(EE_LDFLAGS) $(LZO_SIGNATURE)


lz4-0088-stub: main.o lz4-stub.o lz4-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x0088000 -o lz4-0088-stub main.o lz4-stub.o ./lz4/lz4.a $(EE_LDFLAGS) $(LZ4_SIGNATURE)

lz4-1d00-stub: main.o lz4-stub.o lz4-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x1d00000 -o lz4-1d00-stub main.o lz4-stub.o ./lz4/lz4.a $(EE_LDFLAGS) $(LZ4_SIGNATURE)


lz4-kmode-0088-stub: main-kmode.o lz4-stub.o lz4-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x0088000 -o lz4-kmode-0088-stub main-kmode.o lz4-stub.o ./lz4/lz4.a $(EE_LDFLAGS) $(LZ4_SIGNATURE)

lz4-kmode-1d00-stub: main-kmode.o lz4-stub.o lz4-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x1d00000 -o lz4-kmode-1d00-stub main-kmode.o lz4-stub.o ./lz4/lz4.a $(EE_LDFLAGS) $(LZ4_SIGNATURE)


lzma-0088-stub: main.o lzma-stub.o lzma-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x0088000 -o lzma-0088-stub main.o lzma-stub.o ./lzma/lzma.a $(EE_LDFLAGS) $(LZMA_SIGNATURE)

lzma-1d00-stub: main.o lzma-stub.o lzma-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x1d00000 -o lzma-1d00-stub main.o lzma-stub.o ./lzma/lzma.a $(EE_LDFLAGS) $(LZMA_SIGNATURE)


lzma-kmode-0088-stub: main-kmode.o lzma-stub.o lzma-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x0088000 -o lzma-kmode-0088-stub main-kmode.o lzma-stub.o ./lzma/lzma.a $(EE_LDFLAGS) $(LZMA_SIGNATURE)

lzma-kmode-1d00-stub: main-kmode.o lzma-stub.o lzma-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x1d00000 -o lzma-kmode-1d00-stub main-kmode.o lzma-stub.o ./lzma/lzma.a $(EE_LDFLAGS) $(LZMA_SIGNATURE)


clean:
	rm -f *.o $(TARGETS) *-tag.stamp
	$(SUBMAKE) zlib clean
	$(SUBMAKE) lzo clean
	$(SUBMAKE) lz4 clean
	$(SUBMAKE) lzma clean

rebuild: clean all

%.o : %.c
	$(EE_CC) $(EE_CFLAGS) $(EE_CPPFLAGS) $(EE_INCS) -c $< -o $@

