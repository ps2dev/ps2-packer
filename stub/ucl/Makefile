include $(PS2SDK)/Defs.make

EE_CPPFLAGS = -G0 -I . -D_EE -O3 -Wall

CC = $(EE_CC)
CPPFLAGS = $(EE_CPPFLAGS)

OBJS = n2b_d.o n2d_d.o n2e_d.o

all: ucl.a

ucl.a: $(OBJS)
	$(EE_AR) rc ucl.a $(OBJS)

clean:
	rm -f $(OBJS) ucl.a

rebuild: clean all
