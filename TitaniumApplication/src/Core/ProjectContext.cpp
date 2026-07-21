
#include "ProjectContext.h"
namespace TitaniumRenderer {
bool ProjectContext::LoadTargetFile(const std::filesystem::path& path) {
    m_CurrentPath = path;

    // 1. Execute disassembler work inside the backend context
    m_DisassemblyOutput = m_JVMDisassembler.DisassemblyWriter(path);
    if (m_DisassemblyOutput.empty()) {
        return false;
    }

    // 2. Cache structural data records safely
    m_ClassesMap = m_JVMDisassembler.GetClassesMap();
    m_ClassesToFunctions = m_JVMDisassembler.GetClassesToFunctions();

    // for (auto& classEntry : m_JVMDisassembler.GetClassesToFunctions()) {
    // TD_INFO(classEntry.first);
    // m_Decompiler.ProcessFunctions(classEntry.second);
    // }

    // for (auto& func : m_Disassembler.GetFunctions()) {
    //     for (auto& block : func.GetFunctionCFG()->GetBlocks()) {
    //         func.GetFunctionCFG()->PrintBlock(block);
    //     }
    // }
    return true;
}
}  // namespace TitaniumRenderer
