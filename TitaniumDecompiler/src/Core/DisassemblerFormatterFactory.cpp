#include "DisassemblerFormatterFactory.h"

#include <memory>

#include "FileFormats/JVM/JVMFormatParser.h"
#include "Formatting/Disassembly/JVMFormatter.h"

namespace TitaniumDecompiler {

std::unique_ptr<IDisassemblyFormatter>
DisassemblerFormatterFactory::CreateForParser(const IFormatParser& parser) {
    if (parser.GetFormatName() == "JVM ClassFile") {
        const auto& jvmParser = static_cast<const JVMFormatParser&>(parser);
        return std::make_unique<JVMFormatter>(
            jvmParser.GetClassFile().m_ConstantPool);
    }
    return nullptr;
}
}  // namespace TitaniumDecompiler
