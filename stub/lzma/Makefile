include $(PS2SDK)/Defs.make

EE_OBJS_DIR = obj/
EE_LIB_DIR =
SRC_DIR = ../../common/lzma/

EE_CPPFLAGS = -G0 -I $(SRC_DIR) -D_EE -Os -Wall -D_LZMA_SIZE_OPT

EE_OBJS = Alloc.o LzFind.o LzmaDec.o LzmaEnc.o

EE_LZMA_MT ?= 0
ifeq ($(EE_LZMA_MT),1)
	EE_OBJS += LzFindMt.o MtCoder.o MtDec.o Threads.o
else
	EE_CPPFLAGS += -D_7ZIP_ST
endif

EE_OBJS := $(EE_OBJS:%=$(EE_OBJS_DIR)%)

EE_LIB = $(EE_LIB_DIR)lzma.a

all: $(EE_OBJS_DIR) $(EE_LIB_DIR) $(EE_LIB)

$(EE_OBJS_DIR)%.o: $(SRC_DIR)%.c
	$(EE_CC) $(EE_CFLAGS) $(EE_CPPFLAGS) $(EE_INCS) -c $< -o $@

$(EE_LIB_DIR):
	mkdir -p $(EE_LIB_DIR)

$(EE_OBJS_DIR):
	mkdir -p $(EE_OBJS_DIR)

$(EE_LIB): $(EE_OBJS)
	$(EE_AR) rc $(EE_LIB) $(EE_OBJS)

clean:
	rm -f $(EE_OBJS_DIR)*.o $(EE_LIB_DIR)*.a

rebuild: clean all
