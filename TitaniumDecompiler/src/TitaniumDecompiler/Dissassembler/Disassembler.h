#include "tdpch.h"
#include <capstone/capstone.h>
#include <string>
#include "Platform/Linux/FileFormats/ELF.h"
#include <vector>

namespace TitaniumDecompiler {

class Disassembler {
public:
    void OpenFile();
private:
    ELF m_ElfFile;
};

}