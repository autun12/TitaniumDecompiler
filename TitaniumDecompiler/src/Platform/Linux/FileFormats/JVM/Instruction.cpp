#include "Instruction.h"

#include <TitaniumLogger/Logger/Log.h>
#include <stdio.h>

#include <cstdio>
#include <map>
#include <string>

#include "CodeConstants.h"
#include "ConstPoolInfo.h"
#include "ConstantPool.h"

namespace TitaniumDecompiler {
Instruction::Instruction() { }

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
    {OPCODE_JSR_W, "jsr_w"}
};

// std::string ReadInstruction(const std::vector<uint8_t>& bytes) {
//     std::string output = "";
//     std::vector<Insn> instructions;
//     Insn insn;
//     for(size_t pc = 0; pc < bytes.size(); ++pc) {
//         uint8_t opcode = bytes[pc];
//         auto it = opcodes.find(opcode);
//         if(it != opcodes.end()) {
//             output.append(std::to_string(pc) + ": " + it->second.c_str());
//             if(it->first == OPCODE_GETSTATIC ||
//                it->first == OPCODE_PUTSTATIC ||
//                it->first == OPCODE_GETFIELD ||
//                it->first == OPCODE_PUTFIELD ||
//                it->first == OPCODE_INVOKESPECIAL ||
//                it->first == OPCODE_INVOKEVIRTUAL ||
//                it->first == OPCODE_INVOKEINTERFACE ||
//                it->first == OPCODE_INVOKESTATIC ||
//                it->first == OPCODE_NEW
//             ) {
//                 insn.opcode = opcode;
//                 insn.opcodeName = it->first;
//                 insn.Op1.value = ReferencesInstruction(bytes, pc);
//                 output.append(" ");
//                 output.append(InstructionsWithReferences(bytes, pc));

//                 pc += 2;
//             } else if(
//                 it->first == OPCODE_IF_ACMPEQ ||
//                 it->first == OPCODE_IF_ACMPNE ||
//                 it->first == OPCODE_IF_ICMPEQ ||
//                 it->first == OPCODE_IF_ICMPGE ||
//                 it->first == OPCODE_IF_ICMPGT ||
//                 it->first == OPCODE_IF_ICMPLE ||
//                 it->first == OPCODE_IF_ICMPLT ||
//                 it->first == OPCODE_IF_ICMPNE ||
//                 it->first == OPCODE_IFEQ ||
//                 it->first == OPCODE_IFNE ||
//                 it->first == OPCODE_IFGT ||
//                 it->first == OPCODE_IFLE ||
//                 it->first == OPCODE_IFLT ||
//                 it->first == OPCODE_IFNE ||
//                 it->first == OPCODE_IFNONNULL ||
//                 it->first == OPCODE_IFNULL
//             ) {
//                 output.append(" " + ParseIfStatement(bytes, pc));
//                 pc += 2;
//             } else if(it->first == OPCODE_IINC) {
//                 output.append(ParseIncrement(bytes, pc));
//                 pc += 2;
//             } else if(it->first == OPCODE_LDC) {
//                 output.append(" \"");
//                 output.append(LoadConstFromPool(bytes, pc));
//                 output.append("\"");
//                 pc++;
//             } else if(it->first == OPCODE_BIPUSH) {
//                 output.append(" " + ParseBiPushOpCode(bytes, pc));
//                 pc++;
//             } else if(it->first == OPCODE_GOTO) {
//                 output.append(" ");
//                 output.append(ParseGoto(bytes, pc));
//                 pc += 2;
//             } else if(it->first == OPCODE_CHECKCAST) {
//                 output.append(" ");
//                 output.append(ParseCheckCast(bytes, pc));
//                 pc += 2;
//             } else if(it->first == OPCODE_RETURN || it->first == OPCODE_ARETURN) {
//                 output.append("\n");
//                 output.append(".end method\n");
//             } else if(it->first == OPCODE_MULTIANEWARRAY) {
//                 output.append(" ");
//                 output.append(ParseMultiNewArray(bytes, pc));
//                 pc += 3;
//             } else if(it->first == OPCODE_TABLESWITCH) {
//                 // Program Counter for tableswitch is 3 - (program counter mod 4) to get the correct default byte
//                 // There are a few other ways I have seen but this one is the most reliable for me to implement.
//                 pc += 3 - (pc % 4);

//                 int32_t defaultByte = ((bytes[pc + 1] << 24)
//                     | (bytes[pc + 2] << 16)
//                     | (bytes[pc + 3] << 8)
//                     | (bytes[pc + 4])
//                 ) + pc;

//                 int32_t lowByte = ((bytes[pc + 5] << 24)
//                     | (bytes[pc + 6] << 16)
//                     | (bytes[pc + 7] << 8)
//                     | (bytes[pc + 8]));

//                 int32_t highByte = ((bytes[pc + 9] << 24)
//                     | (bytes[pc + 10] << 16)
//                     | (bytes[pc + 11] << 8)
//                     | (bytes[pc + 12]));

//                 pc += 12;
//                 int32_t numOffsets = highByte - lowByte + 1;

//                 std::vector<int32_t> indices;
//                 for(int i = 0; i < numOffsets; i++) {
//                     int32_t count = ((bytes[pc + 1] << 24)
//                         | (bytes[pc + 2] << 16)
//                         | (bytes[pc + 3] << 8)
//                         | (bytes[pc + 4]));
//                     indices.push_back(count + numOffsets);
//                     pc += 4;
//                 }

//                 output.append(" " + std::to_string(lowByte) + " " + std::to_string(highByte));
//                 for(size_t i = 0; i < indices.size(); i++) {
//                     output.append("\n\t" + std::to_string(i + 1) + ": " + std::to_string(indices.at(i)));
//                 }
//                 output.append("\n\tdefault: " + std::to_string(defaultByte));
//             }

//             instructions.push_back(insn);

//             output.append("\n");

//         } else {
//             printf("Unknown opcode\n");
//         }
//     }
//     return output;
// }

InstrMap ReadInstructions(const std::vector<uint8_t>& bytes) {
    std::string output = "";
    InstrMap instructions;
    Insn insn;

    for (size_t pc = 0; pc < bytes.size(); ++pc) {
        size_t offset = pc;
        uint8_t opcode = bytes[pc];
        auto it = opcodes.find(opcode);
        if (it != opcodes.end()) {
            insn.opcode = it->first;
            insn.opcodeName = it->second;
            uint32_t refs, ref2f;
            if (it->first >= OPCODE_ILOAD_0 && it->first <= OPCODE_ALOAD_3) {
                refs = (it->first - OPCODE_ILOAD_0) % 4;
                ref2f = (it->first - OPCODE_ILOAD_0) / 4;
                ref2f = ref2f == ((OPCODE_LLOAD_0 - OPCODE_ILOAD_0) / 4) || ref2f == ((OPCODE_DLOAD_0 - OPCODE_ILOAD_0) / 4);
                insn.Op1.addr = refs;
                insn.Op1.offset = static_cast<uint16_t>(pc);
                insn.group = GROUP_GENERAL;
            } else if (it->first >= OPCODE_ISTORE_0 && it->first <= OPCODE_ASTORE_3) {
                refs = (it->first - OPCODE_ISTORE_0) % 4;
                ref2f = (it->first - OPCODE_ISTORE_0) / 4;
                ref2f = ref2f == ((OPCODE_LSTORE_0 - OPCODE_ISTORE_0) / 4) || ref2f == ((OPCODE_DLOAD_0 - OPCODE_ILOAD_0) / 4);
                insn.Op1.addr = refs;
                insn.Op1.offset = static_cast<uint16_t>(pc);
                insn.group = GROUP_GENERAL;
            } else if (it->first == OPCODE_GETSTATIC || it->first == OPCODE_PUTSTATIC || it->first == OPCODE_GETFIELD || it->first == OPCODE_PUTFIELD ||
                       it->first == OPCODE_INVOKESPECIAL || it->first == OPCODE_INVOKEVIRTUAL || it->first == OPCODE_INVOKESTATIC || it->first == OPCODE_NEW) {
                insn.Op1.value = ReferencesInstruction(bytes, pc);
                if (it->first >= OPCODE_GETSTATIC && it->first <= OPCODE_PUTFIELD) {
                    insn.group = GROUP_FIELDACCESS;
                } else if (it->first >= OPCODE_INVOKEVIRTUAL && it->first <= OPCODE_INVOKESTATIC) {
                    insn.group = GROUP_INVOCATION;
                }
                pc += 2;
            } else if (it->first == OPCODE_INVOKEDYNAMIC) {
                insn.Op1.value = ReferencesInstruction(bytes, pc);
                insn.group = GROUP_INVOCATION;
                pc += 4;
            } else if (it->first == OPCODE_INVOKEINTERFACE) {
                insn.Op1.value = ReferencesInstruction(bytes, pc);
                insn.group = GROUP_INVOCATION;
                pc += 4;
            } else if (it->first == OPCODE_IF_ACMPEQ || it->first == OPCODE_IF_ACMPNE || it->first == OPCODE_IF_ICMPEQ || it->first == OPCODE_IF_ICMPGE ||
                       it->first == OPCODE_IF_ICMPGT || it->first == OPCODE_IF_ICMPLE || it->first == OPCODE_IF_ICMPLT || it->first == OPCODE_IF_ICMPNE ||
                       it->first == OPCODE_IFEQ || it->first == OPCODE_IFNE || it->first == OPCODE_IFGT || it->first == OPCODE_IFLE ||
                       it->first == OPCODE_IFLT || it->first == OPCODE_IFNE || it->first == OPCODE_IFNONNULL || it->first == OPCODE_IFNULL) {
                insn.Op1.addr = IfStatement(bytes, pc);
                insn.group = GROUP_JUMP;
                pc += 2;
            } else if (it->first == OPCODE_IINC) {
                insn.Op2.value = IncrementInstruction(bytes, pc);
                insn.group = GROUP_GENERAL;
                pc += 2;
            } else if (it->first == OPCODE_LDC) {
                insn.Op1.value = LoadDataConstant(bytes, pc);
                insn.group = GROUP_GENERAL;
                pc++;
            } else if (it->first == OPCODE_BIPUSH) {
                insn.Op1.value = BiPushInstruction(bytes, pc);
                insn.group = GROUP_GENERAL;
                pc++;
            } else if (it->first == OPCODE_GOTO) {
                insn.Op1.addr = GotoInstruction(bytes, pc);
                insn.group = GROUP_JUMP;
                pc += 2;
            } else if (it->first == OPCODE_CHECKCAST) {
                insn.group = GROUP_GENERAL;
                pc += 2;
            } else if (it->first == OPCODE_RETURN || it->first == OPCODE_ARETURN || it->first == OPCODE_FRETURN || it->first == OPCODE_DRETURN ||
                       it->first == OPCODE_IRETURN || it->first == OPCODE_LRETURN) {
                insn.group = GROUP_RETURN;
            } else if (it->first == OPCODE_MULTIANEWARRAY) {
                insn.group = GROUP_GENERAL;
                pc += 3;
            } else if (it->first == OPCODE_ANEWARRAY) {
                insn.group = GROUP_GENERAL;
                insn.Op1.value = ReferencesInstruction(bytes, pc);
                pc += 2;
            } else if (it->first == OPCODE_NEWARRAY) {
                insn.group = GROUP_GENERAL;
                insn.Op1.value = LoadDataConstant(bytes, pc);
                pc++;
            } else if (it->first == OPCODE_TABLESWITCH) {
                // Program Counter for tableswitch is 3 - (program counter mod 4) to get the correct default byte
                // There are a few other ways I have seen but this one is the most reliable for me to implement.
                pc += 3 - (pc % 4);

                int32_t defaultByte = ((bytes[pc + 1] << 24) | (bytes[pc + 2] << 16) | (bytes[pc + 3] << 8) | (bytes[pc + 4])) + pc;

                int32_t lowByte = ((bytes[pc + 5] << 24) | (bytes[pc + 6] << 16) | (bytes[pc + 7] << 8) | (bytes[pc + 8]));

                int32_t highByte = ((bytes[pc + 9] << 24) | (bytes[pc + 10] << 16) | (bytes[pc + 11] << 8) | (bytes[pc + 12]));

                pc += 12;
                int32_t numOffsets = highByte - lowByte + 1;

                std::vector<int32_t> indices;
                for (int i = 0; i < numOffsets; i++) {
                    int32_t count = ((bytes[pc + 1] << 24) | (bytes[pc + 2] << 16) | (bytes[pc + 3] << 8) | (bytes[pc + 4]));
                    indices.push_back(count + numOffsets);
                    pc += 4;
                }

                output.append(" " + std::to_string(lowByte) + " " + std::to_string(highByte));
                for (size_t i = 0; i < indices.size(); i++) {
                    output.append("\n\t" + std::to_string(i + 1) + ": " + std::to_string(indices.at(i)));
                }
                output.append("\n\tdefault: " + std::to_string(defaultByte));
            } else if (it->first == OPCODE_ALOAD || it->first == OPCODE_ILOAD || it->first == OPCODE_DLOAD || it->first == OPCODE_FLOAD || it->first == OPCODE_LLOAD) {
                insn.group = GROUP_GENERAL;
                uint16_t top = 0;
                if (pc + 1 < bytes.size()) {
                    uint8_t index1 = bytes[pc + 1];
                    top = index1;
                }
                insn.Op1.addr = top;
                insn.Op1.offset = static_cast<uint16_t>(pc);
                // pc++;
            }

            instructions.insert({offset, insn});

            output.append("\n");

        } else {
            TD_DECOMP_WARN("Unknown opcode\n");
        }
    }

    return instructions;
}

uint64_t ReferencesInstruction(const std::vector<uint8_t>& bytes, uint32_t pc) {
    int idx = 0;
    if (pc + 2 < bytes.size()) {
        uint8_t index1 = bytes[pc + 1];
        uint8_t index2 = bytes[pc + 2];
        idx = (index1 << 8) | index2;
    }

    return idx;
}

uint8_t LoadDataConstant(const std::vector<uint8_t>& bytes, uint32_t pc) {
    uint8_t output = 0;

    if (pc + 1 < bytes.size()) {
        uint8_t index = bytes[pc + 1];
        output = index;
    } else {
        TD_DECOMP_ERROR("Not enough bytes for reference\n");
    }

    return output;
}

uint8_t LoadDataConstantWide(const std::vector<uint8_t>& bytes, uint32_t pc) {
    uint8_t output = 0;

    if (pc + 2 < bytes.size()) {
        uint8_t index1 = bytes[pc + 1];
        uint8_t index2 = bytes[pc + 2];
        output = (index1 << 8) | index2;
    } else {
        TD_DECOMP_ERROR("Not enough bytes for reference\n");
    }

    return output;
}

int16_t IfStatement(const std::vector<uint8_t>& bytes, uint32_t pc) {
    int16_t output = 0;
    if (pc + 2 < bytes.size()) {
        uint8_t index1 = bytes[pc + 1];
        uint8_t index2 = bytes[pc + 2];
        int16_t offset = (index1 << 8) | index2;
        output = offset + pc;
    } else {
        TD_DECOMP_ERROR("Not enough bytes for reference\n");
        output = 0;
    }
    return output;
}

uint16_t IncrementInstruction(const std::vector<uint8_t>& bytes, uint32_t pc) {
    uint16_t output = 0;

    if (pc + 2 < bytes.size()) {
        uint8_t index1 = bytes[pc + 1];
        uint8_t index2 = bytes[pc + 2];
        output = (index1 << 8) | index2;
    } else {
        TD_DECOMP_ERROR("Not enough bytes for reference\n");
        output = 0;
    }

    return output;
}

int16_t GotoInstruction(const std::vector<uint8_t>& bytes, uint32_t pc) {
    int16_t output = 0;

    if (pc + 2 < bytes.size()) {
        uint8_t index1 = bytes[pc + 1];
        uint8_t index2 = bytes[pc + 2];
        int16_t offset = (index1 << 8) | index2;
        int targetIdx = pc + offset;
        output = targetIdx;
    } else {
        TD_DECOMP_ERROR("Not enough bytes for reference\n");
        output = 0;
    }

    return output;
}

uint8_t BiPushInstruction(const std::vector<uint8_t>& bytes, uint32_t pc) {
    uint8_t output = 0;
    if (pc + 1 < bytes.size()) {
        uint8_t index1 = bytes[pc + 1];
        output = index1;
    } else {
        TD_DECOMP_ERROR("Not enough bytes for reference\n");
    }

    return output;
}

bool LoadIndex(Insn& insn, const std::vector<uint8_t>& bytes, uint32_t pc) {
    uint16_t top = 0;
    if (pc + 1 < bytes.size()) {
        uint8_t index1 = bytes[pc + 1];
        top = index1;
    }
    insn.Op1.addr = top;
    return true;
}
}  // namespace TitaniumDecompiler
