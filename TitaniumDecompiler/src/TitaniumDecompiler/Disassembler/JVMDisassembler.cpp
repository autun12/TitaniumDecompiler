#include "JVMDisassembler.h"

namespace TitaniumDecompiler {
std::string JVMDisassembler::DisassemblyWriter(const std::filesystem::path& path) { return m_Disassembler.Disassemble(path); }

std::vector<std::string> JVMDisassembler::GetClassNames() { return m_Disassembler.GetClassNames(); }

std::vector<std::string> JVMDisassembler::GetFunctionNames() { return m_Disassembler.GetFunctionNames(); }

ClassesMap JVMDisassembler::GetClassesMap() { return m_Disassembler.GetClassesMap(); }

ClassesToFuntions JVMDisassembler::GetClassesToFunctions() { return m_Disassembler.GetClassesToFunctions(); }
}  // namespace TitaniumDecompiler
