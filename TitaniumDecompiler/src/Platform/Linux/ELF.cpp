#include "tdpch.h"
#include "Platform/Linux/ELF.h"
#include <stdio.h>

namespace TitaniumDecompiler {
// void ELF::ReadElfHeader(const char* elfFile, Elf64Hdr* header64) {
//     FILE* file = fopen(elfFile, "rb");
    
//     if(file == NULL) {
//         perror("[E] Error opening file:");
//     }

//     fseek(file, 0, SEEK_SET);
//     fread(&header64, sizeof(char), sizeof(Elf64Hdr), file);

//     fclose(file);
// }

void ELF::GetElfHeader(Elf64Hdr header64) {
    printf("Storage class\t= ");
    switch(header64.e_ident[EI_CLASS]) {
        case ELFCLASS32:
            printf("32-bit object\n");
            break;
        case ELFCLASS64:
            printf("64-bit object\n");
            break;
        default:
            printf("Invalid Class\n");
            break;
    }

    printf("Data format\t= ");
    switch(header64.e_ident[EI_DATA]) {
        case ELFDATA2LSB:
            printf("Little Endian\n");
            break;
        case ELFDATA2MSB:
            printf("Big Endian\n");
            break;
        default:
            printf("Invalid Format\n");
            break;
    }

    printf("OS ABI\t\t= ");
    switch(header64.e_ident[EI_OSABI]) {
        case ELFOSABI_NONE:
			printf("UNIX System V ABI\n");
			break;
		case ELFOSABI_HPUX:
			printf("HP-UX\n");
			break;
		case ELFOSABI_NETBSD:
			printf("NetBSD\n");
			break;
		case ELFOSABI_LINUX:
			printf("Linux\n");
			break;
		case ELFOSABI_SOLARIS:
			printf("Sun Solaris\n");
			break;
		case ELFOSABI_AIX:
			printf("IBM AIX\n");
			break;
		case ELFOSABI_IRIX:
			printf("SGI Irix\n");
			break;
		case ELFOSABI_FREEBSD:
			printf("FreeBSD\n");
			break;
		case ELFOSABI_TRU64:
			printf("Compaq TRU64 UNIX\n");
			break;
		case ELFOSABI_MODESTO:
			printf("Novell Modesto\n");
			break;
		case ELFOSABI_OPENBSD:
			printf("OpenBSD\n");
			break;
		case ELFOSABI_ARM_AEABI:
			printf("ARM EABI\n");
			break;
		case ELFOSABI_ARM:
			printf("ARM\n");
			break;
		case ELFOSABI_STANDALONE:
			printf("Standalone (embedded) app\n");
			break;
		default:
			printf("Unknown (0x%x)\n", header64.e_ident[EI_OSABI]);
			break;
    }

    printf("File Type \t= ");
    switch(header64.e_type) {
        case ET_NONE:
            printf("N/A (0x0)\n");
			break;
		case ET_REL:
			printf("Relocatable\n");
			break;
		case ET_EXEC:
			printf("Executable\n");
			break;
		case ET_DYN:
			printf("Shared Object\n");
			break;
		default:
            printf("Unknown (0x%x)\n", header64.e_type);
            break;
    }

    printf("Machine\t\t= ");
    switch(header64.e_machine) {
        case EM_NONE:
			printf("None (0x0)\n");
			break;
		case EM_386:
			printf("INTEL x86 (0x%x)\n", EM_386);
			break;
		case EM_X86_64:
			printf("AMD x86_64 (0x%x)\n", EM_X86_64);
			break;
		case EM_AARCH64:
			printf("AARCH64 (0x%x)\n", EM_AARCH64);
			break;
		default:
			printf(" 0x%x\n", header64.e_machine);
			break;
    }

    printf("Entry point\t= 0x%08x\n", header64.e_entry);
    printf("\n");
}

void ELF::GetSectionHeaderTable(FILE* elfFile, Elf64Hdr header64, Elf64SHdr sh_table[]) {
    uint32_t i;
    fseek(elfFile, header64.e_shoff, SEEK_SET);
    for(i = 0; i < header64.e_shnum; i++) {
        fread((void*)&sh_table[i], 1, header64.e_shentsize, elfFile);
    }
}

char* ELF::GetSections(FILE* elfFile, Elf64SHdr sectionHeader64) {
    char* buffer = (char*)malloc(sectionHeader64.sh_size);
    
    if(!buffer) {
        printf("%s: Failed to allocate %ld bytes\n", __func__, sectionHeader64.sh_size);
    }

    fseek(elfFile, sectionHeader64.sh_offset, SEEK_SET);
    fread(buffer, 1, sectionHeader64.sh_size, elfFile);

    return buffer;
}

void ELF::GetSectionHeader(FILE* elfFile, Elf64Hdr header64, Elf64SHdr sh_table[]) {
    uint32_t i;
    char* sectionHeaderString;

    sectionHeaderString = GetSections(elfFile, sh_table[header64.e_shstrndx]);
    
    for(i = 0; i < header64.e_shnum; i++) {
		printf("%s\t", (sectionHeaderString + sh_table[i].sh_name));
		printf("\n");
	}
}

char* ELF::GetSectionHeaderForImGui(FILE* elfFile, Elf64Hdr header64, Elf64SHdr sh_table[]) {
    uint32_t i;
    char* sectionHeaderString;

    sectionHeaderString = GetSections(elfFile, sh_table[header64.e_shstrndx]);
    char* sectionHeaderResult;
    for(i = 0; i < header64.e_shnum; i++) {
		sectionHeaderResult = sectionHeaderString + sh_table[i].sh_name;
	}

    return sectionHeaderResult;
}

void ELF::GetSymbolTable(FILE* elfFile, Elf64Hdr header64, Elf64SHdr sh_table[], uint32_t symbol_table) {
    char* string_table;
    Elf64Sym* sym_tbl;
    uint32_t i, symbol_count;
    
    sym_tbl = (Elf64Sym*)GetSections(elfFile, sh_table[symbol_table]);

    uint32_t string_table_index = sh_table[symbol_table].sh_link;
    printf("string_table_index = 0x%x\n", string_table_index);
    string_table = GetSections(elfFile, sh_table[string_table_index]);

    symbol_count = (sh_table[symbol_table].sh_size/sizeof(Elf64Sym));
    printf("%d symbols\n", symbol_count);

    for(i = 0; i < symbol_count; i++) {
        printf("0x%08lx ", sym_tbl[i].st_value);
        printf("0x%02x ", ELF_ST_BIND(sym_tbl[i].st_info));
		printf("0x%02x ", ELF_ST_TYPE(sym_tbl[i].st_info));
		printf("%s\n", (string_table + sym_tbl[i].st_name));
    }
}

void ELF::GetSymbols(FILE* elfFile, Elf64Hdr header64, Elf64SHdr sh_table[]) {
    uint32_t i;

    for(i = 0; i < header64.e_shnum; i++) {
        if((sh_table[i].sh_type == SHT_SYMTAB) || (sh_table[i].sh_type == SHT_DYNSYM)) {
            printf("\n[Section %03d]", i);
            GetSymbolTable(elfFile, header64, sh_table, i);
        }
    }
}

void ELF::GetElfFile(const std::string& elfFile) {
    Elf64Hdr header64;
    Elf64SHdr* sectionHeaderTable64;

    FILE* file = fopen(elfFile.c_str(), "rb");
    
    if(file == NULL) {
        perror("[E] Error opening file:");
    }
    
    fseek(file, 0, SEEK_SET);
    fread(&header64, 1, sizeof(Elf64Hdr), file);
    GetElfHeader(header64);
    sectionHeaderTable64 = (Elf64SHdr*)malloc(header64.e_shentsize * header64.e_shnum);
    if(!sectionHeaderTable64) {
        printf("Failed to allocate % bytes\n", (header64.e_shentsize * header64.e_shnum));
    }
    GetSectionHeaderTable(file, header64, sectionHeaderTable64);
    GetSectionHeader(file, header64, sectionHeaderTable64);
    // GetSymbols(file, header64, sectionHeaderTable64);

    fclose(file);
}

void ELF::GetElfFileSections(const std::string& elfFile) {
    TitaniumDecompiler::Elf64Hdr header64;
    TitaniumDecompiler::Elf64SHdr* sectionHeaderTable64;

    FILE* file = fopen(elfFile.c_str(), "rb");
    
    char* sections;

    if(file == NULL) {
        perror("[E] Error opening file:");
    }
    
    fseek(file, 0, SEEK_SET);
    fread(&header64, 1, sizeof(TitaniumDecompiler::Elf64Hdr), file);
    sectionHeaderTable64 = (TitaniumDecompiler::Elf64SHdr*)malloc(header64.e_shentsize * header64.e_shnum);
    if(!sectionHeaderTable64) {
        printf("Failed to allocate % bytes\n", (header64.e_shentsize * header64.e_shnum));
    }

    GetSectionHeaderTable(file, header64, sectionHeaderTable64);
    sections = GetSectionHeaderForImGui(file, header64, sectionHeaderTable64);
    // GetSymbols(file, header64, sectionHeaderTable64);
    printf("%s", sections);
}

}