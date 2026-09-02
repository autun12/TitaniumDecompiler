#include "JVMFormatter.h"

#include <sstream>

namespace TitaniumDecompiler {
JVMFormatter::JVMFormatter(const ConstantPool& constPool)
    : m_ConstantPool(constPool) {}

std::string JVMFormatter::FormatInstruction(const DecodedInsn& insn) {
    std::stringstream ss;

    ss << insn.mnemonic;

    for (size_t i = 0; i < insn.operands.size(); ++i) {
        const auto& op = insn.operands[i];
        if (op.type == OperandType::Register) {
            ss << "_" << op.rawValue;
        } else {
            ss << (i == 0 ? " " : ", ");

            switch (op.type) {
                case OperandType::Immediate: {
                    uint64_t val = op.rawValue;
                    if (IsConstantPoolInstruction(insn.mnemonic)) {
                        std::string resolved = ResolveConstantPoolEntry(val);
                        if (!resolved.empty()) {
                            ss << "#" << val << " ; " << resolved;
                            break;
                        }
                    }
                    ss << op.rawValue;
                    break;
                }
                case OperandType::BranchTarget:
                    ss << "pc +" << static_cast<int64_t>(op.rawValue);
                    break;
                case OperandType::Memory:
                    ss << "[" << op.rawValue << "]";
                    break;
                case OperandType::Indirect:
                    ss << "(#" << op.rawValue << ")";
                    break;
                default:
                    break;
            }
        }
    }
    return ss.str();
}
std::string JVMFormatter::FormatBlock(const std::vector<DecodedInsn>& blocks) {
    std::string result;

    for (const auto& insn : blocks) {
        result += FormatInstruction(insn) + "\n";
    }

    return result;
}

bool JVMFormatter::IsConstantPoolInstruction(std::string_view mnemonic) const {
    return mnemonic == "ldc" || mnemonic == "ldc_w" || mnemonic == "ldc2_w" ||
           mnemonic == "getstatic" || mnemonic == "putstatic" ||
           mnemonic == "getfield" || mnemonic == "putfield" ||
           mnemonic == "invokevirtual" || mnemonic == "invokespecial" ||
           mnemonic == "invokestatic" || mnemonic == "invokeinterface" ||
           mnemonic == "invokedynamic" || mnemonic == "new" ||
           mnemonic == "anewarray" || mnemonic == "checkcast" ||
           mnemonic == "instanceof";
}

std::string JVMFormatter::ResolveConstantPoolEntry(uint16_t idx) const {
    const auto* cpInfo = m_ConstantPool.GetConstant(idx);
    if (!cpInfo) return "";

    // Pattern-match directly on the variant to extract clean symbolic info
    return std::visit(
        [this](const auto& info) -> std::string {
            using T = std::decay_t<decltype(info)>;

            if constexpr (std::is_same_v<T, UTF8Info>) {
                return "\"" +
                       std::string(info.bytes.begin(), info.bytes.end()) + "\"";
            } else if constexpr (std::is_same_v<T, StringInfo>) {
                return m_ConstantPool.GetConstantUTF8(info.stringIndex);
            } else if constexpr (std::is_same_v<T, ClassInfo>) {
                return m_ConstantPool.GetConstantUTF8(info.nameIndex);
            } else if constexpr (std::is_same_v<T, FieldRefInfo> ||
                                 std::is_same_v<T, MethodRefInfo> ||
                                 std::is_same_v<T, InterfaceMethodRef>) {
                std::pair<std::string, std::string> nameAndTypePair =
                    m_ConstantPool.GetNameAndType(info.nameAndTypeIndex);
                std::string nameAndType =
                    nameAndTypePair.first + nameAndTypePair.second;

                return m_ConstantPool.GetConstantClass(info.classIndex) +
                       nameAndType;
            } else if constexpr (std::is_same_v<T, NameAndTypeInfo>) {
                return "name #" + std::to_string(info.nameIndex) + ", desc #" +
                       std::to_string(info.descriptorIndex);
            } else if constexpr (std::is_same_v<T, IntegerInfo>) {
                return std::to_string(info.bytes);
            } else if constexpr (std::is_same_v<T, FloatInfo>) {
                return std::to_string(info.bytes) + "f";
            } else {
                return "";
            }
        },
        cpInfo->Info);
}
}  // namespace TitaniumDecompiler
