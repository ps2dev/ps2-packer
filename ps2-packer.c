/*
 *  PS2-Packer
 *  Copyright (C) 2004-2005 Nicolas "Pixel" Noble
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

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdarg.h>
#ifndef PS2_PACKER_LITE
#include "dlopen.h"
#endif

#ifdef _WIN32
#define SUFFIX ".dll"
#elif defined (__APPLE__)
#define SUFFIX ".dylib"
#else
#define SUFFIX ".so"
#endif

typedef unsigned long int u32;
typedef unsigned short int u16;
typedef unsigned char u8;

#ifdef _WIN32
#define snprintf(buffer, size, args...) sprintf(buffer, args)
#endif

/* These global variables should contain the data about the loaded stub */
u8 * stub_section;
u32 * stub_data;
u32 stub_pc;    /* Starting point */
u32 stub_base;  /* Loading address */
u32 stub_size;  /* Size of the stub section */
u32 stub_zero;  /* Number of bytes to zero-pad at the end of the section */
u32 stub_signature; /* packer signature located in the stub */

u32 reload = 0;

u8 alternative = 0; /* boolean for alternative loading method */
u32 alignment = 0x10;

int sections;

/*
  This is the pointer in our output elf file.
*/
u32 data_pointer;

struct option long_options[] = {
    {"help",    0, NULL, 'h'},
    {"base",    1, NULL, 'b'},
    {"reload",  1, NULL, 'r'},
#ifndef PS2_PACKER_LITE
    {"packer",  1, NULL, 'p'},
    {"stub",    1, NULL, 's'},
#endif
    {"align",   1, NULL, 'a'},
    {"verbose", 1, NULL, 'v'},
    {0,         0, NULL,  0 },
};

void printe(char * fmt, ...) {
    va_list list;
    va_start(list, fmt);
    vfprintf(stderr, fmt, list);
    va_end(list);
    exit(-1);
}

int verbose = 0;

void printv(char * fmt, ...) {
    va_list list;
    if (!verbose)
	return;
    va_start(list, fmt);
    vfprintf(stdout, fmt, list);
    va_end(list);
}

#ifndef PS2_PACKER_LITE
typedef int (*pack_section_t)(const u8 * source, u8 ** dest, u32 source_size);
pack_section_t pack_section;
typedef u32 (*signature_t)();
signature_t signature;
#endif

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
    u32 originalSize;
    u32 zeroByteSize;
    u32 virtualAddr;
    u32 compressedSize;
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
	printv("Error: sizeof(u8) != 1\n");
    }

    if (sizeof(u16) != 2) {
	printv("Error: sizeof(u16) != 2\n");
    }

    if (sizeof(u32) != 4) {
	printv("Error: sizeof(u32) != 4\n");
    }
}

void show_banner() {
    printf(
        "PS2-Packer v" VERSION " (C) 2004-2005 Nicolas \"Pixel\" Noble\n"
        "This is free software with ABSOLUTELY NO WARRANTY.\n"
        "\n"
    );
}

void show_usage() {
    printf(
	"Usage: ps2-packer [-v] [-a X] [-b X] "
#ifndef PS2_PACKER_LITE
	"[-p X] [-s X] "
#endif
	"[-r X] <in_elf> <out_elf>\n"
	"    -v             verbose mode.\n"
	"    -b base        sets the loading base of the compressed data. When activated\n"
	"                     it will activate the alternative packing way.\n"
#ifndef PS2_PACKER_LITE
        "    -p packer      sets a packer name. n2e by default.\n"
	"    -s stub        sets another uncruncher stub. stub/n2e-asm-1d00-stub,\n"
	"                     or stub/n2e-0088-stub when using alternative packing.\n"
#endif
	"    -r reload      sets a reload base of the stub. Beware, that will only works\n"
	"                     with the special asm stubs.\n"
	"    -a align       sets section alignment. 16 by default. Any value accepted.\n"
    );
}

/* The default ELF ident field */
u8 ident[] = {
    0x7F, 0x45, 0x4C, 0x46, 0x01, 0x01, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void remove_section_zeroes(u8 * section, u32 * section_size, u32 * zeroes) {
    u32 removed = 0;
    u32 whole_size;
    u32 realign = 0;
    
    while (!section[*section_size - 1 - removed]) {
	removed++;
    }

#if 0    
    whole_size = *section_size + *zeroes;
    
    if (whole_size & (alignment - 1)) {
	realign = whole_size + alignment;
	realign &= -alignment;
	realign = whole_size - realign;
	removed -= realign;
	if (removed < 0)
	    removed = 0;
    }
#endif
    
    printv("Removing %i zeroes to section...\n", removed);
    
    *section_size -= removed;
    *zeroes += removed;
}

void realign_data_pointer() {
    if (data_pointer & (alignment - 1)) {
	data_pointer += alignment;
	data_pointer &= -alignment;
    }
}

int count_sections(FILE * stub) {
    u8 * loadbuf;
    int size;
    int i;
    elf_header_t *eh = 0;
    elf_pheader_t *eph = 0;
    int r = 0;

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
    
    /* Parsing the interesting program headers */
    eph = (elf_pheader_t *)(loadbuf + eh->phoff);
    for (i = 0; i < eh->phnum; i++) {
	SWAP_ELF_PHEADER(eph[i]);
        if (eph[i].type != PT_LOAD)
            continue;
	
	r++;
    }
    
    free(loadbuf);
    
    return r;
}

#ifdef PS2_PACKER_LITE
#ifdef __MINGW32__
#include "mingw-builtin_stub_one.h"
#include "mingw-builtin_stub.h"
#endif
extern u8 _binary_b_stub_one_start[];
extern u8 _binary_b_stub_start[];
#endif

/* Loads the stub file in memory, filling up the global variables */
void load_stub(
#ifndef PS2_PACKER_LITE
    FILE * stub
#endif
    ) {
    u8 * loadbuf, * pdata;
    int size;
    int i;
    elf_header_t *eh = 0;
    elf_pheader_t *eph = 0;
    int loaded = 0;

#ifndef PS2_PACKER_LITE
    fseek(stub, 0, SEEK_END);
    size = ftell(stub);
    fseek(stub, 0, SEEK_SET);
    
    loadbuf = (u8 *) malloc(size);
    fread(loadbuf, 1, size, stub);
#else
    if (sections == 1) {
	loadbuf = _binary_b_stub_one_start;
    } else {
	loadbuf = _binary_b_stub_start;
    }
#endif

    eh = (elf_header_t *)loadbuf;
    SWAP_ELF_HEADER((*eh));
    if (*(u32 *)&eh->ident != ELF_MAGIC) {
        printe("This ain't no ELF file\n");
    }
    
    stub_pc = eh->entry;
    
    printv("Stub PC = %08X\n", stub_pc);

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
	
	if (reload != 0)
	    stub_base = reload;
	
	
	loaded = 1;
	break;
    }
    
    if (!loaded)
	printe("Unable to load stub file.\n");

    stub_data = (u32 *) (stub_section + stub_pc - stub_base - 8);
    stub_signature = stub_data[0];
    SWAP32(stub_signature);
    
    remove_section_zeroes(stub_section, &stub_size, &stub_zero);
    printv("Loaded stub: %08X bytes (with %08X zeroes) based at %08X\n", stub_size, stub_zero, stub_base);

#ifndef PS2_PACKER_LITE
    free(loadbuf);
#endif
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
    eh.phnum = alternative ? 2 : 1;
    eh.shoff = 0;
    eh.shnum = 0;
    eh.shentsize = 0;
    eh.entry = stub_pc;
    eh.shstrndx = 0;
    
    SWAP_ELF_HEADER(eh);

    if (fwrite(&eh, 1, sizeof(eh), out) != sizeof(eh)) {
	printe("Error writing elf header\n");
    }
    
    if (!alternative) {
        data_pointer = sizeof(eh) + sizeof(eph);
        realign_data_pointer();
	return;
    }
    
    data_pointer = sizeof(eh) + 2 * sizeof(eph);
    realign_data_pointer();

    eph.type = PT_LOAD;
    eph.flags = PF_R | PF_X;
    eph.offset = data_pointer;
    eph.vaddr = stub_base;
    eph.paddr = stub_base;
    eph.filesz = stub_size;
    eph.memsz = stub_size + stub_zero;
    eph.align = alignment;
    
    SWAP_ELF_PHEADER(eph);
    
    if (fwrite(&eph, 1, sizeof(eph), out) != sizeof(eph)) {
	printe("Error writing stub program header\n");
    }
    
    fseek(out, data_pointer, SEEK_SET);
    
    SWAP32(base);
    stub_data[1] = base;
    
    if (fwrite(stub_section, 1, stub_size, out) != stub_size) {
	printe("Error writing stub\n");
    }
    
    data_pointer += stub_size;
    realign_data_pointer();
    
    printv("Actual pointer in file = %08X\n", data_pointer);
}

/* Will produce the second program section of the elf, by packing all the
   program headers of the input file */
void packing(FILE * out, FILE * in, u32 base, int use_asm_n2e) {
    u8 * loadbuf, * pdata, * packed;
    int size, section_size, packed_size;
    int i;
    elf_header_t *eh = 0;
    elf_pheader_t *eph = 0, weph;
    packed_Header ph;
    packed_SectionHeader psh;
    char zero = 0;

    /* preparing the output program header for that section */
    weph.type = PT_LOAD;
    weph.flags = PF_R | PF_W;
    if (!alternative)
	weph.flags |= PF_X;
    weph.offset = data_pointer;
    if (alternative) {
        weph.vaddr = base;
        weph.paddr = base;
    }
    weph.align = alignment;

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
    
    printv("ELF PC = %08X\n", ph.entryAddr);

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
    if (use_asm_n2e == 2) {  // only one section
	if (fwrite(&ph.entryAddr, 1, sizeof(ph.entryAddr), out) != sizeof(ph.entryAddr))
	    printe("Error writing packed header\n");
	weph.filesz += sizeof(ph.entryAddr);
    } else {
	if (fwrite(&ph, 1, sizeof(ph), out) != sizeof(ph))
	    printe("Error writing packed header\n");
	weph.filesz += sizeof(ph);
    }

    /* looping on the program headers to pack them */
    for (i = 0; i < eh->phnum; i++) {
        if (eph[i].type != PT_LOAD)
            continue;

        pdata = (loadbuf + eph[i].offset);
	section_size = eph[i].filesz;
	
	psh.originalSize = section_size;
	psh.virtualAddr = eph[i].vaddr;
	psh.zeroByteSize = eph[i].memsz - eph[i].filesz;
	
	remove_section_zeroes(pdata, &section_size, &psh.zeroByteSize);
	printv("Loaded section: %08X bytes (with %08X zeroes) based at %08X\n", psh.originalSize, psh.zeroByteSize, psh.virtualAddr);
	
	psh.compressedSize = packed_size = pack_section(pdata, &packed, section_size);
	
	printv("Section packed, from %u to %u bytes, ratio = %5.2f%%\n", section_size, packed_size, 100.0 * (section_size - packed_size) / section_size);
	
	SWAP_PACKED_SECTION_HEADER(psh);
	if (use_asm_n2e == 2) {  // we don't need compressed size
	    if (fwrite(&psh, 1, 3 * sizeof(u32), out) != 3 * sizeof(u32))
	        printe("Error writing packed section header.\n");
	    weph.filesz += 3 * sizeof(u32);
	} else {
	    if (fwrite(&psh, 1, sizeof(psh), out) != sizeof(psh))
	        printe("Error writing packed section header.\n");
	    weph.filesz += sizeof(psh);
	}
	if (fwrite(packed, 1, packed_size, out) != packed_size)
	    printe("Error writing packed section.\n");
	weph.filesz += packed_size;
	while (weph.filesz & 3) {
	    weph.filesz++;
	    fwrite(&zero, 1, 1, out);
	}

	free(packed);
    }
    
    if (!alternative) {
	/* Padd data so they are a multiple of alignment. */
	if (weph.filesz & (alignment - 1)) {
    	    weph.filesz += alignment;
	    weph.filesz &= -alignment;
	}
	fseek(out, data_pointer + weph.filesz, SEEK_SET);
	base = stub_base - weph.filesz;
	printv("Final base address: %08X\n", base);
        weph.vaddr = base;
        weph.paddr = base;
	SWAP32(base);
	stub_data[1] = base;
    
	printv("Writing stub.\n");

	if (fwrite(stub_section, 1, stub_size, out) != stub_size) {
	    printe("Error writing stub\n");
	}
	
	weph.filesz += stub_size;	
    }
    data_pointer += weph.filesz;
    
    printv("All data written, writing program header.\n");
    weph.memsz = weph.filesz + stub_zero;
    
    if (alternative)
	fseek(out, sizeof(*eh) + sizeof(*eph), SEEK_SET);
    else
	fseek(out, sizeof(*eh), SEEK_SET);
    SWAP_ELF_PHEADER(weph);
    if (fwrite(&weph, 1, sizeof(weph), out) != sizeof(weph))
	printe("Error writing packed program header.\n");
    
    free(loadbuf);
}

#ifndef BUFSIZ
#define BUFSIZ 1024
#endif

int file_exists(const char * fname) {
    FILE * f;
    
    if (!(f = fopen(fname, "rb")))
	return 0;

    fclose(f);
    return 1;
}

int main(int argc, char ** argv) {
    char c;
    u32 base = 0;
    char buffer[BUFSIZ + 1];
#ifndef PS2_PACKER_LITE
    char * packer_name = 0;
    char * stub_name = 0;
    char * packer_dll = 0;
#endif
    char * in_name;
    char * out_name;
    void * packer_module = 0;
#ifndef PS2_PACKER_LITE
    FILE * stub_file;
#endif
    FILE * in, * out;
    u32 size_in, size_out;
    int use_asm_n2e = 0;
    char * pwd;
    
    sanity_checks();
    
    show_banner();
    
    if ((pwd = strrchr(argv[0], '/'))) {
	*pwd = 0;
    } else if ((pwd = strrchr(argv[0], '\\'))) {
	*pwd = 0;
    }
    
    pwd = argv[0];
    
    while ((c = getopt_long(argc, argv, "b:a:"
#ifndef PS2_PACKER_LITE
	        "p:s:"
#endif
		"hvr:", long_options, NULL)) != EOF) {
	switch (c) {
	case 'b':
	    base = strtol(optarg, NULL, 0);
	    alternative = 1;
	    break;
	case 'a':
	    alignment = strtol(optarg, NULL, 0);
	    break;
#ifndef PS2_PACKER_LITE
	case 'p':
	    packer_name = strdup(optarg);
	    break;
	case 's':
	    stub_name = strdup(optarg);
	    break;
#endif
	case 'r':
	    reload = strtol(optarg, NULL, 0);
	    break;
	case 'v':
	    verbose = 1;
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

    if (alternative)    
        printv("Using alternative packing method.\n");

    if ((argc - optind) != 2) {
	printe("%i files specified, I need exactly 2.\n", argc - optind);
    }
    
    in_name = argv[optind++];
    out_name = argv[optind++];
    
    if (!(in = fopen(in_name, "rb"))) {
	printe("Unable to open input file %s\n", in_name);
    }
    
    if (!(out = fopen(out_name, "wb"))) {
	printe("Unable to open output file %s\n", out_name);
    }
    
    sections = count_sections(in);
    
#ifndef PS2_PACKER_LITE
    if (!packer_name) {
	packer_name = "n2e";
    }
    
    if (!stub_name) {
	if (!base) {
	    if (strcmp(packer_name, "n2e") == 0) {
		if (sections == 1) {
		    printf("Using special ucl-nrv2e asm (one section) stub\n");
	    	    snprintf(buffer, BUFSIZ, "stub/%s-asm-one-1d00-stub", packer_name);
		    use_asm_n2e = 2;
		} else {
		    printf("Using special ucl-nrv2e asm (multiple sections) stub\n");
	    	    snprintf(buffer, BUFSIZ, "stub/%s-asm-1d00-stub", packer_name);
		    use_asm_n2e = 1;
		}
	    } else {
	        snprintf(buffer, BUFSIZ, "stub/%s-1d00-stub", packer_name);
	    }
	} else {
	    snprintf(buffer, BUFSIZ, "stub/%s-0088-stub", packer_name);
	}
	stub_name = strdup(buffer);
    }

    if (!file_exists(stub_name)) {
        snprintf(buffer, BUFSIZ, PREFIX "/share/ps2-packer/%s", stub_name);
	if (!file_exists(buffer)) {
    	    snprintf(buffer, BUFSIZ, "%s/%s", pwd, stub_name);
        }
	free(stub_name);
	stub_name = strdup(buffer);
    }

    if (!(stub_file = fopen(stub_name, "rb"))) {
	printe("Unable to open stub file %s\n", stub_name);
    }
    
    snprintf(buffer, BUFSIZ, PREFIX "/share/ps2-packer/module/%s-packer" SUFFIX, packer_name);
    if (!file_exists(buffer)) {
	snprintf(buffer, BUFSIZ, "%s/%s-packer" SUFFIX, pwd, packer_name);
	if (!file_exists(buffer))
	    snprintf(buffer, BUFSIZ, "./%s-packer" SUFFIX, packer_name);
    }
    
    packer_dll = strdup(buffer);
#else
    use_asm_n2e = ((sections == 1) ? 2 : 1);
#endif
    
    printf("Compressing %s...\n", in_name);
    
    printv("Loading stub file.\n");
#ifndef PS2_PACKER_LITE
    load_stub(stub_file);
    fclose(stub_file);
#else
    load_stub();
#endif

#ifndef PS2_PACKER_LITE
    printv("Opening packer.\n");
    packer_module = open_module(packer_dll);
    pack_section = get_symbol(packer_module, "pack_section");
    signature = get_symbol(packer_module, "signature");
#endif
    if (signature() != stub_signature) {
	printe("Packer's signature and stub's signature are not matching.\n");
    }
    
    printv("Preparing output elf file.\n");
    prepare_out(out, base);
    
    printv("Packing.\n");
    packing(out, in, base, use_asm_n2e);
    
    printv("Done!\n");
    
    fseek(in, 0, SEEK_END);
    size_in = ftell(in);
    size_out = data_pointer;
    
    printf("File compressed, from %i to %i bytes, ratio = %5.2f%%\n", size_in, size_out, 100.0 * (size_in - size_out) / size_in);
    
    fclose(out);
    fclose(in);

    return 0;
}
