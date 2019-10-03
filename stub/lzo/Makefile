include $(PS2SDK)/Defs.make

EE_CPPFLAGS = -G0 -I . -D_EE -O3 -Wall -DLSB_FIRST -DALIGN_DWORD

OBJS = minilzo.o

all: lzo.a

%.o: %.c
	$(EE_CC) $(EE_CFLAGS) $(EE_CPPFLAGS) $(EE_INCS) -c $< -o $@

lzo.a: $(OBJS)
	$(EE_AR) rc lzo.a $(OBJS)

clean:
	rm -f $(OBJS) lzo.a

rebuild: clean all
