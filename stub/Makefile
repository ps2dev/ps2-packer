include $(PS2SDK)/Defs.make

EE_CPPFLAGS = -G0 -I $(PS2SDK)/ee/include -I $(PS2SDK)/common/include -I . -I zlib -I lzo -I ucl -D_EE -O3 -Wall
EE_LDFLAGS = -T ./linkfile -L $(PS2SDK)/ee/lib -lc -lkernel -lc -lsyscall

TARGETS = \
zlib-1a00-stub zlib-0088-stub \
null-1a00-stub null-0088-stub \
lzo-1a00-stub lzo-0088-stub \
n2b-1a00-stub n2b-0088-stub \
n2d-1a00-stub n2d-0088-stub \
n2e-1a00-stub n2e-0088-stub

all: $(TARGETS)

dist: $(TARGETS)
	ee-strip $(TARGETS)

zlib:
	make -C zlib

lzo:
	make -C lzo

ucl:
	make -C ucl


ZLIB_SIGNATURE = -Wl,--defsym,_signature=0x42494c5a
NULL_SIGNATURE = -Wl,--defsym,_signature=0x4c4c554e
LZO_SIGNATURE = -Wl,--defsym,_signature=0x314f5a4c
N2B_SIGNATURE = -Wl,--defsym,_signature=0x3142324e
N2D_SIGNATURE = -Wl,--defsym,_signature=0x3144324e
N2E_SIGNATURE = -Wl,--defsym,_signature=0x3145324e


zlib-0088-stub: main.o zlib-stub.o zlib
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x0088000 -o zlib-0088-stub main.o zlib-stub.o ./zlib/zlib.a $(EE_LDFLAGS) $(ZLIB_SIGNATURE)

zlib-1a00-stub: main.o zlib-stub.o zlib
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x1a00000 -o zlib-1a00-stub main.o zlib-stub.o ./zlib/zlib.a $(EE_LDFLAGS) $(ZLIB_SIGNATURE)


null-0088-stub: main.o null-stub.o
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x0088000 -o null-0088-stub main.o null-stub.o $(EE_LDFLAGS) $(NULL_SIGNATURE)

null-1a00-stub: main.o null-stub.o
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x1a00000 -o null-1a00-stub main.o null-stub.o $(EE_LDFLAGS) $(NULL_SIGNATURE)


lzo-0088-stub: main.o lzo-stub.o lzo
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x0088000 -o lzo-0088-stub main.o lzo-stub.o ./lzo/lzo.a $(EE_LDFLAGS) $(LZO_SIGNATURE)

lzo-1a00-stub: main.o lzo-stub.o lzo
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x1a00000 -o lzo-1a00-stub main.o lzo-stub.o ./lzo/lzo.a $(EE_LDFLAGS) $(LZO_SIGNATURE)


n2b-0088-stub: main.o n2b-stub.o ucl
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x0088000 -o n2b-0088-stub main.o n2b-stub.o ./ucl/ucl.a $(EE_LDFLAGS) $(N2B_SIGNATURE)

n2b-1a00-stub: main.o n2b-stub.o ucl
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x1a00000 -o n2b-1a00-stub main.o n2b-stub.o ./ucl/ucl.a $(EE_LDFLAGS) $(N2B_SIGNATURE)


n2d-0088-stub: main.o n2d-stub.o ucl
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x0088000 -o n2d-0088-stub main.o n2d-stub.o ./ucl/ucl.a $(EE_LDFLAGS) $(N2D_SIGNATURE)

n2d-1a00-stub: main.o n2d-stub.o ucl
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x1a00000 -o n2d-1a00-stub main.o n2d-stub.o ./ucl/ucl.a $(EE_LDFLAGS) $(N2D_SIGNATURE)


n2e-0088-stub: main.o n2e-stub.o ucl
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x0088000 -o n2e-0088-stub main.o n2e-stub.o ./ucl/ucl.a $(EE_LDFLAGS) $(N2E_SIGNATURE)

n2e-1a00-stub: main.o n2e-stub.o ucl
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x1a00000 -o n2e-1a00-stub main.o n2e-stub.o ./ucl/ucl.a $(EE_LDFLAGS) $(N2E_SIGNATURE)


clean:
	rm -f *.o $(TARGETS)
	make -C zlib clean
	make -C lzo clean
	make -C ucl clean

rebuild: clean all

.PHONY: zlib lzo ucl

%.o : %.c
	$(EE_CC) $(EE_CFLAGS) $(EE_CPPFLAGS) $(EE_INCS) -c $< -o $@

