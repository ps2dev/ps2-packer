include $(PS2SDK)/Defs.make

EE_CPPFLAGS = -G0 -I . -D_EE -O3 -Wall -DLSB_FIRST -DALIGN_DWORD

CC = $(EE_CC)
CPPFLAGS = $(EE_CPPFLAGS)

OBJS = minilzo.o

all: lzo.a

lzo.a: $(OBJS)
	$(EE_AR) rc lzo.a $(OBJS)

clean:
	rm -f $(OBJS) lzo.a

rebuild: clean all
