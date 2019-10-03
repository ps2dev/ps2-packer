include $(PS2SDK)/Defs.make

EE_CPPFLAGS = -G0 -I . -D_EE -O3 -Wall -DLSB_FIRST -DALIGN_DWORD -DLZ4_FORCE_SW_BITCOUNT

OBJS = lz4.o

all: lz4.a

%.o: %.c
	$(EE_CC) $(EE_CFLAGS) $(EE_CPPFLAGS) $(EE_INCS) -c $< -o $@

lz4.a: $(OBJS)
	$(EE_AR) rc lz4.a $(OBJS)

clean:
	rm -f $(OBJS) lz4.a

rebuild: clean all
