/*
 *  PS2-Packer
 *  Copyright (C) 2004 Nicolas "Pixel" Noble
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#define VERSION "0.2.1"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdarg.h>
#include <zlib.h>

typedef unsigned long int u32;
typedef unsigned short int u16;
typedef unsigned char u8;

/* These global variables should contain the data about the loaded stub */
u8 * stub_section;
u32 stub_pc;    /* Starting point */
u32 stub_base;  /* Loading address */
u32 stub_size;  /* Size of the stub section */
u32 stub_zero;  /* Number of bytes to zero-pad at the end of the section */

/*
  This is the pointer in our output elf file.
  The program sections in a PS2 elf seems to be 4096-bytes aligned, so, we
  start at 4096 ( = 0x1000 )
*/
u32 data_pointer = 0x1000;

struct option long_options[] = {
    {"help",    0, NULL, 'h'},
    {"level",   1, NULL, 'l'},
    {"base",    1, NULL, 'b'},
    {"stub",    1, NULL, 's'},
    {0,         0, NULL,  0 },
};

void printe(char * fmt, ...) {
    va_list list;
    va_start(list, fmt);
    vfprintf(stderr, fmt, list);
    va_end(list);
    exit(-1);
}

u8 bigendian = 0;
u32 ELF_MAGIC = 0x464c457f;

#define PT_LOAD     1
#define PF_X        1
#define PF_W   	    2
#define PF_R        4

#define SWAP16(x) { if (bigendian) x = (x >> 8) | (x << 8); }
#define SWAP32(x) { if (bigendian) x = (x >> 24) | ((x >> 8) & 0x0000ff00) | ((x << 8) & 0x00ff0000) | (x << 24); }

/* The primary ELF header. */
typedef struct {
    u8  ident[16];    /* The first 4 bytes are the ELF magic */
    u16 type;         /* == 2, EXEC (executable file) */
    u16 machine;      /* == 8, MIPS r3000 */
    u32 version;      /* == 1, default ELF value */
    u32 entry;        /* program starting point */
    u32 phoff;        /* program header offset in the file */
    u32 shoff;        /* section header offset in the file, unused for us, so == 0 */
    u32 flags;        /* flags, unused for us. */
    u16 ehsize;       /* this header size ( == 52 ) */
    u16 phentsize;    /* size of a program header ( == 32 ) */
    u16 phnum;        /* number of program headers */
    u16 shentsize;    /* size of a section header, unused here */
    u16 shnum;        /* number of section headers, unused here */
    u16 shstrndx;     /* section index of the string table */
} elf_header_t;

#define SWAP_ELF_HEADER(x) { \
    SWAP16(x.type); \
    SWAP16(x.machine); \
    SWAP32(x.version); \
    SWAP32(x.entry); \
    SWAP32(x.phoff); \
    SWAP32(x.shoff); \
    SWAP32(x.flags); \
    SWAP16(x.ehsize); \
    SWAP16(x.phentsize); \
    SWAP16(x.phnum); \
    SWAP16(x.shentsize); \
    SWAP16(x.shnum); \
    SWAP16(x.shstrndx); \
}

typedef struct {
    u32 type;         /* == 1, PT_LOAD (that is, this section will get loaded */
    u32 offset;       /* offset in file, on a 4096 bytes boundary */
    u32 vaddr;        /* virtual address where this section is loaded */
    u32 paddr;        /* physical address where this section is loaded */
    u32 filesz;       /* size of that section in the file */
    u32 memsz;        /* size of that section in memory (rest is zero filled) */
    u32 flags;        /* PF_X | PF_W | PF_R, that is executable, writable, readable */
    u32 align;        /* == 0x1000 that is 4096 bytes */
} elf_pheader_t;

#define SWAP_ELF_PHEADER(x) { \
    SWAP32(x.type); \
    SWAP32(x.offset); \
    SWAP32(x.vaddr); \
    SWAP32(x.paddr); \
    SWAP32(x.filesz); \
    SWAP32(x.memsz); \
    SWAP32(x.flags); \
    SWAP32(x.align); \
}

typedef struct {
    u32 entryAddr;
    u32 numSections;
} packed_Header;

#define SWAP_PACKED_HEADER(x) { \
    SWAP32(x.entryAddr); \
    SWAP32(x.numSections); \
}

typedef struct {
    u32 compressedSize;
    u32 originalSize;
    u32 zeroByteSize;
    u32 virtualAddr;
} packed_SectionHeader;

#define SWAP_PACKED_SECTION_HEADER(x) { \
    SWAP32(x.compressedSize); \
    SWAP32(x.originalSize); \
    SWAP32(x.zeroByteSize); \
    SWAP32(x.virtualAddr); \
}

void sanity_checks() {
    u32 t1 = 0x12345678;
    u8 * t2 = (u8 *) &t1;
    
    if (t2[0] == 0x12) {
	bigendian = 1;
	SWAP32(ELF_MAGIC);
    } else if (t2[0] != 0x78) {
	printe("Error computing machine's endianess!\n");
    }
    
    if (sizeof(u8) != 1) {
	printf("Error: sizeof(u8) != 1\n");
    }

    if (sizeof(u16) != 2) {
	printf("Error: sizeof(u16) != 2\n");
    }

    if (sizeof(u32) != 4) {
	printf("Error: sizeof(u32) != 4\n");
    }
}

void show_banner() {
    printf(
        "PS2-Packer v"VERSION" (C) 2004 Nicolas \"Pixel\" Noble\n"
        "This is free software with ABSOLUTELY NO WARRANTY.\n"
        "\n"
    );
}

void show_usage() {
    printf(
	"Usage: ps2-packer [-l level] [-b base] [-s stub] <in_elf> <out_elf>\n"
    );
}

/* The default ELF ident field */
u8 ident[] = {
    0x7F, 0x45, 0x4C, 0x46, 0x01, 0x01, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* Loads the stub file in memory, filling up the global variables */
void load_stub(FILE * stub) {
    u8 * loadbuf, * pdata;
    int size;
    int i;
    elf_header_t *eh = 0;
    elf_pheader_t *eph = 0;
    int loaded = 0;

    fseek(stub, 0, SEEK_END);
    size = ftell(stub);
    fseek(stub, 0, SEEK_SET);
    
    loadbuf = (u8 *) malloc(size);
    fread(loadbuf, 1, size, stub);

    eh = (elf_header_t *)loadbuf;
    SWAP_ELF_HEADER((*eh));
    if (*(u32 *)&eh->ident != ELF_MAGIC) {
        printe("This ain't no ELF file\n");
    }
    
    stub_pc = eh->entry;
    
    printf("Stub PC = %08X\n", stub_pc);

    /* Parsing the interesting program headers */
    eph = (elf_pheader_t *)(loadbuf + eh->phoff);
    for (i = 0; i < eh->phnum; i++) {
	SWAP_ELF_PHEADER(eph[i]);
        if (eph[i].type != PT_LOAD)
            continue;

        pdata = (loadbuf + eph[i].offset);
	stub_size = eph[i].filesz;
	stub_section = (u8 *) malloc(eph[i].filesz);
        memcpy(stub_section, pdata, eph[i].filesz);
	
	stub_base = eph[i].vaddr;
	stub_zero = eph[i].memsz - eph[i].filesz;
	
	printf("Loaded stub: %08X bytes (with %08X zeroes) based at %08X\n", stub_size, stub_zero, stub_base);
	
	loaded = 1;
	break;
    }
    
    if (!loaded)
	printe("Unable to load stub file.\n");
	
    free(loadbuf);
}

/* Write out the basic elf structures, that is, the ELF header,
   the first program section, and the stub. Updates the global pointer
   data_pointer in order to let the packing routine to know where to put
   its compressed data */
void prepare_out(FILE * out, u32 base) {
    elf_header_t eh;
    elf_pheader_t eph;
    int i;
    
    for (i = 0; i < 16; i++) {
	eh.ident[i] = ident[i];
    }
    eh.type = 2;
    eh.machine = 8;
    eh.version = 1;
    eh.phoff = sizeof(eh);
    eh.shoff = 0;
    eh.flags = 0;
    eh.ehsize = sizeof(eh);
    eh.phentsize = sizeof(elf_pheader_t);
    eh.phnum = 2;
    eh.shoff = 0;
    eh.shnum = 0;
    eh.shentsize = 0;
    eh.entry = stub_pc;
    eh.shstrndx = 0;
    
    SWAP_ELF_HEADER(eh);

    if (fwrite(&eh, 1, sizeof(eh), out) != sizeof(eh)) {
	printe("Error writing elf header\n");
    }
    
    eph.type = PT_LOAD;
    eph.flags = PF_R | PF_X;
    eph.offset = data_pointer;
    eph.vaddr = stub_base;
    eph.paddr = stub_base;
    eph.filesz = stub_size;
    eph.memsz = stub_size + stub_zero;
    eph.align = 0x1000;
    
    SWAP_ELF_PHEADER(eph);
    
    if (fwrite(&eph, 1, sizeof(eph), out) != sizeof(eph)) {
	printe("Error writing stub program header\n");
    }
    
    fseek(out, data_pointer, SEEK_SET);
    
    SWAP32(base);
    ((u32 *) stub_section)[2] = base;
    
    if (fwrite(stub_section, 1, stub_size, out) != stub_size) {
	printe("Error writing stub\n");
    }
    
    data_pointer += stub_size;
    /* Align the pointer to a 4096 bytes boundary if necessary */
    if (data_pointer & 0x0FFF) {
	data_pointer += 0x1000;
	data_pointer &= 0xFFFFF000;
    }
}

/* Will produce the second program section of the elf, by packing all the
   program headers of the input file */
void packing(FILE * out, FILE * in, u32 base, int level) {
    u8 * loadbuf, * pdata, * packed;
    int size, section_size, packed_size;
    int i;
    elf_header_t *eh = 0;
    elf_pheader_t *eph = 0, weph;
    packed_Header ph;
    packed_SectionHeader psh;
    z_stream c_stream;

    /* preparing the output program header for that section */
    weph.type = PT_LOAD;
    weph.flags = PF_R | PF_W;
    weph.offset = data_pointer;
    weph.vaddr = base;
    weph.paddr = base;
    weph.align = 0x1000;

    fseek(in, 0, SEEK_END);
    size = ftell(in);
    fseek(in, 0, SEEK_SET);
    
    loadbuf = (u8 *) malloc(size);
    fread(loadbuf, 1, size, in);

    eh = (elf_header_t *)loadbuf;
    SWAP_ELF_HEADER((*eh));
    if (*(u32 *)&eh->ident != ELF_MAGIC) {
        printe("This ain't no ELF file\n");
    }
    
    ph.entryAddr = eh->entry;
    ph.numSections = 0;
    
    printf("ELF PC = %08X\n", ph.entryAddr);

    eph = (elf_pheader_t *)(loadbuf + eh->phoff);
    
    /* counting and swapping the program headers of the input file */
    for (i = 0; i < eh->phnum; i++) {
	SWAP_ELF_PHEADER(eph[i]);
        if (eph[i].type != PT_LOAD)
            continue;
	ph.numSections++;
    }
    
    weph.filesz = 0;
    fseek(out, data_pointer, SEEK_SET);
    SWAP_PACKED_HEADER(ph);
    if (fwrite(&ph, 1, sizeof(ph), out) != sizeof(ph))
	printe("Error writing packed header\n");
    weph.filesz += sizeof(ph);

    /* looping on the program headers to pack them */
    for (i = 0; i < eh->phnum; i++) {
        if (eph[i].type != PT_LOAD)
            continue;

        pdata = (loadbuf + eph[i].offset);
	section_size = eph[i].filesz;
	
	psh.originalSize = section_size;
	psh.virtualAddr = eph[i].vaddr;
	psh.zeroByteSize = eph[i].memsz - eph[i].filesz;
	
	printf("Loaded section: %08X bytes (with %08X zeroes) based at %08X\n", psh.originalSize, psh.zeroByteSize, psh.virtualAddr);
	
	packed_size = section_size * 1.2 + 2048;
	packed = (u8 *) malloc(packed_size);
	
	c_stream.zalloc = (alloc_func)0;
	c_stream.zfree = (free_func)0;
	c_stream.opaque = (voidpf)0;
	
	if (deflateInit(&c_stream, level) != Z_OK)
	    printe("Error during deflateInit.\n");
	
	c_stream.next_in = pdata;
	c_stream.avail_in = section_size;
	c_stream.next_out = packed;
	c_stream.avail_out = packed_size;
	
	if (deflate(&c_stream, Z_FINISH) != Z_STREAM_END)
	    printe("Error during deflate.\n");
	
	if (deflateEnd(&c_stream) != Z_OK)
	    printe("Error during deflateEnd.\n");
	
	psh.compressedSize = packed_size = c_stream.total_out;
	
	printf("Section deflated, from %u to %u bytes, ratio = %5.2f%%\n", section_size, packed_size, 100.0 * (section_size - packed_size) / section_size);
	
	SWAP_PACKED_SECTION_HEADER(psh);
	if (fwrite(&psh, 1, sizeof(psh), out) != sizeof(psh))
	    printe("Error writing packed section header.\n");
	weph.filesz += sizeof(psh);
	if (fwrite(packed, 1, packed_size, out) != packed_size)
	    printe("Error writing packed section.\n");
	weph.filesz += packed_size;
	
	free(packed);
    }
    
    printf("All section deflated, writing program header.\n");
    weph.memsz = weph.filesz;
    
    fseek(out, sizeof(*eh) + sizeof(*eph), SEEK_SET);
    SWAP_ELF_PHEADER(weph);
    if (fwrite(&weph, 1, sizeof(weph), out) != sizeof(weph))
	printe("Error writing packed program header.\n");
    
    free(loadbuf);
}

int main(int argc, char ** argv) {
    char c;
    u32 base = 0x1b00000;
    char * stub_name = 0;
    FILE * stub_file, * in, * out;
    int level = 9;
    
    sanity_checks();
    
    show_banner();
    
    while ((c = getopt_long(argc, argv, "l:b:s:h", long_options, NULL)) != EOF) {
	switch (c) {
	case 'l':
	    level = strtol(optarg, NULL, 0);
	    break;
	case 'b':
	    base = strtol(optarg, NULL, 0);
	    break;
	case 's':
	    stub_name = strdup(optarg);
	    break;
	case 'h':
	    show_usage();
	    exit(0);
	default:
	    printe("Unknown option %c\n", c);
	    show_usage();
	    exit(-1);
	}
    }
    
    if (!stub_name)
	stub_name = "stub/zlib-0088-stub";
    
    if ((argc - optind) != 2) {
	printe("%i files specified, I need exactly 2.\n", argc - optind);
    }
    
    if (!(stub_file = fopen(stub_name, "rb"))) {
	printe("Unable to open stub file %s\n", stub_name);
    }
    
    if (!(in = fopen(argv[optind++], "rb"))) {
	printe("Unable to open input file %s\n", argv[optind - 1]);
    }
    
    if (!(out = fopen(argv[optind++], "wb"))) {
	printe("Unable to open output file %s\n", argv[optind - 1]);
    }

    printf("Loading stub file.\n");
    
    load_stub(stub_file);
    fclose(stub_file);
    
    printf("Preparing output elf file.\n");
    
    prepare_out(out, base);
    
    printf("Packing.\n");
    
    packing(out, in, base, level);
    
    printf("Done!\n");
    
    fclose(out);
    fclose(in);

    return 0;
}
