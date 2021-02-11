#include "tdpch.h"
#include "Platform/Linux/FileFormats/PE.h"
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

namespace TitaniumDecompiler {
void PE::ReadDosHeader(int fileDescriptor, IMAGE_DOS_HEADER* dosHeader) {
    assert(dosHeader != NULL);
    assert(lseek(fileDescriptor, (off_t)0, SEEK_SET) == (off_t)0);
    assert(read(fileDescriptor, (void*)dosHeader, sizeof(dosHeader)) == sizeof(dosHeader));
}

void PE::GetDosHeader(IMAGE_DOS_HEADER* dosHeader) {
    printf("\t0x%x\t\tMagic Number\n", dosHeader->e_magic);
	printf("\t0x%x\t\tUsed bytes in the last page\n", dosHeader->e_cblp);
	printf("\t0x%x\t\tFile size in pages\n", dosHeader->e_cp);
	printf("\t0x%x\t\tNumber of relocations\n", dosHeader->e_crlc);
    printf("\t0x%x\t\tSize of header in paragraphs\n", dosHeader->e_cparhdr);
	printf("\t0x%x\t\tMinimum extra paragraphs needed\n", dosHeader->e_minalloc);
	printf("\t0x%x\t\tMaximum extra paragraphs needed\n", dosHeader->e_maxalloc);
	// printf("\t0x%x\t\tInitial (relative) SS value\n", dosHeader->e_ss);
	// printf("\t0x%x\t\tInitial SP value\n", dosHeader->e_sp);
	// printf("\t0x%x\t\tChecksum\n", dosHeader->e_csum);
	// printf("\t0x%x\t\tInitial IP value\n", dosHeader->e_ip);
	// printf("\t0x%x\t\tInitial (relative) CS value\n", dosHeader->e_cs);
	// printf("\t0x%x\t\tFile address of relocation table\n", dosHeader->e_lfarlc);
	// printf("\t0x%x\t\tOverlay number\n", dosHeader->e_ovno);
	// printf("\t0x%x\t\tOEM identifier (for e_oeminfo)\n", dosHeader->e_oemid);
	// printf("\t0x%x\t\tOEM information; e_oemid specific\n", dosHeader->e_oeminfo);
	// printf("\t0x%x\t\tFile address of new exe header\n", dosHeader->e_lfanew);
}

void PE::GetPeFile(const std::string& peFile) {
    IMAGE_DOS_HEADER* dosHeader;
    int fileDescriptor;
 
    fileDescriptor = open(peFile.c_str(), O_RDONLY | O_SYNC);
	if(fileDescriptor == NULL) { 
		return;
	}
    ReadDosHeader(fileDescriptor, dosHeader);
    GetDosHeader(dosHeader);
    close(fileDescriptor);
}
}