#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <string_view>

#include "Platform/Linux/FileFormats/ELFFile/ELFConstants.h"


namespace TitaniumDecompiler {
class ELF {
public:
public:
    void ReadElfHeader(int fileDescriptor, Elf64Hdr* header64);
    void GetElfHeader(Elf64Hdr header64);
    void GetSectionHeaderTable(int fileDescriptor, Elf64Hdr header64, Elf64SHdr sh_table[]);
    char* GetSections(int fileDescriptor, Elf64SHdr sectionHeader64);
    void GetSectionHeader(int fileDescriptor, Elf64Hdr header64, Elf64SHdr sh_table[]);
    char* GetSectionHeaderForImGui(int fileDescriptor, Elf64Hdr header64, Elf64SHdr sh_table[]);
    void GetSymbolTable(int fileDescriptor, Elf64Hdr header64, Elf64SHdr sh_table[], uint32_t symbol_table);
    void GetSymbols(int fileDescriptor, Elf64Hdr header64, Elf64SHdr sh_table[]);
    void GetElfFileSections(const std::string& elfFile);
    void GetElfFile(const std::string& elfFile);
    bool IsElfFile(Elf64Hdr header64);
    // bool IsElfFile(const std::string& elfFile);
};

}