include $(PS2SDK)/Defs.make

EE_CPPFLAGS = -G0 -I . -D_EE -O3 -Wall -DNO_GZIP -DBUILDFIXED -DNO_vsnprintf

CC = $(EE_CC)
CPPFLAGS = $(EE_CPPFLAGS)

OBJS = adler32.o zutil.o inflate.o inffast.o inftrees.o

all: zlib.a

zlib.a: $(OBJS)
	$(EE_AR) rc zlib.a $(OBJS)

clean:
	rm -f $(OBJS) zlib.a

rebuild: clean all
