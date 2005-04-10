SUBMAKE = MAKE=$(MAKE) $(MAKE) -C

include $(PS2SDK)/Defs.make

EE_CPPFLAGS = -G0 -I $(PS2SDK)/ee/include -I $(PS2SDK)/common/include -I . -I zlib -I lzo -I ucl -D_EE -O3 -Wall -DDO_EXECPS2
EE_LDFLAGS = -T ./linkfile -L $(PS2SDK)/ee/lib -lc -lkernel -lc -lsyscall

TARGETS = \
zlib-1d00-stub zlib-0088-stub \
null-1d00-stub null-0088-stub \
null-kmode-1d00-stub null-kmode-0088-stub \
lzo-1d00-stub lzo-0088-stub \
lzo-kmode-1d00-stub lzo-kmode-0088-stub \
n2b-1d00-stub n2b-0088-stub \
n2b-kmode-1d00-stub n2b-kmode-0088-stub \
n2d-1d00-stub n2d-0088-stub \
n2d-kmode-1d00-stub n2d-kmode-0088-stub \
n2e-1d00-stub n2e-0088-stub \
n2e-kmode-1d00-stub n2e-kmode-0088-stub \
n2e-asm-1d00-stub n2e-asm-one-1d00-stub

all: $(TARGETS)

install: all
	install $(TARGETS) $(PREFIX)/share/ps2-packer/stub -m 644

dist: $(TARGETS)
	ee-strip $(TARGETS)

zlib-tag.stamp:
	$(SUBMAKE) zlib
	touch zlib-tag.stamp

lzo-tag.stamp:
	$(SUBMAKE) lzo
	touch lzo-tag.stamp

ucl-tag.stamp:
	$(SUBMAKE) ucl
	touch ucl-tag.stamp


ZLIB_SIGNATURE = -Wl,--defsym,_signature=0x42494c5a
NULL_SIGNATURE = -Wl,--defsym,_signature=0x4c4c554e
LZO_SIGNATURE = -Wl,--defsym,_signature=0x314f5a4c
N2B_SIGNATURE = -Wl,--defsym,_signature=0x3142324e
N2D_SIGNATURE = -Wl,--defsym,_signature=0x3144324e
N2E_SIGNATURE = -Wl,--defsym,_signature=0x3145324e

zlib-0088-stub: main.o zlib-stub.o zlib-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x0088000 -o zlib-0088-stub main.o zlib-stub.o ./zlib/zlib.a $(EE_LDFLAGS) $(ZLIB_SIGNATURE)

zlib-1d00-stub: main.o zlib-stub.o zlib-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x1d00000 -o zlib-1d00-stub main.o zlib-stub.o ./zlib/zlib.a $(EE_LDFLAGS) $(ZLIB_SIGNATURE)


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


n2b-0088-stub: main.o n2b-stub.o ucl-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x0088000 -o n2b-0088-stub main.o n2b-stub.o ./ucl/ucl.a $(EE_LDFLAGS) $(N2B_SIGNATURE)

n2b-1d00-stub: main.o n2b-stub.o ucl-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x1d00000 -o n2b-1d00-stub main.o n2b-stub.o ./ucl/ucl.a $(EE_LDFLAGS) $(N2B_SIGNATURE)


n2b-kmode-0088-stub: main-kmode.o n2b-stub.o ucl-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x0088000 -o n2b-kmode-0088-stub main-kmode.o n2b-stub.o ./ucl/ucl.a $(EE_LDFLAGS) $(N2B_SIGNATURE)

n2b-kmode-1d00-stub: main-kmode.o n2b-stub.o ucl-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x1d00000 -o n2b-kmode-1d00-stub main-kmode.o n2b-stub.o ./ucl/ucl.a $(EE_LDFLAGS) $(N2B_SIGNATURE)


n2d-0088-stub: main.o n2d-stub.o ucl-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x0088000 -o n2d-0088-stub main.o n2d-stub.o ./ucl/ucl.a $(EE_LDFLAGS) $(N2D_SIGNATURE)

n2d-1d00-stub: main.o n2d-stub.o ucl-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x1d00000 -o n2d-1d00-stub main.o n2d-stub.o ./ucl/ucl.a $(EE_LDFLAGS) $(N2D_SIGNATURE)


n2d-kmode-0088-stub: main-kmode.o n2d-stub.o ucl-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x0088000 -o n2d-kmode-0088-stub main-kmode.o n2d-stub.o ./ucl/ucl.a $(EE_LDFLAGS) $(N2D_SIGNATURE)

n2d-kmode-1d00-stub: main-kmode.o n2d-stub.o ucl-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x1d00000 -o n2d-kmode-1d00-stub main-kmode.o n2d-stub.o ./ucl/ucl.a $(EE_LDFLAGS) $(N2D_SIGNATURE)


n2e-0088-stub: main.o n2e-stub.o ucl-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x0088000 -o n2e-0088-stub main.o n2e-stub.o ./ucl/ucl.a $(EE_LDFLAGS) $(N2E_SIGNATURE)

n2e-1d00-stub: main.o n2e-stub.o ucl-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x1d00000 -o n2e-1d00-stub main.o n2e-stub.o ./ucl/ucl.a $(EE_LDFLAGS) $(N2E_SIGNATURE)


n2e-kmode-0088-stub: main-kmode.o n2e-stub.o ucl-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x0088000 -o n2e-kmode-0088-stub main-kmode.o n2e-stub.o ./ucl/ucl.a $(EE_LDFLAGS) $(N2E_SIGNATURE)

n2e-kmode-1d00-stub: main-kmode.o n2e-stub.o ucl-tag.stamp
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x1d00000 -o n2e-kmode-1d00-stub main-kmode.o n2e-stub.o ./ucl/ucl.a $(EE_LDFLAGS) $(N2E_SIGNATURE)


n2e-asm-1d00-stub: n2e-asm-stub.S ucl/n2e.S
	$(EE_CC) -nostartfiles -Wl,--defsym,_start_address=0x1d00000 -o n2e-asm-1d00-stub n2e-asm-stub.S $(EE_LDFLAGS) $(N2E_SIGNATURE) -DARGS_IN_S

n2e-asm-one-1d00-stub: n2e-asm-one-stub.S ucl/n2e.S
	$(EE_CC) -nostartfiles -Wl,--defsym,_start_address=0x1d00000 -o n2e-asm-one-1d00-stub n2e-asm-one-stub.S $(EE_LDFLAGS) $(N2E_SIGNATURE) -DARGS_IN_S


clean:
	rm -f *.o $(TARGETS) *-tag.stamp
	$(SUBMAKE) zlib clean
	$(SUBMAKE) lzo clean
	$(SUBMAKE) ucl clean

rebuild: clean all

%.o : %.c
	$(EE_CC) $(EE_CFLAGS) $(EE_CPPFLAGS) $(EE_INCS) -c $< -o $@

