#include "ProjectContext.h"

#include "Core/DisassemblerFactory.h"
#include "Core/DisassemblerFormatterFactory.h"
#include "Disassembler/IDisassembler.h"
#include "TitaniumLogger/Logger/Log.h"

namespace TitaniumRenderer {
bool ProjectContext::LoadTargetFile(const std::filesystem::path& path) {
    m_CurrentPath = path;

    m_Parser = TitaniumDecompiler::FormatFactory::CreateParserForFile(path);
    if (!m_Parser || !m_Parser->Load(path)) {
        TD_ERROR("Unsupported file format or failed to parse: {0}",
                 path.string());
        return false;
    }

    m_Disassembler =
        TitaniumDecompiler::DisassemblerFactory::CreateForParser(*m_Parser);
    if (!m_Disassembler) {
        TD_ERROR("Disassembler not found for format: {0}",
                 m_Parser->GetFormatName());
    }

    m_DisassemblerFormatter =
        TitaniumDecompiler::DisassemblerFormatterFactory::CreateForParser(
            *m_Parser);
    if (!m_DisassemblerFormatter) {
        TD_ERROR("Disassembler Formatter not found for format: {0}",
                 m_Parser->GetFormatName());
    }

    for (const auto& section : m_Parser->GetSections()) {
        if (section.IsExecutable) {
            std::vector<TitaniumDecompiler::DecodedInsn> instructions =
                m_Disassembler->DisassembleBlock(section.VirtualAddress,
                                                 section.Bytes);
            std::string cleanName = section.Name;

            if (cleanName.rfind(".code.", 0) == 0) {
                cleanName = cleanName.substr(6);
            }

            m_Methods.push_back({.methodName = cleanName,
                                 .instructions = std::move(instructions)});
        }
    }

    // 2. Cache structural data records safely
    // m_ClassesMap = m_JVMDisassembler.GetClassesMap();
    // m_ClassesToFunctions = m_JVMDisassembler.GetClassesToFunctions();

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

std::string ProjectContext::GetDisassemblyText() const {
    std::string fullText;
    for (const auto& block : m_Methods) {
        fullText += "; -- " + block.methodName + " ---\n";
        fullText += m_DisassemblerFormatter->FormatBlock(block.instructions);
    }
    return fullText;
}
}  // namespace TitaniumRenderer
