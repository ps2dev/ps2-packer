include $(PS2SDK)/Defs.make

EE_CPPFLAGS = -G0 -I $(PS2SDK)/ee/include -I $(PS2SDK)/common/include -I . -I zlib -D_EE -O3 -Wall
EE_LDFLAGS = -T ./linkfile -L $(PS2SDK)/ee/lib -lc -lkernel -lc -lsyscall

TARGETS = zlib-1a00-stub zlib-0088-stub null-1a00-stub null-0088-stub

all: $(TARGETS)

dist: $(TARGETS)
	ee-strip $(TARGETS)

zlib:
	make -C zlib

ZLIB_SIGNATURE = -Wl,--defsym,_signature=0x42494c5a
NULL_SIGNATURE = -Wl,--defsym,_signature=0x4c4c554e


zlib-0088-stub: main.o zlib-stub.o zlib
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x0088000 -o zlib-0088-stub main.o zlib-stub.o ./zlib/zlib.a $(EE_LDFLAGS) $(ZLIB_SIGNATURE)

zlib-1a00-stub: main.o zlib-stub.o zlib
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x1a00000 -o zlib-1a00-stub main.o zlib-stub.o ./zlib/zlib.a $(EE_LDFLAGS) $(ZLIB_SIGNATURE)


null-0088-stub: main.o null-stub.o
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x0088000 -o null-0088-stub main.o null-stub.o $(EE_LDFLAGS) $(NULL_SIGNATURE)

null-1a00-stub: main.o null-stub.o
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x1a00000 -o null-1a00-stub main.o null-stub.o $(EE_LDFLAGS) $(NULL_SIGNATURE)

clean:
	rm -f *.o $(TARGETS)
	make -C zlib clean

rebuild: clean all

.PHONY: zlib

%.o : %.c
	$(EE_CC) $(EE_CFLAGS) $(EE_CPPFLAGS) $(EE_INCS) -c $< -o $@

