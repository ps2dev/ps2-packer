include $(PS2SDK)/Defs.make

EE_CPPFLAGS = -G0 -I . -D_EE -O3 -Wall

OBJS = n2b_d.o n2d_d.o n2e_d.o

all: ucl.a

%.o: %.c
	$(EE_CC) $(EE_CFLAGS) $(EE_CPPFLAGS) $(EE_INCS) -c $< -o $@

ucl.a: $(OBJS)
	$(EE_AR) rc ucl.a $(OBJS)

clean:
	rm -f $(OBJS) ucl.a

rebuild: clean all
