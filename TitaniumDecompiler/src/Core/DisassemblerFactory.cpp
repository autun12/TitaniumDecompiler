#include "DisassemblerFactory.h"

#include <memory>

#include "Disassembler/IDisassembler.h"
#include "Disassembler/Providers/JVM/JVMDisassembler.h"
#include "FileFormats/JVM/JVMFormatParser.h"

namespace TitaniumDecompiler {

std::unique_ptr<IDisassembler> DisassemblerFactory::CreateForParser(
    const IFormatParser& parser) {
    if (parser.GetFormatName() == "JVM ClassFile") {
        const auto& jvmParser = static_cast<const JVMFormatParser&>(parser);
        return std::make_unique<JVMDisassembler>(
            jvmParser.GetClassFile().m_ConstantPool);
    }
    return nullptr;
}
}  // namespace TitaniumDecompiler
