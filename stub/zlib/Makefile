include $(PS2SDK)/Defs.make

EE_CPPFLAGS = -G0 -I . -D_EE -O3 -Wall -DNO_GZIP -DBUILDFIXED -DNO_vsnprintf

OBJS = adler32.o zutil.o inflate.o inffast.o inftrees.o

all: zlib.a

%.o: %.c
	$(EE_CC) $(EE_CFLAGS) $(EE_CPPFLAGS) $(EE_INCS) -c $< -o $@

zlib.a: $(OBJS)
	$(EE_AR) rc zlib.a $(OBJS)

clean:
	rm -f $(OBJS) zlib.a

rebuild: clean all
