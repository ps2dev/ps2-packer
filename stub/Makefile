include $(PS2SDK)/Defs.make

EE_CPPFLAGS = -G0 -I $(PS2SDK)/ee/include -I $(PS2SDK)/common/include -I . -I zlib -D_EE -O3 -Wall
EE_LDFLAGS = -T ./linkfile -L $(PS2SDK)/ee/lib -lc -lkernel -lc -lsyscall

TARGETS = zlib-1a00-stub zlib-0088-stub

all: $(TARGETS)

dist: $(TARGETS)
	ee-strip $(TARGETS)

zlib:
	make -C zlib

# The linking order is important here.
# "crt0" has to be the first linked object of the final stub.
# Read crt0.s for more explanations.
zlib-0088-stub: main.o zlib
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x0088000 -o zlib-0088-stub main.o ./zlib/zlib.a $(EE_LDFLAGS)

zlib-1a00-stub: main.o zlib
	$(EE_CC) -nostartfiles ./crt0.s -Wl,--defsym,_start_address=0x1a00000 -o zlib-1a00-stub main.o ./zlib/zlib.a $(EE_LDFLAGS)

main.o: main.c
	$(EE_CC) $(EE_CPPFLAGS) -c main.c

clean:
	rm -f main.o $(TARGETS)
	make -C zlib clean

rebuild: clean all

.PHONY: zlib
