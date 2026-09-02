#include "JVMFormatParser.h"

#include <exception>
#include <format>

#include "Core/Types.h"
#include "FileFormats/JVM/AttributeStructures.h"
#include "TitaniumLogger/Logger/Log.h"

namespace TitaniumDecompiler {
bool JVMFormatParser::Load(const std::filesystem::path& path) {
    try {
        m_ClassFile = m_Parser.OpenClassFile(path);
        m_IsLoaded = (m_ClassFile.m_Magic == 0xCAFEBABE);
        TD_DECOMP_INFO("JVM Class file fully read");
        return m_IsLoaded;
    } catch (const std::exception&) {
        m_IsLoaded = false;
        return false;
    }
}

std::vector<Section> JVMFormatParser::GetSections() const {
    std::vector<Section> sections;
    if (!m_IsLoaded) return sections;

    uint64_t currentAddress = 0x1000;

    Section cpSection;
    cpSection.Name = ".constants";
    cpSection.VirtualAddress = currentAddress;
    cpSection.Bytes = m_ClassFile.m_ConstantPool.ToRawBytes();
    cpSection.IsExecutable = false;
    sections.push_back(cpSection);
    currentAddress += cpSection.Bytes.size() + 0x10;
    for (const auto& method : m_ClassFile.m_Methods) {
        if (const auto* codeAttr = method.m_Attr.GetAttributePayload<Code>(
                "Code", m_ClassFile.m_ConstantPool)) {
            std::string methodName =
                m_ClassFile.m_ConstantPool.GetConstantUTF8(method.m_NameIndex);
            Section codeSection;
            codeSection.Name = std::format(".code.{}", methodName);
            codeSection.VirtualAddress = currentAddress;
            codeSection.Bytes = codeAttr->code;
            codeSection.IsExecutable = true;
            sections.push_back(codeSection);
            currentAddress += codeSection.Bytes.size() + 0x10;
        }
    }

    return sections;
}

std::vector<Symbol> JVMFormatParser::GetSymbols() const {
    std::vector<Symbol> symbols;
    if (!m_IsLoaded) return symbols;

    for (const auto& method : m_ClassFile.m_Methods) {
        Symbol sym;
        sym.IsFunction = true;
        std::string methName =
            m_ClassFile.m_ConstantPool.GetConstantUTF8(method.m_NameIndex);
        std::string descName =
            m_ClassFile.m_ConstantPool.GetConstantUTF8(method.m_DescIndex);
        sym.Name = methName + descName;
        symbols.push_back(sym);
    }

    return symbols;
}
}  // namespace TitaniumDecompiler
