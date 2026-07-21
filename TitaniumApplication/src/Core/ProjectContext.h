#pragma once
#include <TitaniumDecompiler.h>

#include <filesystem>
#include <map>
#include <memory>
#include <string>

namespace TitaniumRenderer {

class ProjectContext {
public:
    ProjectContext() = default;

    bool LoadTargetFile(const std::filesystem::path& path);
    // Clean accessors for panels to query when notified
    const std::filesystem::path& GetFilePath() const { return m_CurrentPath; }
    const std::string& GetDisassemblyText() const {
        return m_DisassemblyOutput;
    }
    const TitaniumDecompiler::ClassesMap& GetClassesMap() const {
        return m_ClassesMap;
    }
    const auto& GetClassesToFunctions() const { return m_ClassesToFunctions; }

private:
    std::filesystem::path m_CurrentPath;
    std::string m_DisassemblyOutput;

    TitaniumDecompiler::ClassesMap m_ClassesMap;
    // Adjust type wrapper names to match your precise internal map types
    std::map<std::string, std::vector<TitaniumDecompiler::Function>>
        m_ClassesToFunctions;

    // Backend engines reside safely hidden away inside the data context!
    TitaniumDecompiler::JVMDisassembler m_JVMDisassembler;
    TitaniumDecompiler::Decompiler m_Decompiler;
};

}  // namespace TitaniumRenderer
