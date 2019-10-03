OBJS_DIR = obj/
LIB_DIR =
SRC_DIR = ../common/lzma/

CPPFLAGS += -fPIC -I $(SRC_DIR)

OBJS = Alloc.o LzFind.o LzmaDec.o LzmaEnc.o

LZMA_MT ?= 1
ifeq ($(LZMA_MT),1)
	OBJS += LzFindMt.o MtCoder.o MtDec.o Threads.o
else
	CPPFLAGS += -D_7ZIP_ST
endif

OBJS := $(OBJS:%=$(OBJS_DIR)%)

LIB = $(LIB_DIR)lzma.a

all: $(OBJS_DIR) $(LIB_DIR) $(LIB)

$(OBJS_DIR)%.o: $(SRC_DIR)%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCS) -c $< -o $@

$(LIB_DIR):
	mkdir -p $(LIB_DIR)

$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

$(LIB): $(OBJS)
	$(AR) rc $(LIB) $(OBJS)

clean:
	rm -f $(OBJS_DIR)*.o $(LIB_DIR)*.a

rebuild: clean all
