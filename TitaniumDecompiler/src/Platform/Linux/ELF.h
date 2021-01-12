#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <cstring>
#include <string>

// ELF File Format Structures 

// Data Types
// typedef unsigned char uint8_t;

// typedef uint32_t Elf32_Addr;
// typedef uint16_t Elf32_Half;
// typedef uint32_t Elf32_Off;
// typedef int32_t Elf32_Sword;
// typedef uint32_t Elf32_Word;

// typedef uint64_t Elf64_Addr;
// typedef uint16_t Elf64_Half;
// typedef int16_t Elf64_SHalf;
// typedef uint64_t Elf64_Off;
// typedef int32_t Elf64_Sword;
// typedef uint32_t Elf64_Word;
// typedef uint64_t Elf64_XWord;
// typedef int64_t Elf64_Sxword;

// E_TYPE
#define ET_NONE 0 // No file type
#define ET_REL 1 // Relocatable file
#define ET_EXEC 2 // Executable file
#define ET_DYN 3 // Shared object file
#define ET_CORE 4 // Core file
#define ET_LOPROC 0xFF00 // Processor-specific
#define ET_HIPROC 0xFFFF // Processor-specific

// E_MACHINE
#define EM_NONE  0 // No machine
#define EM_M32   1 // AT&T WE 32100
#define EM_SPARC 2 // SPARC
#define EM_386   3 // Intel 80386
#define EM_68K   4 // Motorola m68k family
#define EM_88K   5 // Motorola m88k family
#define EM_486   6 // Intel 80486// Reserved for future use
#define EM_860   7 // Intel 80860
#define EM_MIPS  8 // MIPS R3000 (officially, big-endian only)
#define EM_S370  9 // IBM System/370
#define EM_MIPS_RS3_LE 10 // MIPS R3000 little-endian (Oct 4 1999 Draft) Deprecated
#define EM_res011      11 // Reserved
#define EM_res012      12 // Reserved
#define EM_res013      13 // Reserved
#define EM_res014      14 // Reserved
#define EM_PARISC      15 // HPPA
#define EM_res016      16 // Reserved
#define EM_VPP550      17 // Fujitsu VPP500
#define EM_SPARC32PLUS 18 // Sun's "v8plus"
#define EM_960         19 // Intel 80960
#define EM_PPC         20 // PowerPC
#define EM_PPC64       21 // 64-bit PowerPC
#define EM_S390        22 // IBM S/390
#define EM_SPU         23 // Sony/Toshiba/IBM SPU
#define EM_res024      24 // Reserved
#define EM_res025      25 // Reserved
#define EM_res026      26 // Reserved
#define EM_res027      27 // Reserved
#define EM_res028      28 // Reserved
#define EM_res029      29 // Reserved
#define EM_res030      30 // Reserved
#define EM_res031      31 // Reserved
#define EM_res032      32 // Reserved
#define EM_res033      33 // Reserved
#define EM_res034      34 // Reserved
#define EM_res035      35 // Reserved
#define EM_V800        36 // NEC V800 series
#define EM_FR20        37 // Fujitsu FR20
#define EM_RH32        38 // TRW RH32
#define EM_MCORE       39 // Motorola M*Core // May also be taken by Fujitsu MMA
#define EM_RCE         39 // Old name for MCore
#define EM_ARM         40 // ARM
#define EM_OLD_ALPHA   41 // Digital Alpha
#define EM_SH          42 // Renesas (formerly Hitachi) / SuperH SH
#define EM_SPARCV9     43 // SPARC v9 64-bit
#define EM_TRICORE     44 // Siemens Tricore embedded processor
#define EM_ARC         45 // ARC Cores
#define EM_H8_300      46 // Renesas (formerly Hitachi) H8/300
#define EM_H8_300H     47 // Renesas (formerly Hitachi) H8/300H
#define EM_H8S         48 // Renesas (formerly Hitachi) H8S
#define EM_H8_500      49 // Renesas (formerly Hitachi) H8/500
#define EM_IA_64       50 // Intel IA-64 Processor
#define EM_MIPS_X      51 // Stanford MIPS-X
#define EM_COLDFIRE    52 // Motorola Coldfire
#define EM_68HC12      53 // Motorola M68HC12
#define EM_MMA         54 // Fujitsu Multimedia Accelerator
#define EM_PCP         55 // Siemens PCP
#define EM_NCPU        56 // Sony nCPU embedded RISC processor
#define EM_NDR1        57 // Denso NDR1 microprocesspr
#define EM_STARCORE    58 // Motorola Star*Core processor
#define EM_ME16        59 // Toyota ME16 processor
#define EM_ST100       60 // STMicroelectronics ST100 processor
#define EM_TINYJ       61 // Advanced Logic Corp. TinyJ embedded processor
#define EM_X86_64      62 // Advanced Micro Devices X86-64 processor
#define EM_PDSP        63 // Sony DSP Processor
#define EM_PDP10       64 // Digital Equipment Corp. PDP-10
#define EM_PDP11       65 // Digital Equipment Corp. PDP-11
#define EM_FX66        66 // Siemens FX66 microcontroller
#define EM_ST9PLUS     67 // STMicroelectronics ST9+ 8/16 bit microcontroller
#define EM_ST7         68 // STMicroelectronics ST7 8-bit microcontroller
#define EM_68HC16      69 // Motorola MC68HC16 Microcontroller
#define EM_68HC11      70 // Motorola MC68HC11 Microcontroller
#define EM_68HC08      71 // Motorola MC68HC08 Microcontroller
#define EM_68HC05      72 // Motorola MC68HC05 Microcontroller
#define EM_SVX         73 // Silicon Graphics SVx
#define EM_ST19        74 // STMicroelectronics ST19 8-bit cpu
#define EM_VAX         75 // Digital VAX
#define EM_CRIS        76 // Axis Communications 32-bit embedded processor
#define EM_JAVELIN     77 // Infineon Technologies 32-bit embedded cpu
#define EM_FIREPATH    78 // Element 14 64-bit DSP processor
#define EM_ZSP         79 // LSI Logic's 16-bit DSP processor
#define EM_MMIX        80 // Donald Knuth's educational 64-bit processor
#define EM_HUANY       81 // Harvard's machine-independent format
#define EM_PRISM       82 // SiTera Prism
#define EM_AVR         83 // Atmel AVR 8-bit microcontroller
#define EM_FR30        84 // Fujitsu FR30
#define EM_D10V        85 // Mitsubishi D10V
#define EM_D30V        86 // Mitsubishi D30V
#define EM_V850        87 // NEC v850
#define EM_M32R        88 // Renesas M32R (formerly Mitsubishi M32R)
#define EM_MN10300     89 // Matsushita MN10300
#define EM_MN10200     90 // Matsushita MN10200
#define EM_PJ          91 // picoJava
#define EM_OPENRISC    92 // OpenRISC 32-bit embedded processor
#define EM_ARC_A5      93 // ARC Cores Tangent-A5
#define EM_XTENSA      94 // Tensilica Xtensa Architecture
#define EM_VIDEOCORE   95 // Alphamosaic VideoCore processor
#define EM_TMM_GPP     96 // Thompson Multimedia General Purpose Processor
#define EM_NS32K       97 // National Semiconductor 32000 series
#define EM_TPC         98 // Tenor Network TPC processor
#define EM_SNP1K       99 // Trebia SNP 1000 processor
#define EM_ST200       100 // STMicroelectronics ST200 microcontroller
#define EM_IP2K        101 // Ubicom IP2022 micro controller
#define EM_MAX         102 // MAX Processor
#define EM_CR          103 // National Semiconductor CompactRISC
#define EM_F2MC16      104 // Fujitsu F2MC16
#define EM_MSP430      105 // TI msp430 micro controller
#define EM_BLACKFIN    106 // ADI Blackfin
#define EM_SE_C33      107 // S1C33 Family of Seiko Epson processors
#define EM_SEP         108 // Sharp embedded microprocessor
#define EM_ARCA        109 // Arca RISC Microprocessor
#define EM_UNICORE     110 // Microprocessor series from PKU-Unity Ltd. and MPRC of Peking University
#define EM_EXCESS       111 // eXcess: 16/32/64-bit configurable embedded CPU
#define EM_DXP          112 // Icera Semiconductor Inc. Deep Execution Processor
#define EM_ALTERA_NIOS2 113 // Altera Nios II soft-core processor
#define EM_CRX          114 // National Semiconductor CRX
#define EM_XGATE        115 // Motorola XGATE embedded processor
#define EM_C166         116 // Infineon C16x/XC16x processor
#define EM_M16C         117 // Renesas M16C series microprocessors
#define EM_DSPIC30F     118 // Microchip Technology dsPIC30F Digital Signal Controller
#define EM_CE            119 // Freescale Communication Engine RISC core
#define EM_M32C          120 // Renesas M32C series microprocessors
#define EM_res121        121 // Reserved
#define EM_res122        122 // Reserved
#define EM_res123        123 // Reserved
#define EM_res124        124 // Reserved
#define EM_res125        125 // Reserved
#define EM_res126        126 // Reserved
#define EM_res127        127 // Reserved
#define EM_res128        128 // Reserved
#define EM_res129        129 // Reserved
#define EM_res130        130 // Reserved
#define EM_TSK3000       131 // Altium TSK3000 core
#define EM_RS08          132 // Freescale RS08 embedded processor
#define EM_res133        133 // Reserved
#define EM_ECOG2         134 // Cyan Technology eCOG2 microprocessor
#define EM_SCORE         135 // Sunplus Score
#define EM_SCORE7        135 // Sunplus S+core7 RISC processor
#define EM_DSP24         136 // New Japan Radio (NJR) 24-bit DSP Processor
#define EM_VIDEOCORE3    137 // Broadcom VideoCore III processor
#define EM_LATTICEMICO32 138 // RISC processor for Lattice FPGA architecture
#define EM_SE_C17        139 // Seiko Epson C17 family
#define EM_TI_C6000      140 // Texas Instruments TMS320C6000 DSP family
#define EM_TI_C2000      141 // Texas Instruments TMS320C2000 DSP family
#define EM_TI_C5500      142 // Texas Instruments TMS320C55x DSP family
#define EM_res143        143 // Reserved
#define EM_res144        144 // Reserved
#define EM_res145        145 // Reserved
#define EM_res146        146 // Reserved
#define EM_res147        147 // Reserved
#define EM_res148        148 // Reserved
#define EM_res149        149 // Reserved
#define EM_res150        150 // Reserved
#define EM_res151        151 // Reserved
#define EM_res152        152 // Reserved
#define EM_res153        153 // Reserved
#define EM_res154        154 // Reserved
#define EM_res155        155 // Reserved
#define EM_res156        156 // Reserved
#define EM_res157        157 // Reserved
#define EM_res158        158 // Reserved
#define EM_res159        159 // Reserved
#define EM_MMDSP_PLUS    160 // STMicroelectronics 64bit VLIW Data Signal Processor
#define EM_CYPRESS_M8C   161 // Cypress M8C microprocessor
#define EM_R32C          162 // Renesas R32C series microprocessors
#define EM_TRIMEDIA      163 // NXP Semiconductors TriMedia architecture family
#define EM_QDSP6         164 // QUALCOMM DSP6 Processor
#define EM_8051          165 // Intel 8051 and variants
#define EM_STXP7X        166 // STMicroelectronics STxP7x family
#define EM_NDS32           167 // Andes Technology compact code size embedded RISC processor family
#define EM_ECOG1           168 // Cyan Technology eCOG1X family
#define EM_ECOG1X          168 // Cyan Technology eCOG1X family
#define EM_MAXQ30          169 // Dallas Semiconductor MAXQ30 Core Micro-controllers
#define EM_XIMO16          170 // New Japan Radio (NJR) 16-bit DSP Processor
#define EM_MANIK           171 // M2000 Reconfigurable RISC Microprocessor
#define EM_CRAYNV2         172 // Cray Inc. NV2 vector architecture
#define EM_RX              173 // Renesas RX family
#define EM_METAG           174 // Imagination Technologies META processor architecture
#define EM_MCST_ELBRUS     175 // MCST Elbrus general purpose hardware architecture
#define EM_ECOG16          176 // Cyan Technology eCOG16 family
#define EM_CR16            177 // National Semiconductor CompactRISC 16-bit processor
#define EM_ETPU            178 // Freescale Extended Time Processing Unit
#define EM_SLE9X           179 // Infineon Technologies SLE9X core
#define EM_L1OM            180 // Intel L1OM
#define EM_K10M            181 // Intel K10M
#define EM_INTEL182        182 // Reserved by Intel
#define EM_AARCH64         183 // ARM AARCH64
#define EM_res184          184 // Reserved by ARM
#define EM_AVR32           185 // Atmel Corporation 32-bit microprocessor family
#define EM_STM8            186 // STMicroeletronics STM8 8-bit microcontroller
#define EM_TILE64          187 // Tilera TILE64 multicore architecture family
#define EM_TILEPRO         188 // Tilera TILEPro multicore architecture family
#define EM_MICROBLAZE      189 // Xilinx MicroBlaze 32-bit RISC soft processor core
#define EM_CUDA            190 // NVIDIA CUDA architecture
#define EM_TILEGX          191 // Tilera TILE-Gx multicore architecture family
#define EM_CLOUDSHIELD     192 // CloudShield architecture family
#define EM_COREA_1ST       193 // KIPO-KAIST Core-A 1st generation processor family
#define EM_COREA_2ND       194 // KIPO-KAIST Core-A 2nd generation processor family
#define EM_ARC_COMPACT2    195 // Synopsys ARCompact V2
#define EM_OPEN8           196 // Open8 8-bit RISC soft processor core
#define EM_RL78            197 // Renesas RL78 family
#define EM_VIDEOCORE5      198 // Broadcom VideoCore V processor
#define EM_78KOR           199 // Renesas 78KOR family
#define EM_56800EX         200 // Freescale 56800EX Digital Signal Controller (DSC)
#define EM_BA1             201 // Beyond BA1 CPU architecture
#define EM_BA2             202 // Beyond BA2 CPU architecture
#define EM_XCORE           203 // XMOS xCORE processor family
#define EM_MCHP_PIC        204 // Microchip 8-bit PIC(r) family
#define EM_INTEL205        205 // Reserved by Intel
#define EM_INTEL206        206 // Reserved by Intel
#define EM_INTEL207        207 // Reserved by Intel
#define EM_INTEL208        208 // Reserved by Intel
#define EM_INTEL209        209 // Reserved by Intel
#define EM_KM32            210 // KM211 KM32 32-bit processor
#define EM_KMX32           211 // KM211 KMX32 32-bit processor
#define EM_KMX16           212 // KM211 KMX16 16-bit processor
#define EM_KMX8            213 // KM211 KMX8 8-bit processor
#define EM_KVARC           214 // KM211 KVARC processor
#define EM_CDP             215 // Paneve CDP architecture family
#define EM_COGE            216 // Cognitive Smart Memory Processor
#define EM_COOL            217 // iCelero CoolEngine
#define EM_NORC            218 // Nanoradio Optimized RISC
#define EM_CSR_KALIMBA     219 // CSR Kalimba architecture family
#define EM_Z80             220 // Zilog Z80
#define EM_VISIUM          221 // Controls and Data Services VISIUMcore processor
#define EM_FT32            222 // FTDI Chip FT32 high performance 32-bit RISC architecture
#define EM_MOXIE           223 // Moxie processor family
#define EM_AMDGPU          224 // AMD GPU architecture
#define EM_RISCV           243 // RISC-V
#define EM_LANAI           244 // Lanai processor
#define EM_CEVA            245 // CEVA Processor Architecture Family
#define EM_CEVA_X2         246 // CEVA X2 Processor Family
#define EM_BPF             247 // Linux BPF – in-kernel virtual machine
#define EM_GRAPHCORE_IPU   248 // Graphcore Intelligent Processing Unit
#define EM_IMG1            249 // Imagination Technologies
#define EM_NFP             250 // Netronome Flow Processor (P)
#define EM_CSKY            252 // C-SKY processor family
#define EM_ARC_COMPACT3_64 253 // "Synopsys ARCv2.3 64-bit"
#define EM_MCS6502         254 // "MOS Technology MCS 6502 processor"
#define EM_ARC_COMPACT3    255 // "Synopsys ARCv2.3 32-bit"
#define EM_KVX             256 // "Kalray VLIW core of the MPPA processor family"
#define EM_65816           257 // "WDC 65816/65C816"
#define EM_LOONGARCH       258 // "Loongson Loongarch"
#define EM_KF32            259 // "ChipON KungFu32"

#define EM_MT                0x2530 // "Morpho Techologies MT processor"
#define EM_ALPHA             0x9026 // "Alpha"
#define EM_WEBASSEMBLY       0x4157 // "Web Assembly"
#define EM_DLX               0x5aa5 // "OpenDLX"
#define EM_XSTORMY16         0xad45 // "Sanyo XStormy16 CPU core"
#define EM_IQ2000            0xFEBA // "Vitesse IQ2000"
#define EM_M32C_OLD          0xFEB0
#define EM_NIOS32            0xFEBB // "Altera Nios"
#define EM_CYGNUS_MEP        0xF00D // "Toshiba MeP Media Engine"
#define EM_ADAPTEVA_EPIPHANY 0x1223 // "Adapteva EPIPHANY"
#define EM_CYGNUS_FRV        0x5441 // "Fujitsu FR-V"
#define EM_S12Z              0x4DEF // "Freescale S12Z"

// E_VERSION
#define EV_NONE 0 // Invalid version
#define EV_CURRENT 1 // Current Version


// E_IDENT
#define EI_MAG0 0 // File identification
#define EI_MAG1 1 // File identification
#define EI_MAG2 2 // File identification
#define EI_MAG3 3 // File identification
#define EI_CLASS 4 // File class
#define EI_DATA 5 // Data encoding
#define EI_VERSION 6 // File version
#define EI_OSABI 7 // OS ABI
#define EI_ABIVERSION 8 // ABI Version
#define EI_PAD 9 // Start of padding bytes
#define EI_NIDENT 16 // Size of e_ident

// Magic Numbers
#define ELFMAG0 0x7F // e_ident[EI_MAG0]
#define ELFMAG1 'E'  // e_ident[EI_MAG1]
#define ELFMAG2 'L' // e_ident[EI_MAG2]
#define ELFMAG3 'F' // e_ident[EI_MAG3]

// EI_CLASS
#define ELFCLASSNONE 0 // Invalid class
#define ELFCLASS32 1 // 32-bit objects
#define ELFCLASS64 2 // 64-bit objects

// EI_DATA
#define ELFDATANONE 0 // Invalid data encoding
#define ELFDATA2LSB 1 // Specifies 2's complement values with least significant byte occupying the lowest address
#define ELFDATA2MSB 2 // Specifies 2's complement values with most significant byte occupying the lowest address 

// OS extensions
#define ELFOSABI_NONE		0	// UNIX System V ABI 
#define ELFOSABI_SYSV		0	// Alias.  
#define ELFOSABI_HPUX		1	// HP-UX 
#define ELFOSABI_NETBSD		2	// NetBSD.  
#define ELFOSABI_GNU		3	// Object uses GNU ELF extensions.  
#define ELFOSABI_LINUX		ELFOSABI_GNU // Compatibility alias.  
#define ELFOSABI_SOLARIS	6	// Sun Solaris.  
#define ELFOSABI_AIX		7	// IBM AIX.  
#define ELFOSABI_IRIX		8	// SGI Irix.  
#define ELFOSABI_FREEBSD	9	// FreeBSD.  
#define ELFOSABI_TRU64		10	// Compaq TRU64 UNIX.  
#define ELFOSABI_MODESTO	11	// Novell Modesto.  
#define ELFOSABI_OPENBSD	12	// OpenBSD.  
#define ELFOSABI_ARM_AEABI	64	// ARM EABI 
#define ELFOSABI_ARM		97	// ARM 
#define ELFOSABI_STANDALONE	255	// Standalone (embedded) application 

// Sections
#define SHN_UNDEF 0 // Marks undefined, missing, irrelevant, otherwise meaningless section reference
#define SHN_LORESERVE 0xFF00 // Value specifies lower bound of the range reserved indexes
#define SHN_LOPROC 0xFF00 // SHN_LOPROC through SHN_HIPROC Values inclusive range reserved for processor-specific semantics
#define SHN_HIPROC 0xFF1F
#define SHN_ABS 0xFFF1 // Value specifies absolute values for corresponding reference. For example, symbols defined relative to section number SHN_ABS have absolute values and are not affected by relocation
#define SHN_COMMON 0xFFF2 // Symbols defined relative to this section are common symbols, such as FORTRAN COMMON or unallocated C external variables
#define SHN_HIRESERVE 0xFFFF // Value specifies upper bound of range of reserved indexes. System reserves indexes between SHN_LORESERVE and HIRESERVE, inclusive; the values do not reference section header table. That is the section header table does not contain entries for reserved indexes

// Section Types
#define SHT_NULL          0 // Value marks section header as inactive
#define SHT_PROGBITS      1 // Holds information defined by program.
#define SHT_SYMTAB        2 // Holds symbol table. Provides symbols for link editing 
#define SHT_STRTAB        3 // Holds string table.
#define SHT_RELA          4 // Holds relocation table
#define SHT_HASH          5 // Holds symbol hash table
#define SHT_DYNAMIC       6 // Holds information for dynamic linking
#define SHT_NOTE          7 // Holds information that marks file in some way
#define SHT_NOBITS        8 // Section of this type occupies no space in file but otherwise resembles SHT_PROGBITS
#define SHT_REL           9 // Holds relaction entries without explicit addends such as type Elf32_Rel
#define SHT_SHLIB         10 // Section type is reserved but has unspecified semantics
#define SHT_DYNSYM        11 // Holds symbol table. Holds a minimal set of dynamic linking symbols to save space
#define SHT_INIT_ARRAY    14
#define SHT_FINI_ARRAY    15
#define SHT_PREINIT_ARRAY 16
#define SHT_GROUP         17
#define SHT_SYMTAB_SHNDX  18
#define SHT_LOOS          0x60000000
#define SHT_HIOS          0x6fffffff
#define SHT_LOPROC        0x70000000
#define SHT_HIPROC        0x7FFFFFFF
#define SHT_LOUSER        0x80000000
#define SHT_HIUSER        0xFFFFFFFF

// Section Attributes
#define SHF_WRITE            0x1 // Section contains data that should be writable during process executation
#define SHF_ALLOC            0x2 // Section occupies memory during process exection. Some control sections do not reside in memory image of an object file; this attribute is off for those sections
#define SHF_EXECINSTR        0x4 // Section contains executable machine instructions
#define SHF_MERGE            0x10
#define SHF_STRINGS          0x20
#define SHF_INFO_LINK        0x40
#define SHF_LINK_ORDER       0x80
#define SHF_OS_NONCONFORMING 0x100
#define SHF_GROUP            0x200
#define SHF_TLS              0x400
#define SHF_MASKOS           0x0ff00000
#define SHF_MASKPROC         0xF0000000 // All bits included in this mask are reserved for processor-specific semantics

// Section Group Flags
#define GRP_COMDAT   0x1
#define GRP_MASKOS   0x0ff00000
#define GRP_MASKPROC 0xf0000000

// Symbol Binding
#define STB_LOCAL    0 // Local symbols are not visible outside object file containing their definition. Local symbols of same name may exist in multiple files without interfering with each other
#define STB_GLOBAL   1 // Global symbols are visible to all object files being combined
#define STB_WEAK     2 // Weak symbols resemble global symbols but their definitions have lower precedence
#define STB_LOOS     10
#define STB_HIOS     12
#define STB_MULTIDEF 13
#define STB_LOPROC   13 // STB_LOPROC through STB_HIPROC values in this inclusive range are reserved for processor-specific semantics
#define STB_HIPROC   15

// Note Types
#define NT_AMDGPU_METADATA         1
#define NT_AMD_AMDGPU_HSA_METADATA 10
#define NT_AMD_AMDGPU_ISA          11
#define NT_AMD_AMDGPU_PAL_METADATA 12

// Symbol Types
#define STT_NOTYPE            0 // Symbol type not specified
#define STT_OBJECT            1 // Symbol associated with data object, such as variable, array, etc
#define STT_FUNC              2 // Symbol associated with function or other executable code
#define STT_SECTION           3 // Symbol associated with a section. Symbol table entries of this type exist primarily for relocation and normally have STB_LOCAL binding
#define STT_FILE              4 // Symbol name gives name of source file associated with object file
#define STT_COMMON            5
#define STT_TLS               6
#define STT_LOOS              10
#define STT_AMDGPU_HSA_KERNEL 10
#define STT_HIOS              12
#define STT_LOPROC            13 // STT_LOPROC through STT_HIPROC values in this inclusive range are reserved for processor-specific semantics
#define STT_HIPROC            15

// Symbol Visibility
#define STV_DEFAULT   0
#define STV_INTERNAL  1
#define STV_HIDDEN    2
#define STV_PROTECTED 3

// Undefined Symbol Index
#define STN_UNDEF 0

// Relocation Types
#define R_386_NONE               0
#define R_X86_64_NONE            0
#define R_AMDGPU_NONE            0
#define R_386_32                 1
#define R_X86_64_64              1
#define R_AMDGPU_ABS32_LO        1
#define R_386_PC32               2
#define R_X86_64_PC32            2
#define R_AMDGPU_ABS32_HI        2
#define R_386_GOT32              3 // Relocation type computes distance from base of global offset table to symbols global offset table entry
#define R_X86_64_GOT32           3
#define R_AMDGPU_ABS64           3
#define R_386_PLT32              4 // Relocation type computes distance from base of procedure linkage table to symbols procedure linkage table entry
#define R_X86_64_PLT32           4
#define R_AMDGPU_REL32           4
#define R_386_COPY               5
#define R_X86_64_COPY            5
#define R_AMDGPU_REL64           5
#define R_386_GLOB_DAT           6
#define R_X86_64_GLOB_DAT        6
#define R_AMDGPU_ABS32           6
#define R_386_JMP_SLOT           7
#define R_X86_64_JUMP_SLOT       7
#define R_AMDGPU_GOTPCREL        7
#define R_386_RELATIVE           8
#define R_X86_64_RELATIVE        8
#define R_AMDGPU_GOTPCREL32_LO   8
#define R_386_GOTOFF             9
#define R_X86_64_GOTPCREL        9
#define R_AMDGPU_GOTPCREL32_HI   9
#define R_386_GOTPC              10
#define R_X86_64_32              10
#define R_AMDGPU_REL32_LO        10
#define R_386_32PLT              11
#define R_X86_64_32S             11
#define R_AMDGPU_REL32_HI        11
#define R_X86_64_16              12
#define R_X86_64_PC16            13
#define R_AMDGPU_RELATIVE64      13
#define R_386_TLS_TPOFF          14
#define R_X86_64_8               14
#define R_386_TLS_IE             15
#define R_X86_64_PC8             15
#define R_386_TLS_GOTIE          16
#define R_X86_64_DTPMOD64        16
#define R_386_TLS_LE             17
#define R_X86_64_DTPOFF64        17
#define R_386_TLS_GD             18
#define R_X86_64_TPOFF64         18
#define R_386_TLS_LDM            19
#define R_X86_64_TLSGD           19
#define R_386_16                 20
#define R_X86_64_TLSLD           20
#define R_386_PC16               21
#define R_X86_64_DTPOFF32        21
#define R_386_8                  22
#define R_X86_64_GOTTPOFF        22
#define R_386_PC8                23
#define R_X86_64_TPOFF32         23
#define R_386_TLS_GD_32          24
#define R_X86_64_PC64            24
#define R_386_TLS_GD_PUSH        25
#define R_X86_64_GOTOFF64        25
#define R_386_TLS_GD_CALL        26
#define R_X86_64_GOTPC32         26
#define R_386_TLS_GD_POP         27
#define R_X86_64_GOT64           27
#define R_386_TLS_LDM_32         28
#define R_X86_64_GOTPCREL64      28
#define R_386_TLS_LDM_PUSH       29
#define R_X86_64_GOTPC64         29
#define R_386_TLS_LDM_CALL       30
#define R_X86_64_GOTPLT64        30
#define R_386_TLS_LDM_POP        31
#define R_X86_64_PLTOFF64        31
#define R_386_TLS_LDO_32         32
#define R_386_TLS_IE_32          33
#define R_386_TLS_LE_32          34
#define R_X86_64_GOTPC32_TLSDESC 34
#define R_386_TLS_DTPMOD32       35
#define R_X86_64_TLSDESC_CALL    35
#define R_386_TLS_DTPOFF32       36
#define R_X86_64_TLSDESC         36
#define R_386_TLS_TPOFF32        37
#define R_X86_64_IRELATIVE       37
#define R_386_SIZE32             38
#define R_386_TLS_GOTDESC        39
#define R_386_TLS_DESC_CALL      40
#define R_386_TLS_DESC           41
#define R_386_IRELATIVE          42
#define R_386_GOT32X             43
#define R_X86_64_GNU_VTINHERIT   250
#define R_X86_64_GNU_VTENTRY     251

// Segment Types
#define PT_NULL    0
#define PT_LOAD    1
#define PT_DYNAMIC 2
#define PT_INTERP  3
#define PT_NOTE    4
#define PT_SHLIB   5
#define PT_PHDR    6
#define PT_TLS     7
#define PT_LOOS    0x60000000
#define PT_HIOS    0x6fffffff
#define PT_LOPROC  0x70000000
#define PT_HIPROC  0x7FFFFFFF

// Segment Flags
#define PF_X        1          // Execute
#define PF_W        2          // Write
#define PF_R        4          // Read
#define PF_MASKOS   0x0ff00000 // Unspecified
#define PF_MASKPROC 0xf0000000 // Unspecified

// Dynamic Array Tags
#define DT_NULL            0
#define DT_NEEDED          1
#define DT_PLTRELSZ        2
#define DT_PLTGOT          3
#define DT_HASH            4
#define DT_STRTAB          5
#define DT_SYMTAB          6
#define DT_RELA            7
#define DT_RELASZ          8
#define DT_RELAENT         9
#define DT_STRSZ           10
#define DT_SYMENT          11
#define DT_INIT            12
#define DT_FINI            13
#define DT_SONAME          14
#define DT_RPATH           15
#define DT_SYMBOLIC        16
#define DT_REL             17
#define DT_RELSZ           18
#define DT_RELENT          19
#define DT_PLTREL          20
#define DT_DEBUG           21
#define DT_TEXTREL         22
#define DT_JMPREL          23
#define DT_BIND_NOW        24
#define DT_INIT_ARRAY      25
#define DT_FINI_ARRAY      26
#define DT_INIT_ARRAYSZ    27
#define DT_FINI_ARRAYSZ    28
#define DT_RUNPATH         29
#define DT_FLAGS           30
#define DT_ENCODING        32
#define DT_PREINIT_ARRAY   32
#define DT_PREINIT_ARRAYSZ 33
#define DT_MAXPOSTAGS      34
#define DT_LOOS            0x6000000D
#define DT_HIOS            0x6ffff000
#define DT_LOPROC          0x70000000
#define DT_HIPROC          0x7FFFFFFF

// DT_FLAGS Values
#define DF_ORIGIN     0x1
#define DF_SYMBOLIC   0x2
#define DF_TEXTREL    0x4
#define DF_BIND_NOW   0x8
#define DF_STATIC_TLS 0x10

namespace TitaniumDecompiler {

//32 BIT

// File Header
typedef struct {
    uint8_t e_ident[EI_NIDENT]; // Magic number and other info
    uint16_t e_type; // Object file type
    uint16_t e_machine; // Architecture
    uint32_t e_version; // Version info
    uint32_t e_entry; // Entry point
    uint32_t e_phoff; // Points to start of program header table
    uint32_t e_shoff; // Points to start of section header table
    uint32_t e_flags; // Interpretation of this field depends on target architecture
    uint16_t e_ehsize; // Contains size of this header
    uint16_t e_phentsize; // Contains size of a program header table entry
    uint16_t e_phnum; // Contains number of entries in program header table
    uint16_t e_shentsize; // Contains size of a section header table entry
    uint16_t e_shnum; // Contains number of entries in section header table
    uint16_t e_shstrndx; // Contains index of section header table entry that contains section names.
} Elf32Hdr;

// Program Header
typedef struct {
    uint32_t p_type; // Identifies type of segment
    uint32_t p_offset; // Offset of segment in file image
    uint32_t p_vaddr; // Virtual address of segment in memory
    uint32_t p_paddr; // On systems where physical address is relevant, reserved for segments physical address
    uint32_t p_filesz; // Size in bytes of segment in file image. May be zero
    uint32_t p_memsz; // Size in bytes of segment in memory. May be zero
    uint32_t p_flags; // Segment-dependent flags(position for 32-bit structure)
    uint32_t p_align; // 0 and 1 specify no alignment. Otherwise should be positive integral power of 2 with p_vaddr equating p_offset modulus p_align
} Elf32PHdr;

// Section Header
typedef struct {
    uint32_t sh_name; // Offset to string in .shstrtab section represents name of this section
    uint32_t sh_type; // Identifies type of this header
    uint32_t sh_flags; // Identifies attributes of section
    uint32_t sh_addr; // Virtual address of section in memory for sections that are loaded
    uint32_t sh_offset; // Offset of section in file image
    uint32_t sh_size; // Size in bytes of section in file image. May be zero
    uint32_t sh_link; // Contains section index of associated section. This field is used for several purposes depending on type of section
    uint32_t sh_info; // Contains extra information about section. This field is used for several purposes depending on type of section
    uint32_t sh_addralign; // Contains required alignment of section. This field must be power of two
    uint32_t sh_entsize; // Contains size in bytes of each entry for sections that contain fixed-size entries. Otherwise this field contains zero 
} Elf32SHdr;

// Symbol table
typedef struct {
    uint32_t st_name; // Holds index into files symbol string table which holds character representations of symbol names
    uint32_t st_values; // Gives value of associated symbol depending on context this may be an absolute value an address so on
    uint32_t st_size; // Symbols have associated sizes data objects size is number of bytes contained in object. This member holds 0 if symbol has no size or an unknown size
    uint8_t st_info; // Specifies symbols type and binding attributes
    uint8_t st_other; // Member currently hold 0 and has no defined meaning
    uint16_t st_shndx; // Every symbol table entry is "defined" in relation to some section this member holds relevant section header table index
} Elf32Sym;

// Relocation
typedef struct {
    uint32_t r_offset; // Gives the location at which to apply the relocation action. For a relocatable file, the value is the byte offset from the beginning of the section to the storage unit affected by the relocation. For an executable file or a shared object, the value is the virtual address of the storage unit affected by the relocation.
    uint32_t r_info; // Gives both the symbol table index with respect to which the relocation must be made, and the type of relocation to apply. For example, a call instruction's relocation entry would hold the symbol table index of the function being called. If the index is STN_UNDEF, the undefined symbol index, the relocation uses 0 as the "symbol value.'' Relocation types are processor-specific; descriptions of their behavior appear in the processor supplement. When the text in the processor supplement refers to a relocation entry's relocation type or symbol table index, it means the result of applying ELF32_R_TYPE or ELF32_R_SYM, respectively, to the entry's r_info member. 
} Elf32Rel;

// Relocation
typedef struct {
    uint32_t r_offset;
    uint32_t r_info;
    int32_t r_addend; // Specifies a constant addend used to compute value to be stored into relocatable field
} Elf32Rela;

#define ELF32_R_SYM(i) ((i) >> 8)
#define ELF32_R_TYPE(i) ((uint8_t)(i))
#define ELF32_R_INFO(s, t) (((s) << 8) + (uint8_t)(t))

// Dynamic structure
typedef struct {
    int32_t d_tag;
    union {
        uint32_t d_val;
        uint32_t d_ptr;
    } d_un;
} Elf32Dyn;

//32 and 64 BIT

// File Header
typedef struct {
    uint8_t e_ident[EI_NIDENT]; // Magic number and other info
    uint16_t e_type; // Object file type
    uint16_t e_machine; // Architecture
    uint32_t e_version; // Version info
    uint64_t e_entry; // Entry point
    uint64_t e_phoff; // Points to start of program header table
    uint64_t e_shoff; // Points to start of section header table
    uint32_t e_flags; // Interpretation of this field depends on target architecture
    uint16_t e_ehsize; // Contains size of this header
    uint16_t e_phentsize; // Contains size of a program header table entry
    uint16_t e_phnum; // Contains number of entries in program header table
    uint16_t e_shentsize; // Contains size of a section header table entry
    uint16_t e_shnum; // Contains number of entries in section header table
    uint16_t e_shstrndx; // Contains index of section header table entry that contains section names.
} Elf64Hdr;

// Program Header
typedef struct {
    uint32_t p_type; // Identifies type of segment
    uint32_t p_flags; // Segment-depended flags(position for 64-bit structure)
    uint64_t p_offset; // Offset of segment in file image
    uint64_t p_vaddr; // Virtual address of segment in memory
    uint64_t p_paddr; // On systems where physical address is relevant, reserved for segments physical address
    uint64_t p_filesz; // Size in bytes of segment in file image. May be zero
    uint64_t p_memsz; // Size in bytes of segment in memory. May be zero
    uint64_t p_align; // 0 and 1 specify no alignment. Otherwise should be positive integral power of 2 with p_vaddr equating p_offset modulus p_align
} Elf64PHdr;

// Section Header
typedef struct {
    uint32_t sh_name; // Offset to string in .shstrtab section represents name of this section
    uint32_t sh_type; // Identifies type of this header
    uint64_t sh_flags; // Identifies attributes of section
    uint64_t sh_addr; // Virtual address of section in memory for sections that are loaded
    uint64_t sh_offset; // Offset of section in file image
    uint64_t sh_size; // Size in bytes of section in file image. May be zero
    uint32_t sh_link; // Contains section index of associated section. This field is used for several purposes depending on type of section
    uint32_t sh_info; // Contains extra information about section. This field is used for several purposes depending on type of section
    uint64_t sh_addralign; // Contains required alignment of section. This field must be power of two
    uint64_t sh_entsize; // Contains size in bytes of each entry for sections that contain fixed-size entries. Otherwise this field contains zero 
} Elf64SHdr;

// Symbol table
typedef struct {
    uint32_t st_name; // Holds index into files symbol string table which holds character representations of symbol names
    uint8_t st_info; // Specifies symbols type and binding attributes
    uint8_t st_other; // Member currently hold 0 and has no defined meaning
    uint32_t st_shndx; // Every symbol table entry is "defined" in relation to some section this member holds relevant section header table index
    uint64_t st_value; // Gives value of associated symbol depending on context this may be an absolute value an address so on
    uint64_t st_size; // Symbols have associated sizes data objects size is number of bytes contained in object. This member holds 0 if symbol has no size or an unknown size
} Elf64Sym;

#define ELF_ST_BIND(i) ((i) >> 4)
#define ELF_ST_TYPE(i) ((i) & 0xF)
#define ELF_ST_INFO(b, t) ((b) << 4) + ((t) & 0xF))

#define ELF_ST_VISIBILITY(o) ((o) & 0x03)

// Relocation
typedef struct {
    uint64_t r_offset; // Gives the location at which to apply the relocation action. For a relocatable file, the value is the byte offset from the beginning of the section to the storage unit affected by the relocation. For an executable file or a shared object, the value is the virtual address of the storage unit affected by the relocation.
    uint64_t r_info; // Gives both the symbol table index with respect to which the relocation must be made, and the type of relocation to apply. For example, a call instruction's relocation entry would hold the symbol table index of the function being called. If the index is STN_UNDEF, the undefined symbol index, the relocation uses 0 as the "symbol value.'' Relocation types are processor-specific; descriptions of their behavior appear in the processor supplement. When the text in the processor supplement refers to a relocation entry's relocation type or symbol table index, it means the result of applying ELF32_R_TYPE or ELF32_R_SYM, respectively, to the entry's r_info member. 
} Elf64Rel;

// Relocation
typedef struct {
    uint64_t r_offset;
    uint64_t r_info;
    int64_t r_addend; // Specifies a constant addend used to compute value to be stored into relocatable field
} Elf64Rela;

#define ELF64_R_SYM(i) ((i) >> 32)
#define ELF64_R_TYPE(i) ((i) & 0xFFFFFFFFL)
#define ELF64_R_INFO(s, t) ((((int64_t)(s)) << 32) + ((t) & 0xFFFFFFFFL))

// Dynamic structure
typedef struct {
    int64_t d_tag;
    union {
        uint64_t d_val;
        uint64_t d_ptr;
    } d_un;
} Elf64Dyn;

class ELF {
public:
    void ReadElfHeader(const char* elfFile, Elf64Hdr* header64);
    void GetElfHeader(Elf64Hdr header64);
    void GetSectionHeaderTable(FILE* elfFile, Elf64Hdr header64, Elf64SHdr sh_table[]);
    char* GetSections(FILE* elfFile, Elf64SHdr sectionHeader64);
    void GetSectionHeader(FILE* elfFile, Elf64Hdr header64, Elf64SHdr sh_table[]);
    char* GetSectionHeaderForImGui(FILE* elfFile, Elf64Hdr header64, Elf64SHdr sh_table[]);
    void GetSymbolTable(FILE* elfFile, Elf64Hdr header64, Elf64SHdr sh_table[], uint32_t symbol_table);
    void GetSymbols(FILE* elfFile, Elf64Hdr header64, Elf64SHdr sh_table[]);
    void GetElfFileSections(const std::string& elfFile);
    void GetElfFile(const std::string& elfFile);
    // bool IsElfFile(const char* elfFile);
};
}