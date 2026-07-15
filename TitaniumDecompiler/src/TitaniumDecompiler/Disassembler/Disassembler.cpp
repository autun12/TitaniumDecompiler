#include "Disassembler.h"

#include <algorithm>
#include <memory>
#include <string>
// #include <unordered_set>
#include <iostream>
#include <vector>

#include "Platform/Linux/FileFormats/JVM/AccessFlags.h"
#include "Platform/Linux/FileFormats/JVM/CodeConstants.h"
#include "Platform/Linux/FileFormats/JVM/ConstantPool.h"

namespace TitaniumDecompiler {

std::string Disassembler::Disassemble(const std::filesystem::path& path) {
    m_Loader.LoadFile(path);
    std::string output = "";
    m_Output = "";
    m_ClassFileNames.clear();
    m_ClassToFunctions.clear();
    m_ClassFunctions.clear();
    m_ClassesMap.clear();

    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    if (extension == ".jar") {
        m_JarFile = m_Loader.GetJarFile();
        for (auto& classes : m_JarFile.m_Classes) {
            std::string accFlag = TitaniumDecompiler::ParseJavaClassAccessFlag(classes.m_AccessFlags & ACC_CLASS);
            std::string thisClass = GetConstantClassFromClass(nullptr, classes, classes.m_ThisClass);
            m_ClassFileNames.push_back(thisClass);

            output.append(".class " + accFlag + " " + thisClass + "\n");
            std::string superClass = GetConstantClassFromClass(nullptr, classes, classes.m_SuperClass);
            output.append(".super " + superClass + "\n\n");
            std::vector<TitaniumDecompiler::Fields> fields = classes.m_Fields;
            for (size_t i = 0; i < fields.size(); i++) {
                std::string fieldName = GetConstantUTF8FromClass(classes, fields.at(i).m_Name);
                std::string fieldDesc = GetConstantUTF8FromClass(classes, fields.at(i).m_Desc);
                std::string fieldAccessFlag = TitaniumDecompiler::ParseJavaFieldAccessFlag(fields.at(i).m_Access & ACC_FIELD);
                if (fieldAccessFlag.empty()) {
                    output.append(".field " + fieldName + " " + fieldDesc);
                } else {
                    output.append(".field " + fieldAccessFlag + " " + fieldName + " " + fieldDesc);
                }
                output.append("\n");
            }
            output.append(DisassembleClassFile(classes, thisClass));
        }
    } else {
        m_ClassFile = m_Loader.GetClassFile();
        if (m_ClassFile.m_Magic == 0) {
            return "";
        }

        std::string accFlag = TitaniumDecompiler::ParseJavaClassAccessFlag(m_ClassFile.m_AccessFlags & ACC_CLASS);
        std::string thisClass = GetConstantClassFromClass(nullptr, m_ClassFile, m_ClassFile.m_ThisClass);
        m_Functions.emplace_back(thisClass);
        m_ClassFileNames.push_back(thisClass);

        output.append(".class " + accFlag + " " + thisClass + "\n");
        std::string superClass = GetConstantClassFromClass(nullptr, m_ClassFile, m_ClassFile.m_SuperClass);
        output.append(".super " + superClass + "\n\n");
        std::vector<TitaniumDecompiler::Fields> fields = m_ClassFile.m_Fields;
        for (size_t i = 0; i < fields.size(); i++) {
            std::string fieldName = GetConstantUTF8FromClass(m_ClassFile, fields.at(i).m_Name);
            std::string fieldDesc = GetConstantUTF8FromClass(m_ClassFile, fields.at(i).m_Desc);
            std::string fieldAccessFlag = TitaniumDecompiler::ParseJavaFieldAccessFlag(fields.at(i).m_Access & ACC_FIELD);
            if (fieldAccessFlag.empty()) {
                output.append(".field " + fieldName + " " + fieldDesc);
            } else {
                output.append(".field " + fieldAccessFlag + " " + fieldName + " " + fieldDesc);
            }
            output.append("\n");
        }
        output.append(DisassembleClassFile(m_ClassFile, thisClass));
    }

    return output;
}

void Disassembler::CreateFunctions(const ClassFile& classFile, const std::string& className) {
    std::vector<Function> currentClassFunctions;

    for (auto& method : classFile.m_Methods) {
        std::string methodAccessFlag = TitaniumDecompiler::ParseJavaMethodsAccessFlag(method.m_AccessFlags & ACC_METHOD);
        std::string methodName = GetConstantUTF8FromClass(classFile, method.m_NameIndex);
        std::string methodDesc = GetConstantUTF8FromClass(classFile, method.m_DescIndex);
        Function function(methodName);
        function.SetMethod(method);
        function.SetMethodDesc(methodDesc);
        function.SetMethodAccessFlags(methodAccessFlag);
        function.SetFunctionClassFile(classFile);
        if (method.m_Attr.m_Attributes.at(0).tag == TitaniumDecompiler::AttributeTypes::CodeType) {
            auto info = method.m_Attr.m_Attributes.at(0).info;
            TitaniumDecompiler::Code& codeInstance = static_cast<TitaniumDecompiler::Code&>(*info);
            function.SetMaxStack(codeInstance.maxStack);
            function.SetMaxLocals(codeInstance.maxLocals);
            if (!codeInstance.attributes.empty()) {
                for (auto& attr : codeInstance.attributes) {
                    if (attr.tag == LocalVariableTableType) {
                        TitaniumDecompiler::LocalVariableTable& lvt = static_cast<TitaniumDecompiler::LocalVariableTable&>(*attr.info);
                    } else if (attr.tag == StackMapTableType) {
                        TitaniumDecompiler::StackMapTable& smt = static_cast<TitaniumDecompiler::StackMapTable&>(*attr.info);
                        function.SetStackMapTable(smt);
                        for (auto& entry : smt.entries) {
                            if (entry.m_Type >= 0 && entry.m_Type <= 63) {
                                TitaniumDecompiler::StackMapSame& same = static_cast<TitaniumDecompiler::StackMapSame&>(*entry.m_StackFrameType);
                            } else if (entry.m_Type >= 64 && entry.m_Type <= 127) {
                                TitaniumDecompiler::StackMapSameLocals1StackItemFrame& sameLocals =
                                    static_cast<TitaniumDecompiler::StackMapSameLocals1StackItemFrame&>(*entry.m_StackFrameType);
                            } else if (entry.m_Type >= 252 && entry.m_Type <= 254) {
                                TitaniumDecompiler::StackMapAppendFrame& appendFrame =
                                    static_cast<TitaniumDecompiler::StackMapAppendFrame&>(*entry.m_StackFrameType);

                                for (auto& local : appendFrame.locals) {
                                    if (local.m_Tags == VerificationTypeInfoTags::TYPE_INTEGER) {
                                        function.GetLocals().insert({appendFrame.offsetDelta, "int var"});
                                    }
                                }
                            }
                        }
                    }
                    // } else if(attr.tag == LocalVariableTypeTableType) {
                    //     TitaniumDecompiler::LocalVariableTypeTable& lvtt = static_cast<TitaniumDecompiler::LocalVariableTypeTable&>(*attr.info);
                    //     printf("%d\n", lvt.localVariableTableLength);
                    // }
                }
            }
            // m_Cfg = BuildCFG(codeInstance.code);
            function.SetFunctionCFG(std::make_shared<CFG>(BuildCFG(codeInstance.code)));
            m_ClassesMap.insert({className, methodName});
        }

        currentClassFunctions.push_back(function);
    }
    m_ClassToFunctions[className] = currentClassFunctions;
}

std::string Disassembler::DisassembleClassFile(const ClassFile& classFile, const std::string& className) {
    std::string output = "";
    std::string code = "";
    CreateFunctions(classFile, className);

    if (m_ClassToFunctions.find(className) != m_ClassToFunctions.end()) {
        auto& functions = m_ClassToFunctions[className];
        for (auto& function : functions) {
            output.append(ParseFunction(classFile, function));
        }
    }

    return output;
}

std::string Disassembler::ParseFunction(const ClassFile& classFile, Function& function) {
    std::string output = "";
    output.append(".method " + function.GetMethodAccessFlags() + " ");
    output.append(function.GetFunctionName());
    output.append(function.GetMethodDesc());
    output.append(":\n");
    output.append(".limit stack " + std::to_string(function.GetMaxStack()) + "\n");
    output.append(".limit locals " + std::to_string(function.GetMaxLocals()) + "\n");
    for (const auto& it : function.GetFunctionCFG()->m_Blocks) {
        output.append(ParseFunctionInstructions(function, classFile, it.second->GetInstructions()));
    }
    return output;
}
// void Disassembler::RemoveEmptyBlocks(CFG graph) {
//     std::vector<std::shared_ptr<BasicBlock>> blocks = graph.GetBlocks();
//     bool cont;
//     do {
//         cont = false;

//         for(size_t i = blocks.size() - 1; i >=0; i--) {
//             std::shared_ptr<BasicBlock> block = blocks.at(i);
//             if(RemoveEmptyBlocks(graph, block)) {
//                 cont = true;
//                 break;
//             }
//         }
//     } while(cont);
// }

// bool Disassembler::RemoveEmptyBlocks(CFG graph, std::shared_ptr<BasicBlock>
// block) {
//     bool deletedRanges = false;
//     if(block->GetInstructions().size() > 0) {
//         // std::cout << "HELLO FUCKER" << std::endl;
//         // std::cout << "Successor size: " <<
//         block->GetInstructions().at(i)->GetSuccessor().size() << std::endl;
//         if(block->GetSuccessor().size() > 1) {
//             if(block->GetPreds().size() > 1) {
//                 // std::cout << "empty block with multiple predecessors" <<
//                 std::endl;
//             }
//         }
//     }

//     std::unordered_set<std::shared_ptr<BasicBlock>>
//     setPreds(block->GetPreds().begin(), block->GetPreds().end());
//     std::unordered_set<std::shared_ptr<BasicBlock>>
//     setSuccs(block->GetSuccessor().begin(), block->GetSuccessor().end());

//     for(auto& pred : setPreds) {
//         for(auto& succ : setSuccs) {
//             pred->ReplaceSuccessor(block, succ);
//         }
//     }
//     // return false;

//     // std::unordered_set<std::shared_ptr<BasicBlock>> setExits;
//     // if(setExits.find(block)) {

//     // }

//     // std::vector<std::shared_ptr<BasicBlock>> blocks = graph.GetBlocks();
//     // bool cont;
//     // do {
//     //     cont = false;

//     //     for(size_t i = blocks.size() - 1; i >=0; i--) {
//     //         std::shared_ptr<BasicBlock> block = blocks.at(i);
//     //     }
//     // } while(cont);
// }

std::string Disassembler::GetClassName() { return TitaniumDecompiler::GetConstantClass(m_ClassFile.m_ThisClass); }

CFG Disassembler::BuildCFG(const std::vector<uint8_t>& bytes) {
    CFG cfg;
    cfg.SetCFGClassFile(m_ClassFile);
    InstrMap insns = ReadInstructions(bytes);
    std::map<uint32_t, std::shared_ptr<BasicBlock>> mapBlocks = cfg.CreateBasicBlocks(insns);

    cfg.ConnectBasicBlocks(mapBlocks);

    //  std::vector<std::shared_ptr<BasicBlock>> blocks = cfg.BuildBlocks(insns);
    //  cfg.BuildGraph();

    // cfg.PrintBasicBlocks(blocks);
    //  for(size_t i = 0; i < blocks.size(); i++) {
    //      auto block = blocks.at(i);
    //      cfg.m_Blocks.insert({i, block});
    //  }
    //  for(const auto& block : blocks) {
    //      for(size_t i = 0; i < block->GetInstructions().size(); i++) {
    //          std::cout << block->GetInstructions().at(i).opcodeName <<
    //          std::endl;
    //      }
    //      std::cout << block->m_ID << std::endl;
    //  }

    // for(int i = 0; i < cfg.m_Blocks.size(); i++) {
    //     std::cout << "basicblock begin" << std::endl;
    //     // for(int j = 0; j < cfg.m_Blocks.at(i)->GetInstructions().size();
    //     j++) {
    //     //     std::cout <<
    //     cfg.m_Blocks.at(i)->GetInstructions().at(j).opcodeName << std::endl;
    //     // }
    //     std::cout << "basicblock end" << std::endl;
    // }
    // cfg.ConnectBasicBlocks(cfg.m_Blocks);
    return cfg;
}

std::string Disassembler::ParseFunctionInstructions(Function& function, const ClassFile& classFile, const std::vector<Insn>& instructions) {
    std::string output = "";
    for (auto& insn : instructions) {
        output.append(insn.opcodeName + " ");
        switch (insn.opcode) {
            case OPCODE_GETSTATIC:
            case OPCODE_PUTSTATIC:
            case OPCODE_GETFIELD:
            case OPCODE_PUTFIELD:
            case OPCODE_INVOKESPECIAL:
            case OPCODE_INVOKEVIRTUAL:
            case OPCODE_INVOKEINTERFACE:
            case OPCODE_INVOKESTATIC:
            case OPCODE_INVOKEDYNAMIC:
            case OPCODE_NEW: {
                output.append(InstructionsWithReferences(function, classFile, insn));
                break;
            }
            case OPCODE_IF_ACMPEQ:
            case OPCODE_IF_ACMPNE:
            case OPCODE_IF_ICMPEQ:
            case OPCODE_IF_ICMPGE:
            case OPCODE_IF_ICMPGT:
            case OPCODE_IF_ICMPLE:
            case OPCODE_IF_ICMPLT:
            case OPCODE_IF_ICMPNE:
            case OPCODE_IFEQ:
            case OPCODE_IFNE:
            case OPCODE_IFGT:
            case OPCODE_IFLE:
            case OPCODE_IFLT:
            case OPCODE_IFNONNULL:
            case OPCODE_IFNULL: {
                output.append(ParseIfStatement(insn));
                break;
            }
            case OPCODE_IINC: {
                output.append(ParseIncrement(insn));
                break;
            }
            case OPCODE_LDC: {
                output.append(" \"");
                output.append(LoadConstFromPool(function, classFile, insn));
                output.append("\"");
                break;
            }
            case OPCODE_BIPUSH: {
                output.append(ParseBiPushOpCode(insn));

                break;
            }
            case OPCODE_GOTO: {
                output.append(ParseGoto(insn));
                break;
            }
            case OPCODE_CHECKCAST: {
                output.append(" ");
                // output.append(ParseCheckCast(insn));
                break;
            }
            case OPCODE_RETURN:
            case OPCODE_ARETURN:
            case OPCODE_DRETURN:
            case OPCODE_FRETURN:
            case OPCODE_LRETURN:
            case OPCODE_IRETURN: {
                output.append("\n");
                break;
            }
            case OPCODE_MULTIANEWARRAY: {
                output.append(" ");
                // output.append(ParseMultiNewArray(insn));
                break;
            }
        }
        output.append("\n");
    }
    //     } else if(it->first == OPCODE_TABLESWITCH) {
    //         // Program Counter for tableswitch is 3 - (program counter mod 4) to get the correct default byte
    //         // There are a few other ways I have seen but this one is the most reliable for me to implement.
    //         pc += 3 - (pc % 4);

    //         int32_t defaultByte = ((bytes[pc + 1] << 24)
    //             | (bytes[pc + 2] << 16)
    //             | (bytes[pc + 3] << 8)
    //             | (bytes[pc + 4])
    //         ) + pc;

    //         int32_t lowByte = ((bytes[pc + 5] << 24)
    //             | (bytes[pc + 6] << 16)
    //             | (bytes[pc + 7] << 8)
    //             | (bytes[pc + 8]));

    //         int32_t highByte = ((bytes[pc + 9] << 24)
    //             | (bytes[pc + 10] << 16)
    //             | (bytes[pc + 11] << 8)
    //             | (bytes[pc + 12]));

    //         pc += 12;
    //         int32_t numOffsets = highByte - lowByte + 1;

    //         std::vector<int32_t> indices;
    //         for(int i = 0; i < numOffsets; i++) {
    //             int32_t count = ((bytes[pc + 1] << 24)
    //                 | (bytes[pc + 2] << 16)
    //                 | (bytes[pc + 3] << 8)
    //                 | (bytes[pc + 4]));
    //             indices.push_back(count + numOffsets);
    //             pc += 4;
    //         }

    //         output.append(" " + std::to_string(lowByte) + " " + std::to_string(highByte));
    //         for(size_t i = 0; i < indices.size(); i++) {
    //             output.append("\n\t" + std::to_string(i + 1) + ": " + std::to_string(indices.at(i)));
    //         }
    //         output.append("\n\tdefault: " + std::to_string(defaultByte));
    //     }
    return output;
}

std::string Disassembler::GetConstantUTF8FromClass(const ClassFile& classFile, int idx) {
    int constantIdx = idx - 1;

    auto cpInfo = classFile.m_ConstantPool.m_ConstPoolInfo.at(constantIdx);
    auto* utf8Info = cpInfo.asUTF8Info();

    std::stringstream s;
    for (int i = 0; i < utf8Info->length; i++) {
        s << utf8Info->bytes[i];
    }

    return s.str();
}

std::string Disassembler::GetConstantClassFromClass(Function* function, const ClassFile& classFile, int idx) {
    if (function == nullptr) {
        int constantIdx = idx - 1;
        auto cpInfo = classFile.m_ConstantPool.m_ConstPoolInfo.at(constantIdx);
        auto* classInfo = cpInfo.asClassInfo();
        std::string className = GetConstantUTF8FromClass(classFile, classInfo->nameIndex);
        return className;
    } else {
        int constantIdx = idx - 1;
        auto cpInfo = classFile.m_ConstantPool.m_ConstPoolInfo.at(constantIdx);
        auto* classInfo = cpInfo.asClassInfo();
        std::string className = GetConstantUTF8FromClass(classFile, classInfo->nameIndex);
        function->InsertIntoRefConstPool(idx - 1, className);
        return className;
    }
}

std::string Disassembler::GetNameAndTypeFromClass(Function* function, const ClassFile& classFile, int idx) {
    if (function == nullptr) {
        int constantIdx = idx - 1;
        auto cpInfo = classFile.m_ConstantPool.m_ConstPoolInfo.at(constantIdx);
        auto* nameAndTypeInfo = cpInfo.asNameAndType();
        std::string output = "";
        InsnTempWrapper insn;
        std::string name = GetConstantUTF8FromClass(classFile, nameAndTypeInfo->nameIndex);
        std::string desc = GetConstantUTF8FromClass(classFile, nameAndTypeInfo->descriptorIndex);
        insn.m_Name = name;
        insn.m_Desc = desc;

        output.append(name);
        output.append(":");
        output.append(desc);
        m_TempInsn.push_back(insn);
        return output;
    } else {
        int constantIdx = idx - 1;
        auto cpInfo = classFile.m_ConstantPool.m_ConstPoolInfo.at(constantIdx);
        auto* nameAndTypeInfo = cpInfo.asNameAndType();
        std::string output = "";
        InsnTempWrapper insn;
        std::string name = GetConstantUTF8FromClass(classFile, nameAndTypeInfo->nameIndex);
        std::string desc = GetConstantUTF8FromClass(classFile, nameAndTypeInfo->descriptorIndex);
        function->InsertIntoRefConstPool(idx - 1, name + desc);
        insn.m_Name = name;
        insn.m_Desc = desc;

        output.append(name);
        output.append(":");
        output.append(desc);
        m_TempInsn.push_back(insn);
        return output;
    }
}

BootstrapMethods Disassembler::GetBootstrapMethod(const ClassFile& classFile) {
    for (auto& attr : classFile.m_Attributes.m_Attributes) {
        if (attr.tag == AttributeTypes::BootstrapMethodsType) {
            if (auto* bm = dynamic_cast<BootstrapMethods*>(attr.info.get())) {
                return *bm;
            } else {
                throw std::runtime_error("Invalid cast to BootstrapMethods");
            }
        }
    }
    throw std::runtime_error("BootstrapMethods attribute not found");
}

std::string Disassembler::InstructionsWithReferences(Function& function, const ClassFile& classFile, const Insn& insn) {
    std::string output = "";
    InsnTempWrapper tempInsn;

    ConstPoolInfo info = classFile.m_ConstantPool.m_ConstPoolInfo.at(insn.Op1.value - 1);
    if (info.Tag == Tags::Field) {
        auto field = static_cast<FieldRefInfo*>(info.Info.get());
        std::string classInf = GetConstantClassFromClass(&function, classFile, field->classIndex);
        std::string nameInf = GetNameAndTypeFromClass(&function, classFile, field->nameAndTypeIndex);
        tempInsn.m_ClassInf = classInf;
        tempInsn.m_NameInf = nameInf;

        output.append(classInf);
        output.append(".");
        output.append(nameInf);
    } else if (info.Tag == Tags::Method) {
        auto method = static_cast<MethodRefInfo*>(info.Info.get());
        std::string classInf = GetConstantClassFromClass(&function, classFile, method->classIndex);
        std::string nameInf = GetNameAndTypeFromClass(&function, classFile, method->nameAndTypeIndex);
        tempInsn.m_ClassInf = classInf;
        tempInsn.m_NameInf = nameInf;

        output.append(classInf);
        output.append(".");
        output.append(nameInf);
    } else if (info.Tag == Tags::InvokeDynamic) {
        auto method = static_cast<InvokeDynamicInfo*>(info.Info.get());
        BootstrapMethodsInner bootstrapMethod = GetBootstrapMethod(classFile).bootstrapsMethods.at(method->bootstrapMethodAttrIndex);
        auto cpi = classFile.m_ConstantPool.m_ConstPoolInfo.at(bootstrapMethod.bootstrapMethodRef);
        if (cpi.Tag == Tags::Method) {
            auto methodInner = static_cast<MethodRefInfo*>(cpi.Info.get());
            std::string classInf = GetConstantClassFromClass(&function, classFile, methodInner->classIndex);
            BootstrapMethodsInner bootstrapMethod = GetBootstrapMethod(classFile).bootstrapsMethods.at(method->bootstrapMethodAttrIndex);
            std::string nameInf = GetNameAndTypeFromClass(&function, classFile, methodInner->nameAndTypeIndex);
            tempInsn.m_ClassInf = classInf;
            tempInsn.m_NameInf = nameInf;

            output.append(classInf);
            output.append(".");
            output.append(nameInf);
        }
    } else if (info.Tag == Tags::InterfaceMethod) {
        auto method = static_cast<InterfaceMethodRef*>(info.Info.get());
        std::string classInf = GetConstantClassFromClass(&function, classFile, method->classIndex);
        std::string nameInf = GetNameAndTypeFromClass(&function, classFile, method->nameAndTypeIndex);
        tempInsn.m_ClassInf = classInf;
        tempInsn.m_NameInf = nameInf;

        output.append(classInf);
        output.append(".");
        output.append(nameInf);
    } else if (info.Tag == Tags::Class) {
        std::string classInf = GetConstantClassFromClass(&function, classFile, insn.Op1.value);
        tempInsn.m_ClassInf = classInf;

        output.append(classInf);
    }

    m_TempInsn.push_back(tempInsn);
    return output;
}

std::string Disassembler::LoadConstFromPool(const Function& function, const ClassFile& classFile, const Insn& insn) {
    Function func = function;
    std::string output = "";
    InsnTempWrapper tempInsn;
    ConstPoolInfo info = classFile.m_ConstantPool.m_ConstPoolInfo.at(insn.Op1.value - 1);
    if (info.Tag == Tags::String) {
        auto str = static_cast<StringInfo*>(info.Info.get());
        std::string constStr = GetConstantUTF8FromClass(classFile, str->stringIndex);
        tempInsn.m_ConstString = constStr;
        output = constStr;
        Function funct = function;
        funct.InsertIntoRefConstPool(insn.Op1.value - 1, constStr);
    } else {
        printf("info.tag = %d\n", info.Tag);
    }

    m_TempInsn.push_back(tempInsn);
    return output;
}

std::string Disassembler::ParseIfStatement(const Insn& insn) {
    std::string output = "";
    output.append(std::to_string(insn.Op1.addr));
    return output;
}

std::string Disassembler::ParseIncrement(const Insn& insn) {
    std::string output = "";
    uint8_t index1 = (insn.Op2.value >> 8) & 0xFF;  // Extract the high byte
    uint8_t index2 = insn.Op2.value & 0xFF;
    output.append(" " + std::to_string(index1) + ", " + std::to_string(index2));
    return output;
}

std::string Disassembler::ParseGoto(const Insn& insn) {
    std::string output = "";
    output.append(std::to_string(insn.Op1.addr));
    return output;
}

std::string Disassembler::ParseBiPushOpCode(const Insn& insn) {
    std::string output = "";
    output.append(std::to_string(insn.Op1.value));
    return output;
}

// std::string ParseCheckCast(const std::vector<uint8_t>& bytes, uint32_t pc) {
//     std::string output = "";
//     ConstPoolInfo info = GetConstant(idx-1);
//     if(info.Tag == Tags::Class) {
//         std::string constStr = GetConstantClass(idx);
//         output = constStr;
//     }

//     return output;
// }

// std::string ParseMultiNewArray(const std::vector<uint8_t>& bytes, uint32_t pc) {
//     std::string output = "";
//     if(pc + 3 < bytes.size()) {
//         uint8_t index1 = bytes[pc + 1];
//         uint8_t index2 = bytes[pc + 2];
//         int idx = (index1 << 8) | index2;
//         uint8_t index3 = bytes[pc + 3];
//         ConstPoolInfo info = GetConstant(idx-1);
//         if(info.Tag == Tags::Class) {
//             std::string constStr = GetConstantClass(idx);
//             output.append(constStr);
//         }
//         output.append(" " + std::to_string(index3));
//     } else {
//         printf("Not enough bytes for reference\n");
//         output = "";
//     }

//     return output;
// }

}  // namespace TitaniumDecompiler
