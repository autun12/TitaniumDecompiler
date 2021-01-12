#include "tdpch.h"
#include <capstone/capstone.h>
#include <string>
#include "Platform/Linux/ELF.h"
#include <vector>

namespace TitaniumDecompiler {

struct Disassembly {
    uint64_t address;
    uint64_t offset;
    size_t size;
    std::string bytes;
    std::string mnemonic;
    std::string operators;
};

class Disassembler {
public:
    Disassembler();

};

}