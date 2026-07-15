#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "Disassembler.h"

namespace TitaniumDecompiler {
struct JVMDisassembler {
public:
    JVMDisassembler() = default;
    ~JVMDisassembler() = default;
    std::string DisassemblyWriter(const std::filesystem::path& path);
    std::vector<std::string> GetClassNames();
    std::vector<std::string> GetFunctionNames();
    ClassesMap GetClassesMap();
    ClassesToFuntions GetClassesToFunctions();
    Disassembler GetDisassembler() { return m_Disassembler; }

private:
    Disassembler m_Disassembler;
};
}  // namespace TitaniumDecompiler
