#include "JVMDisassembler.h"

#include <map>

#include "../../../FileFormats/JVM/CodeConstants.h"
namespace TitaniumDecompiler {

const std::map<uint8_t, std::string> opcodes = {
    {OPCODE_NOP, "nop"},
    {OPCODE_ACONST_NULL, "aconst_null"},
    {OPCODE_ICONST_M1, "iconst_m1"},
    {OPCODE_ICONST_0, "iconst_0"},
    {OPCODE_ICONST_1, "iconst_1"},
    {OPCODE_ICONST_2, "iconst_2"},
    {OPCODE_ICONST_3, "iconst_3"},
    {OPCODE_ICONST_4, "iconst_4"},
    {OPCODE_ICONST_5, "iconst_5"},
    {OPCODE_LCONST_0, "lconst_0"},
    {OPCODE_LCONST_1, "lconst_1"},
    {OPCODE_FCONST_0, "fconst_0"},
    {OPCODE_FCONST_1, "fconst_1"},
    {OPCODE_FCONST_2, "fconst_2"},
    {OPCODE_DCONST_0, "dconst_0"},
    {OPCODE_DCONST_1, "dconst_1"},
    {OPCODE_BIPUSH, "bipush"},
    {OPCODE_SIPUSH, "sipush"},
    {OPCODE_LDC, "ldc"},
    {OPCODE_LDC_W, "ldc_w"},
    {OPCODE_LDC2_W, "ldc2_w"},
    {OPCODE_ILOAD, "iload"},
    {OPCODE_LLOAD, "lload"},
    {OPCODE_FLOAD, "fload"},
    {OPCODE_DLOAD, "dload"},
    {OPCODE_ALOAD, "aload"},
    {OPCODE_ILOAD_0, "iload_0"},
    {OPCODE_ILOAD_1, "iload_1"},
    {OPCODE_ILOAD_2, "iload_2"},
    {OPCODE_ILOAD_3, "iload_3"},
    {OPCODE_LLOAD_0, "lload_0"},
    {OPCODE_LLOAD_1, "lload_1"},
    {OPCODE_LLOAD_2, "lload_2"},
    {OPCODE_LLOAD_3, "lload_3"},
    {OPCODE_FLOAD_0, "fload_0"},
    {OPCODE_FLOAD_1, "fload_1"},
    {OPCODE_FLOAD_2, "fload_2"},
    {OPCODE_FLOAD_3, "fload_3"},
    {OPCODE_DLOAD_0, "dload_0"},
    {OPCODE_DLOAD_1, "dload_1"},
    {OPCODE_DLOAD_2, "dload_2"},
    {OPCODE_DLOAD_3, "dload_3"},
    {OPCODE_ALOAD_0, "aload_0"},
    {OPCODE_ALOAD_1, "aload_1"},
    {OPCODE_ALOAD_2, "aload_2"},
    {OPCODE_ALOAD_3, "aload_3"},
    {OPCODE_IALOAD, "iaload"},
    {OPCODE_LALOAD, "laload"},
    {OPCODE_FALOAD, "faload"},
    {OPCODE_DALOAD, "daload"},
    {OPCODE_AALOAD, "aaload"},
    {OPCODE_BALOAD, "baload"},
    {OPCODE_CALOAD, "caload"},
    {OPCODE_SALOAD, "saload"},
    {OPCODE_ISTORE, "istore"},
    {OPCODE_LSTORE, "lstore"},
    {OPCODE_FSTORE, "fstore"},
    {OPCODE_DSTORE, "dstore"},
    {OPCODE_ASTORE, "astore"},
    {OPCODE_ISTORE_0, "istore_0"},
    {OPCODE_ISTORE_1, "istore_1"},
    {OPCODE_ISTORE_2, "istore_2"},
    {OPCODE_ISTORE_3, "istore_3"},
    {OPCODE_LSTORE_0, "lstore_0"},
    {OPCODE_LSTORE_1, "lstore_1"},
    {OPCODE_LSTORE_2, "lstore_2"},
    {OPCODE_LSTORE_3, "lstore_3"},
    {OPCODE_FSTORE_0, "fstore_0"},
    {OPCODE_FSTORE_1, "fstore_1"},
    {OPCODE_FSTORE_2, "fstore_2"},
    {OPCODE_FSTORE_3, "fstore_3"},
    {OPCODE_DSTORE_0, "dstore_0"},
    {OPCODE_DSTORE_1, "dstore_1"},
    {OPCODE_DSTORE_2, "dstore_2"},
    {OPCODE_DSTORE_3, "dstore_3"},
    {OPCODE_ASTORE_0, "astore_0"},
    {OPCODE_ASTORE_1, "astore_1"},
    {OPCODE_ASTORE_2, "astore_2"},
    {OPCODE_ASTORE_3, "astore_3"},
    {OPCODE_IASTORE, "iastore"},
    {OPCODE_LASTORE, "lastore"},
    {OPCODE_FASTORE, "fastore"},
    {OPCODE_DASTORE, "dastore"},
    {OPCODE_AASTORE, "aastore"},
    {OPCODE_BASTORE, "bastore"},
    {OPCODE_CASTORE, "castore"},
    {OPCODE_SASTORE, "sastore"},
    {OPCODE_POP, "pop"},
    {OPCODE_POP2, "pop2"},
    {OPCODE_DUP, "dup"},
    {OPCODE_DUP_X1, "dup_x1"},
    {OPCODE_DUP_X2, "dup_x2"},
    {OPCODE_DUP2, "dup2"},
    {OPCODE_DUP2_X1, "dup2_x1"},
    {OPCODE_DUP2_X2, "dup2_x2"},
    {OPCODE_SWAP, "swap"},
    {OPCODE_IADD, "iadd"},
    {OPCODE_LADD, "ladd"},
    {OPCODE_FADD, "fadd"},
    {OPCODE_DADD, "dadd"},
    {OPCODE_ISUB, "isub"},
    {OPCODE_LSUB, "lsub"},
    {OPCODE_FSUB, "fsub"},
    {OPCODE_DSUB, "dsub"},
    {OPCODE_IMUL, "imul"},
    {OPCODE_LMUL, "lmul"},
    {OPCODE_FMUL, "fmul"},
    {OPCODE_DMUL, "dmul"},
    {OPCODE_IDIV, "idiv"},
    {OPCODE_LDIV, "ldiv"},
    {OPCODE_FDIV, "fdiv"},
    {OPCODE_DDIV, "ddiv"},
    {OPCODE_IREM, "irem"},
    {OPCODE_LREM, "lrem"},
    {OPCODE_FREM, "frem"},
    {OPCODE_DREM, "drem"},
    {OPCODE_INEG, "ineg"},
    {OPCODE_LNEG, "lneg"},
    {OPCODE_FNEG, "fneg"},
    {OPCODE_DNEG, "dneg"},
    {OPCODE_ISHL, "ishl"},
    {OPCODE_LSHL, "lshl"},
    {OPCODE_ISHR, "ishr"},
    {OPCODE_LSHR, "lshr"},
    {OPCODE_IUSHR, "iushr"},
    {OPCODE_LUSHR, "lushr"},
    {OPCODE_IAND, "iand"},
    {OPCODE_LAND, "land"},
    {OPCODE_IOR, "ior"},
    {OPCODE_LOR, "lor"},
    {OPCODE_IXOR, "ixor"},
    {OPCODE_LXOR, "lxor"},
    {OPCODE_IINC, "iinc"},
    {OPCODE_I2L, "i2l"},
    {OPCODE_I2F, "i2f"},
    {OPCODE_I2D, "i2d"},
    {OPCODE_L2I, "l2i"},
    {OPCODE_L2F, "l2f"},
    {OPCODE_L2D, "l2d"},
    {OPCODE_F2I, "f2i"},
    {OPCODE_F2L, "f2l"},
    {OPCODE_F2D, "f2d"},
    {OPCODE_D2I, "d2i"},
    {OPCODE_D2L, "d2l"},
    {OPCODE_D2F, "d2f"},
    {OPCODE_I2B, "int2byte"},
    {OPCODE_I2C, "int2char"},
    {OPCODE_I2S, "int2short"},
    {OPCODE_LCMP, "lcmp"},
    {OPCODE_FCMPL, "fcmpl"},
    {OPCODE_FCMPG, "fcmpg"},
    {OPCODE_DCMPL, "dcmpl"},
    {OPCODE_DCMPG, "dcmpg"},
    {OPCODE_IFEQ, "ifeq"},
    {OPCODE_IFNE, "ifne"},
    {OPCODE_IFLT, "iflt"},
    {OPCODE_IFGE, "ifge"},
    {OPCODE_IFGT, "ifgt"},
    {OPCODE_IFLE, "ifle"},
    {OPCODE_IF_ICMPEQ, "if_icmpeq"},
    {OPCODE_IF_ICMPNE, "if_icmpne"},
    {OPCODE_IF_ICMPLT, "if_icmplt"},
    {OPCODE_IF_ICMPGE, "if_icmpge"},
    {OPCODE_IF_ICMPGT, "if_icmpgt"},
    {OPCODE_IF_ICMPLE, "if_icmple"},
    {OPCODE_IF_ACMPEQ, "if_acmpeq"},
    {OPCODE_IF_ACMPNE, "if_acmpne"},
    {OPCODE_GOTO, "goto"},
    {OPCODE_JSR, "jsr"},
    {OPCODE_RET, "ret"},
    {OPCODE_TABLESWITCH, "tableswitch"},
    {OPCODE_LOOKUPSWITCH, "lookupswitch"},
    {OPCODE_IRETURN, "ireturn"},
    {OPCODE_LRETURN, "lreturn"},
    {OPCODE_FRETURN, "freturn"},
    {OPCODE_DRETURN, "dreturn"},
    {OPCODE_ARETURN, "areturn"},
    {OPCODE_RETURN, "return"},
    {OPCODE_GETSTATIC, "getstatic"},
    {OPCODE_PUTSTATIC, "putstatic"},
    {OPCODE_GETFIELD, "getfield"},
    {OPCODE_PUTFIELD, "putfield"},
    {OPCODE_INVOKEVIRTUAL, "invokevirtual"},
    {OPCODE_INVOKESPECIAL, "invokespecial"},
    {OPCODE_INVOKESTATIC, "invokestatic"},
    {OPCODE_INVOKEINTERFACE, "invokeinterface"},
    {OPCODE_INVOKEDYNAMIC, "invokedynamic"},
    {OPCODE_NEW, "new"},
    {OPCODE_NEWARRAY, "newarray"},
    {OPCODE_ANEWARRAY, "anewarray"},
    {OPCODE_ARRAYLENGTH, "arraylength"},
    {OPCODE_ATHROW, "athrow"},
    {OPCODE_CHECKCAST, "checkcast"},
    {OPCODE_INSTANCEOF, "instanceof"},
    {OPCODE_MONITORENTER, "monitorenter"},
    {OPCODE_MONITOREXIT, "monitorexit"},
    {OPCODE_WIDE, "wide"},
    {OPCODE_MULTIANEWARRAY, "multianewarray"},
    {OPCODE_IFNULL, "ifnull"},
    {OPCODE_IFNONNULL, "ifnonnull"},
    {OPCODE_GOTO_W, "goto_w"},
    {OPCODE_JSR_W, "jsr_w"}};

JVMDisassembler::JVMDisassembler(const ConstantPool& constPool)
    : m_ConstantPool(constPool) {}

bool JVMDisassembler::DecodeInstruction(uint64_t address,
                                        std::span<const uint8_t> bytes,
                                        DecodedInsn& outInsn) {
    if (bytes.empty()) return false;

    uint8_t opcode = bytes[0];
    outInsn.address = address;
    outInsn.length = 1;
    outInsn.operands.clear();

    switch (opcode) {
        case 0x01:
            outInsn.mnemonic = "aconst_null";
            outInsn.length = 1;
            return true;

        // iconst_m1 (0x02) to iconst_5 (0x08)
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
        case 0x08: {
            static constexpr std::string_view iconstNames[] = {
                "iconst_m1", "iconst_0", "iconst_1", "iconst_2",
                "iconst_3",  "iconst_4", "iconst_5"};
            outInsn.mnemonic = iconstNames[opcode - 0x02];
            outInsn.length = 1;
            return true;
        }

        // lconst_0 (0x09), lconst_1 (0x0a)
        case 0x09:
        case 0x0a:
            outInsn.mnemonic = (opcode == 0x09) ? "lconst_0" : "lconst_1";
            outInsn.length = 1;
            return true;

        // fconst_0 to fconst_2 (0x0b to 0x0d)
        case 0x0b:
        case 0x0c:
        case 0x0d: {
            static constexpr std::string_view fconstNames[] = {
                "fconst_0", "fconst_1", "fconst_2"};
            outInsn.mnemonic = fconstNames[opcode - 0x0b];
            outInsn.length = 1;
            return true;
        }

        // dconst_0, dconst_1 (0x0e, 0x0f)
        case 0x0e:
        case 0x0f:
            outInsn.mnemonic = (opcode == 0x0e) ? "dconst_0" : "dconst_1";
            outInsn.length = 1;
            return true;
            // bipush (0x10) - 2 bytes
        case 0x10: {
            if (bytes.size() < 2) return false;
            outInsn.mnemonic = "bipush";
            outInsn.length = 2;
            int8_t val = static_cast<int8_t>(bytes[1]);
            outInsn.operands.push_back(
                {OperandType::Immediate, static_cast<uint64_t>(val)});
            return true;
        }

        // sipush (0x11) - 3 bytes
        case 0x11: {
            if (bytes.size() < 3) return false;
            outInsn.mnemonic = "sipush";
            outInsn.length = 3;
            int16_t val = static_cast<int16_t>((bytes[1] << 8) | bytes[2]);
            outInsn.operands.push_back(
                {OperandType::Immediate, static_cast<uint64_t>(val)});
            return true;
        }
            // ldc (0x12) - 2 bytes (1 byte index)
        case 0x12: {
            if (bytes.size() < 2) return false;
            outInsn.mnemonic = "ldc";
            outInsn.length = 2;
            outInsn.operands.push_back({OperandType::Immediate, bytes[1]});
            return true;
        }

        // ldc_w (0x13), ldc2_w (0x14) - 3 bytes (2 byte index)
        case 0x13:
        case 0x14: {
            if (bytes.size() < 3) return false;
            outInsn.mnemonic = (opcode == 0x13) ? "ldc_w" : "ldc2_w";
            outInsn.length = 3;
            uint16_t cpIndex =
                static_cast<uint16_t>((bytes[1] << 8) | bytes[2]);
            outInsn.operands.push_back({OperandType::Immediate, cpIndex});
            return true;
        }
        case 0x15:
        case 0x16:
        case 0x17:
        case 0x18:
        case 0x19: {
            if (bytes.size() < 2) return false;
            if (opcode == 0x15)
                outInsn.mnemonic = "iload";
            else if (opcode == 0x16)
                outInsn.mnemonic = "lload";
            else if (opcode == 0x17)
                outInsn.mnemonic = "fload";
            else if (opcode == 0x18)
                outInsn.mnemonic = "dload";
            else
                outInsn.mnemonic = "aload";
            outInsn.length = 2;
            outInsn.operands = {{OperandType::Immediate, bytes[1]}};
            return true;
        }
        case 0x1a:
        case 0x1b:
        case 0x1c:
        case 0x1d: {
            outInsn.mnemonic = "iload";
            uint32_t varIndex = opcode - 0x1a;  // 0 to 3
            outInsn.operands.push_back({OperandType::Register, varIndex});
            return true;
        }

        // Compressed lload_0 to lload_3 (0x1e to 0x21)
        case 0x1e:
        case 0x1f:
        case 0x20:
        case 0x21: {
            outInsn.mnemonic = "lload";
            uint32_t varIndex = opcode - 0x1e;
            outInsn.operands.push_back({OperandType::Register, varIndex});
            return true;
        }

        // Compressed fload_0 to fload_3 (0x22 to 0x25)
        case 0x22:
        case 0x23:
        case 0x24:
        case 0x25: {
            outInsn.mnemonic = "fload";
            uint32_t varIndex = opcode - 0x22;
            outInsn.operands.push_back({OperandType::Register, varIndex});
            return true;
        }

        // Compressed dload_0 to dload_3 (0x26 to 0x29)
        case 0x26:
        case 0x27:
        case 0x28:
        case 0x29: {
            outInsn.mnemonic = "dload";
            uint32_t varIndex = opcode - 0x26;
            outInsn.operands.push_back({OperandType::Register, varIndex});
            return true;
        }

        // Compressed aload_0 to aload_3 (0x2a to 0x2d)
        case 0x2a:
        case 0x2b:
        case 0x2c:
        case 0x2d: {
            outInsn.mnemonic = "aload";
            uint32_t varIndex = opcode - 0x2a;
            outInsn.operands.push_back({OperandType::Register, varIndex});
            return true;
        }
            // astore (0x3a) - 2 bytes (1 byte opcode + 1 byte local variable
            // index)
        case 0x3a: {
            if (bytes.size() < 2) return false;
            outInsn.mnemonic = "astore";
            outInsn.length = 2;
            outInsn.operands.push_back({OperandType::Immediate, bytes[1]});
            return true;
        }

        // Compressed astore_0 to astore_3 (0x4b to 0x4e) - 1 byte
        case 0x4b:
        case 0x4c:
        case 0x4d:
        case 0x4e: {
            outInsn.mnemonic = "astore";
            uint32_t varIndex = opcode - 0x4b;  // 0 to 3
            outInsn.length = 1;
            outInsn.operands.push_back({OperandType::Register, varIndex});
            return true;
        }

        // Store equivalents (istore_0 to istore_3: 0x3b to 0x3e)
        case 0x3b:
        case 0x3c:
        case 0x3d:
        case 0x3e: {
            outInsn.mnemonic = "istore";
            uint32_t varIndex = opcode - 0x3b;
            outInsn.operands.push_back({OperandType::Register, varIndex});
            return true;
        }
        case 0x84: {
            if (bytes.size() < 3) return false;

            outInsn.mnemonic = "iinc";
            outInsn.length = 3;

            uint8_t varIndex = bytes[1];
            int8_t constValue =
                static_cast<int8_t>(bytes[2]);  // Can be negative (decrement)

            outInsn.operands.push_back({OperandType::Register, varIndex});
            outInsn.operands.push_back(
                {OperandType::Immediate,
                 static_cast<uint64_t>(static_cast<int64_t>(constValue))});
            return true;
        }
        case 0x99:  // ifeq
        case 0x9a:  // ifne
        case 0x9b:  // iflt
        case 0x9c:  // ifge
        case 0x9d:  // ifgt
        case 0x9e:  // ifle
        case 0x9f:  // if_icmpeq
        case 0xa7:  // goto
            // ... (other if_* and goto/jsr opcodes)
            {
                if (bytes.size() < 3) return false;

                // Extract signed 16-bit offset
                int16_t branchOffset =
                    static_cast<int16_t>((bytes[1] << 8) | bytes[2]);
                uint64_t targetAddress = address + branchOffset;

                // Assign mnemonic based on opcode
                if (opcode == 0x99)
                    outInsn.mnemonic = "ifeq";
                else if (opcode == 0x9a)
                    outInsn.mnemonic = "ifne";
                else if (opcode == 0x9b)
                    outInsn.mnemonic = "iflt";
                else if (opcode == 0x9c)
                    outInsn.mnemonic = "ifge";
                else if (opcode == 0x9d)
                    outInsn.mnemonic = "ifgt";
                else if (opcode == 0x9e)
                    outInsn.mnemonic = "ifle";
                else if (opcode == 0x9f)
                    outInsn.mnemonic = "if_icmpeq";
                else if (opcode == 0xa0)
                    outInsn.mnemonic = "if_icmpne";
                else if (opcode == 0xa1)
                    outInsn.mnemonic = "if_icmplt";
                else if (opcode == 0xa2)
                    outInsn.mnemonic = "if_icmpge";
                else if (opcode == 0xa3)
                    outInsn.mnemonic = "if_icmpgt";
                else if (opcode == 0xa4)
                    outInsn.mnemonic = "if_icmple";
                else if (opcode == 0xa5)
                    outInsn.mnemonic = "if_acmpeq";
                else if (opcode == 0xa6)
                    outInsn.mnemonic = "if_acmpne";
                else if (opcode == 0xa7)
                    outInsn.mnemonic = "goto";

                outInsn.length = 3;
                outInsn.isBranch = true;

                // Store the target address (or raw offset) as a numeric operand
                outInsn.operands.push_back(
                    {OperandType::BranchTarget, targetAddress});
                return true;
            }
        case 0xac:  // ireturn
        case 0xad:  // lreturn
        case 0xae:  // freturn
        case 0xaf:  // dreturn
        case 0xb0:  // areturn
        case 0xb1:  // return
        {
            if (opcode == 0xac)
                outInsn.mnemonic = "ireturn";
            else if (opcode == 0xad)
                outInsn.mnemonic = "lreturn";
            else if (opcode == 0xae)
                outInsn.mnemonic = "freturn";
            else if (opcode == 0xaf)
                outInsn.mnemonic = "dreturn";
            else if (opcode == 0xb0)
                outInsn.mnemonic = "areturn";
            else
                outInsn.mnemonic = "return";

            outInsn.length = 1;
            outInsn.isReturn = true;
            return true;
        }
        case 0xb2:  // getstatic
        case 0xb3:  // putstatic
        case 0xb4:  // getfield
        case 0xb5:  // putfield
        case 0xb6:  // invokevirtual
        case 0xb7:  // invokespecial
        case 0xb8:  // invokestatic
        case 0xbb:  // new
        {
            if (bytes.size() < 3) return false;

            uint16_t cpIndex =
                static_cast<uint16_t>((bytes[1] << 8) | bytes[2]);

            if (opcode == 0xb2)
                outInsn.mnemonic = "getstatic";
            else if (opcode == 0xb3)
                outInsn.mnemonic = "putstatic";
            else if (opcode == 0xb4)
                outInsn.mnemonic = "getfield";
            else if (opcode == 0xb5)
                outInsn.mnemonic = "putfield";
            else if (opcode == 0xb6) {
                outInsn.mnemonic = "invokevirtual";
                outInsn.isCall = true;
            } else if (opcode == 0xb7) {
                outInsn.mnemonic = "invokespecial";
                outInsn.isCall = true;
            } else if (opcode == 0xb8) {
                outInsn.mnemonic = "invokestatic";
                outInsn.isCall = true;
            } else if (opcode == 0xbb)
                outInsn.mnemonic = "new";

            outInsn.length = 3;
            outInsn.operands.push_back({OperandType::Immediate, cpIndex});
            return true;
        }
        case 0xb9:
        case 0xba: {
            if (bytes.size() < 5) return false;

            uint16_t cpIndex =
                static_cast<uint16_t>((bytes[1] << 8) | bytes[2]);
            outInsn.mnemonic =
                (opcode == 0xb9) ? "invokeinterface" : "invokedynamic";
            outInsn.length = 5;
            outInsn.isCall = true;
            outInsn.operands.push_back({OperandType::Immediate, cpIndex});
            return true;
        }
        case 0xbd: {
            if (bytes.size() < 3) return false;

            uint16_t cpIndex =
                static_cast<uint16_t>((bytes[1] << 8) | bytes[2]);
            outInsn.mnemonic = "anewarray";
            outInsn.length = 3;
            outInsn.operands.push_back({OperandType::Immediate, cpIndex});
            return true;
        }
        default:
            return false;
    }
}

std::vector<DecodedInsn> JVMDisassembler::DisassembleBlock(
    uint64_t startAddress, std::span<const uint8_t> bytes) {
    std::vector<DecodedInsn> block;
    uint32_t offset = 0;

    while (offset < bytes.size()) {
        std::span<const uint8_t> remainingBytes = bytes.subspan(offset);

        DecodedInsn insn;
        uint64_t currentAddr = startAddress + offset;
        if (!DecodeInstruction(currentAddr, remainingBytes, insn)) {
            break;
        }

        offset += insn.length;
        block.push_back(insn);
    }

    return block;
}

std::string_view JVMDisassembler::GetArchitectureName() const { return "JVM"; }
}  // namespace TitaniumDecompiler
