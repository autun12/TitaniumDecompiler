#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

namespace TitaniumDecompiler {

// PE Magic Headers
#define IMAGE_DOS_SIGNATURE 0x5A4D // MZ
#define IMAGE_OS2_SIGNATURE 0x454E      // NE
#define IMAGE_OS2_SIGNATURE_LE 0x454C      // LE
#define IMAGE_NT_SIGNATURE 0x00004550  // PE00
#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16
#define IMAGE_SIZEOF_SHORT_NAME 8

// Image File Definitions
// These are the settings of the Machine field.
#define IMAGE_FILE_MACHINE_UNKNOWN      0
#define IMAGE_FILE_MACHINE_I860         0x014d
#define IMAGE_FILE_MACHINE_I386         0x014c
#define IMAGE_FILE_MACHINE_R3000        0x0162
#define IMAGE_FILE_MACHINE_R4000        0x0166
#define IMAGE_FILE_MACHINE_R10000       0x0168
#define IMAGE_FILE_MACHINE_WCEMIPSV2    0x0169
#define IMAGE_FILE_MACHINE_ALPHA        0x0184
#define IMAGE_FILE_MACHINE_SH3          0x01a2
#define IMAGE_FILE_MACHINE_SH3DSP       0x01a3
#define IMAGE_FILE_MACHINE_SH3E         0x01a4
#define IMAGE_FILE_MACHINE_SH4          0x01a6
#define IMAGE_FILE_MACHINE_SH5          0x01a8
#define IMAGE_FILE_MACHINE_ARM          0x01c0
#define IMAGE_FILE_MACHINE_THUMB        0x01c2
#define IMAGE_FILE_MACHINE_ARMNT        0x01c4
#define IMAGE_FILE_MACHINE_ARM64        0xaa64
#define IMAGE_FILE_MACHINE_AM33         0x01d3
#define IMAGE_FILE_MACHINE_POWERPC      0x01f0
#define IMAGE_FILE_MACHINE_POWERPCFP    0x01f1
#define IMAGE_FILE_MACHINE_IA64         0x0200
#define IMAGE_FILE_MACHINE_MIPS16       0x0266
#define IMAGE_FILE_MACHINE_ALPHA64      0x0284
#define IMAGE_FILE_MACHINE_MIPSFPU      0x0366
#define IMAGE_FILE_MACHINE_MIPSFPU16    0x0466
#define IMAGE_FILE_MACHINE_AXP64        IMAGE_FILE_MACHINE_ALPHA64
#define IMAGE_FILE_MACHINE_TRICORE      0x0520
#define IMAGE_FILE_MACHINE_CEF          0x0cef
#define IMAGE_FILE_MACHINE_EBC          0x0ebc
#define IMAGE_FILE_MACHINE_AMD64        0x8664
#define IMAGE_FILE_MACHINE_M32R         0x9041
#define IMAGE_FILE_MACHINE_CEE          0xc0ee

// These defines the meanings of the bits in the Characteristics field
#define IMAGE_FILE_RELOCS_STRIPPED      0x0001 // No relocation info
#define IMAGE_FILE_EXECUTABLE_IMAGE     0x0002
#define IMAGE_FILE_LINE_NUMS_STRIPPED   0x0004
#define IMAGE_FILE_LOCAL_SYMS_STRIPPED  0x0008
#define IMAGE_FILE_AGGRESIVE_WS_TRIM    0x0010
#define IMAGE_FILE_LARGE_ADDRESS_AWARE  0x0020
#define IMAGE_FILE_16BIT_MACHINE        0x0040
#define IMAGE_FILE_BYTES_REVERSED_LO    0x0080
#define IMAGE_FILE_32BIT_MACHINE        0x0100
#define IMAGE_FILE_DEBUG_STRIPPED       0x0200
#define IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP      0x0400
#define IMAGE_FILE_NET_RUN_FROM_SWAP    0x0800
#define IMAGE_FILE_SYSTEM               0x1000
#define IMAGE_FILE_DLL                  0x2000
#define IMAGE_FILE_UP_SYSTEM_ONLY       0x4000
#define IMAGE_FILE_BYTES_REVERSED_HI    0x8000

// Directory Entries (16 entries are pre-defined)
#define IMAGE_DIRECTORY_ENTRY_EXPORT         0     // Export Directory 
#define IMAGE_DIRECTORY_ENTRY_IMPORT         1     // Import Directory 
#define IMAGE_DIRECTORY_ENTRY_RESOURCE       2     // Resource Directory 
#define IMAGE_DIRECTORY_ENTRY_EXCEPTION      3     // Exception Directory 
#define IMAGE_DIRECTORY_ENTRY_SECURITY       4     // Security Directory 
#define IMAGE_DIRECTORY_ENTRY_BASERELOC      5     // Base Relocation Table 
#define IMAGE_DIRECTORY_ENTRY_DEBUG          6     // Debug Directory 
#define IMAGE_DIRECTORY_ENTRY_COPYRIGHT      7     // (x86 usage) 
#define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE   7     // Architecture Specific Data 
#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR 	 8     // RVA of GP 
#define IMAGE_DIRECTORY_ENTRY_TLS            9	   // TLS Directory 
#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG    10    // Load Configuration Directory 
#define IMAGE_DIRECTORY_ENTRY_LOAD_BOUND_IMPORT 	11     // Bound Import Directory in headers 
#define IMAGE_DIRECTORY_ENTRY_LOAD_IAT 				12     // Import Address Table 
#define IMAGE_DIRECTORY_ENTRY_LOAD_DELAY_IMPORT 	13     // Delay Load Import Descriptors 
#define IMAGE_DIRECTORY_ENTRY_LOAD_COM_DESCRIPTOR 	14     // COM Runtime descriptor 

// These defines are for the Characteristics bitfield. 
// #define IMAGE_SCN_TYPE_REG                   0x00000000 - Reserved 
// #define IMAGE_SCN_TYPE_DSECT                 0x00000001 - Reserved 
// #define IMAGE_SCN_TYPE_NOLOAD                0x00000002 - Reserved 
// #define IMAGE_SCN_TYPE_GROUP                 0x00000004 - Reserved 
#define IMAGE_SCN_TYPE_NO_PAD                0x00000008 // Reserved 
// #define IMAGE_SCN_TYPE_COPY                  0x00000010 - Reserved 
 
#define IMAGE_SCN_CNT_CODE                      0x00000020
#define IMAGE_SCN_CNT_INITIALIZED_DATA          0x00000040
#define IMAGE_SCN_CNT_UNINITIALIZED_DATA        0x00000080
 
#define IMAGE_SCN_LNK_OTHER                     0x00000100
#define IMAGE_SCN_LNK_INFO                      0x00000200
// #define      IMAGE_SCN_TYPE_OVER             0x00000400 - Reserved 
#define IMAGE_SCN_LNK_REMOVE                    0x00000800
#define IMAGE_SCN_LNK_COMDAT                    0x00001000
 
//                                              0x00002000 - Reserved 
// #define IMAGE_SCN_MEM_PROTECTED              0x00004000 - Obsolete 
#define IMAGE_SCN_MEM_FARDATA                   0x00008000
 
// #define IMAGE_SCN_MEM_SYSHEAP                0x00010000 - Obsolete 
#define IMAGE_SCN_MEM_PURGEABLE                 0x00020000
#define IMAGE_SCN_MEM_16BIT                     0x00020000
#define IMAGE_SCN_MEM_LOCKED                    0x00040000
#define IMAGE_SCN_MEM_PRELOAD                   0x00080000
 
#define IMAGE_SCN_ALIGN_1BYTES                  0x00100000
#define IMAGE_SCN_ALIGN_2BYTES                  0x00200000
#define IMAGE_SCN_ALIGN_4BYTES                  0x00300000
#define IMAGE_SCN_ALIGN_8BYTES                  0x00400000
#define IMAGE_SCN_ALIGN_16BYTES                 0x00500000  // Default 
#define IMAGE_SCN_ALIGN_32BYTES                 0x00600000
#define IMAGE_SCN_ALIGN_64BYTES                 0x00700000
#define IMAGE_SCN_ALIGN_128BYTES                0x00800000
#define IMAGE_SCN_ALIGN_256BYTES                0x00900000
#define IMAGE_SCN_ALIGN_512BYTES                0x00A00000
#define IMAGE_SCN_ALIGN_1024BYTES               0x00B00000
#define IMAGE_SCN_ALIGN_2048BYTES               0x00C00000
#define IMAGE_SCN_ALIGN_4096BYTES               0x00D00000
#define IMAGE_SCN_ALIGN_8192BYTES               0x00E00000
//                                              0x00F00000 - Unused 
#define IMAGE_SCN_ALIGN_MASK                    0x00F00000
#define IMAGE_SCN_LNK_NRELOC_OVFL               0x01000000
#define IMAGE_SCN_MEM_DISCARDABLE               0x02000000
#define IMAGE_SCN_MEM_NOT_CACHED                0x04000000
#define IMAGE_SCN_MEM_NOT_PAGED                 0x08000000
#define IMAGE_SCN_MEM_SHARED                    0x10000000
#define IMAGE_SCN_MEM_EXECUTE                   0x20000000
#define IMAGE_SCN_MEM_READ                      0x40000000
#define IMAGE_SCN_MEM_WRITE                     0x80000000

// PE File Format Structures
typedef struct _IMAGE_DOS_HEADER {
	uint16_t e_magic;    // MZ Header signature                                                                // MANDATORY
	uint16_t e_cblp;     // Bytes on last page of file
	uint16_t e_cp;       // Pages in file
	uint16_t e_crlc;     // Relocations
	uint16_t e_cparhdr;  // Size of header in paragraphs
	uint16_t e_minalloc; // Minimum extra paragraphs needed
	uint16_t e_maxalloc; // Maximum extra paragraphs needed
	uint16_t e_ss;       // Initial (relative) SS value
	uint16_t e_sp;       // Initial SP value
	uint16_t e_csum;     // Checksum
	uint16_t e_ip;       // Initial IP value
	uint16_t e_cs;       // Initial (relative) CS value
	uint16_t e_lfarlc;   // File address of relocation table
	uint16_t e_ovno;     // Overlay number
	uint16_t e_res[4];   // Reserved words
	uint16_t e_oemid;    // OEM identifier (for e_oeminfo)
	uint16_t e_oeminfo;  // OEM information; e_oemid specific
	uint16_t e_res2[10]; // Reserved words // MANDATORY
	uint32_t e_lfanew;   // File address of new exe header
} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

typedef struct _IMAGE_FILE_HEADER {
	uint16_t Machine; // Architecture type of the computer
	uint16_t NumberOfSections; // Number of sections
	uint32_t TimeDateStamp; // Low 32 bits of time stamp of image
	uint32_t PointerToSymbolTable; // Offset of symbol table
	uint32_t NumberOfSymbols; // Number of symbols in symbol table
	uint16_t SizeOfOptionalHeader; // Size of optional header in bytes
	uint16_t Characteristics; // Characteristics of image
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

typedef struct _IMAGE_DATA_DIRECTORY {
  uint32_t VirtualAddress; // Relative virtual address of table
  uint32_t Size; // Size of table in bytes
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;


typedef struct _IMAGE_OPTIONAL_HEADER {
	// Standard fields
	uint16_t Magic;
	uint8_t MajorLinkerVersion;
	uint8_t MinorLinkerVersion;
	uint32_t SizeOfCode; 
	uint32_t SizeOfInitializedData;
	uint32_t SizeOfUninitializedData;
	uint32_t AddressOfEntryPoint;
	uint32_t BaseOfCode;
	uint32_t BaseOfData;
	// NT additional fields
	uint32_t ImageBase;
	uint32_t SectionAlignment;
	uint32_t FileAlignment;
	uint16_t MajorOperatingSystemVersion;
	uint16_t MinorOperatingSystemVersion;
	uint16_t MajorImageVersion;
	uint16_t MinorImageVersion;
	uint16_t MajorSubsystemVersion;
	uint16_t MinorSubsystemVersion;
	uint32_t Win32VersionValue;
	uint32_t SizeOfImage;
	uint32_t SizeOfHeaders;
	uint32_t CheckSum;
	uint16_t Subsystem;
	uint16_t DllCharacteristics;
	uint32_t SizeOfStackReserve;
	uint32_t SizeOfStackCommit;
	uint32_t SizeOfHeapReserve;
	uint32_t SizeOfHeapCommit;
	uint32_t LoaderFlags;
	uint32_t NumberOfRvaAndSizes;
	IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER32, *PIMAGE_OPTIONAL_HEADER32;

typedef struct _IMAGE_OPTIONAL_HEADER64 {
	uint16_t Magic;
	uint8_t MajorLinkerVersion;
	uint8_t MinorLinkerVersion;
	uint32_t SizeOfCode;
	uint32_t SizeOfInitializedData;
	uint32_t SizeOfUninitializedData;
	uint32_t AddressOfEntryPoint;
	uint32_t BaseOfCode;
	uint64_t ImageBase;
	uint32_t SectionAlignment;
	uint32_t FileAlignment;
	uint16_t MajorOperatingSystemVersion;
	uint16_t MinorOperatingSystemVersion;
	uint16_t MajorImageVersion;
	uint16_t MinorImageVersion;
	uint16_t MajorSubsystemVersion;
	uint16_t MinorSubsystemVersion;
	uint32_t Win32VersionValue;
	uint32_t SizeOfImage;
	uint32_t SizeOfHeaders;
	uint32_t CheckSum;
	uint16_t Subsystem;
	uint16_t DllCharacteristics;
	uint64_t SizeOfStackReserve;
	uint64_t SizeOfStackCommit;
	uint64_t SizeOfHeapReserve;
	uint64_t SizeOfHeapCommit;
	uint32_t LoaderFlags;
	uint32_t NumberOfRvaAndSizes;
	IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER64, *PIMAGE_OPTIONAL_HEADER64;

typedef struct _IMAGE_NT_HEADERS32 { // 32 Bit Image NT Header
	uint32_t Signature; // 4 uint8_t signature identifying file as a PE image. The bytes are "PE\0\0" 
	IMAGE_FILE_HEADER FileHeader; // IMAGE_FILE_HEADER structure that specifies file header
	IMAGE_OPTIONAL_HEADER32 OptionalHeader; // IMAGE_OPTIONAL_HEADER strcture that specifies optional file header
} IMAGE_NT_HEADERS32, *PIMAGE_NT_HEADERS32;

typedef struct _IMAGE_NT_HEADERS64 { // 64 Bit Image NT Header
	uint32_t Signature; // 4 uint8_t signature identifying file as a PE image. The bytes are "PE\0\0"
	IMAGE_FILE_HEADER FileHeader; // IMAGE_FILE_HEADER structure that specifies file header
	IMAGE_OPTIONAL_HEADER64 OptionalHeader; // IMAGE_OPTIONAL_HEADER strcture that specifies optional file header
} IMAGE_NT_HEADERS64, *PIMAGE_NT_HEADERS64;

typedef struct _IMAGE_SECTION_HEADER {
	uint8_t Name[IMAGE_SIZEOF_SHORT_NAME];
	union {
		uint32_t PhysicalAddress;
		uint32_t VirtualSize;
	} Misc;
	uint32_t VirtualAddress;
	uint32_t SizeOfRawData;
	uint32_t PointerToRawData;
	uint32_t PointerToRelocations;
	uint32_t PointerToLinenumbers;
	uint16_t NumberOfRelocations;
	uint16_t NumberOfLinenumbers;
	uint32_t Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

class PE {
public:
	void ReadDosHeader(int fileDescriptor, IMAGE_DOS_HEADER* dosHeader);
	void GetDosHeader(IMAGE_DOS_HEADER* dosHeader);
	void GetPeFile(const std::string& peFile);
};

}