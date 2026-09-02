#pragma once
#include <TitaniumDecompiler.h>

#include <filesystem>
#include <memory>
#include <string>

#include "Core/IFormatParser.h"
#include "Disassembler/IDisassembler.h"
#include "Disassembler/IDisassemblerFormatter.h"

namespace TitaniumRenderer {

struct MethodCode {
    std::string methodName;
    std::vector<TitaniumDecompiler::DecodedInsn> instructions;
};

class ProjectContext {
public:
    ProjectContext() = default;

    bool LoadTargetFile(const std::filesystem::path& path);

    // Clean accessors for panels to query when notified
    const std::filesystem::path& GetFilePath() const { return m_CurrentPath; }
    std::string GetDisassemblyText() const;

private:
    std::filesystem::path m_CurrentPath;
    std::unique_ptr<TitaniumDecompiler::IFormatParser> m_Parser;
    std::unique_ptr<TitaniumDecompiler::IDisassembler> m_Disassembler;
    std::unique_ptr<TitaniumDecompiler::IDisassemblyFormatter>
        m_DisassemblerFormatter;
    std::vector<MethodCode> m_Methods;
};

}  // namespace TitaniumRenderer
