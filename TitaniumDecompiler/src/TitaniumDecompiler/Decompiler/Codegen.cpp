#include "Codegen.h"

#include <TitaniumLogger/Logger/Log.h>

#include <memory>
#include <stdexcept>
#include <variant>

#include "../../Utils/Utils.h"
#include "AST.h"
#include "CommonOps.h"
#include "DomHelper.h"
#include "GraphUtils.h"

namespace TitaniumDecompiler {

/**
 * Responsible for parsing Java bytecode instructions and converting them
 * into an Abstract Syntax Tree representation that can be further analyzed,
 * optimized, and decompiled into high-level Java code.
 *
 */

uint32_t Codegen::FindStartNodeId(
    const std::vector<std::shared_ptr<BasicBlock>>& blocks) {
    for (const auto& block : blocks) {
        if (block->GetPreds().empty()) {
            return block->m_ID;
        }
    }

    return -1;
}

// std::vector<AST> Codegen::GenJavaCode(const Function& function) {
//     Function funct = function;
//     std::vector<AST> ast_; // Renamed to avoid shadowing

//     std::shared_ptr<CFG> cfg = funct.GetFunctionCFG();
//     if(!cfg) {
//         std::cerr << "Error: Function has a null CFG." << std::endl;
//         return ast_;
//     }
//     // 1. Collect CFG Blocks
//     std::map<int, std::shared_ptr<BasicBlock>> cfgBlocks;
//     for (auto& block : funct.GetFunctionCFG()->GetBlocks()) {
//         cfgBlocks[block->m_ID] = block; // Store blocks by ID for easy access
//     }

//     // 2. Calculate Dominators
//     uint32_t startNodeId = FindStartNodeId(cfg->GetBlocks()); // Assuming 0
//     is the start node ID.  **CRITICAL:  Make sure this is correct for your
//     CFG!** std::map<uint32_t, uint32_t> dominators =
//     lengauerTarjan(cfg->m_Blocks, startNodeId); DominatorTree
//     domTree(dominators, cfg->m_Blocks, startNodeId);

//     // 3. Convert CFG to AST, starting from the entry block
//     //    This is the core change:  Instead of processing instructions
//     linearly,
//     //    we start at the entry block of the CFG and use convertCFGToAST to
//     //    recursively build the AST.
//     if (cfgBlocks.find(startNodeId) != cfgBlocks.end()) { // Ensure the start
//     node exists
//         ASTPtr rootAST = ConvertCFGToAST(cfgBlocks[startNodeId], cfgBlocks,
//         dominators); if (rootAST) {
//              std::vector<ASTPtr> rootStatements;
//              rootStatements.push_back(rootAST);
//             //  ast_.push_back(std::move(*rootAST)); //  This is wrong.  The
//             function returns a vector. for(auto& statement : rootStatements){
//                  ast_.push_back(std::move(*statement));
//             }
//         }
//     }
//     else{
//         std::cerr << "Error: Start node not found in CFG blocks." <<
//         std::endl;
//     }

//     return std::move(ast_);
// }

std::vector<AST> Codegen::GenJavaCode(const Function& function) {
    Function funct = function;

    // Collect all instructions from all blocks
    for (auto& block : funct.GetFunctionCFG()->GetBlocks()) {
        auto instr = block->GetInstructions();
        funcInstructions.insert(funcInstructions.end(), instr.begin(),
                                instr.end());
    }

    uint32_t startNodeId = 0;
    std::map<uint32_t, uint32_t> dominators =
        lengauerTarjan(funct.GetFunctionCFG()->m_Blocks, startNodeId);
    DominatorTree domTree(dominators, funct.GetFunctionCFG()->m_Blocks,
                          startNodeId);

    std::vector<std::set<uint32_t>> loops =
        FindLoops(*funct.GetFunctionCFG(), domTree);

    // TD_DECOMP_INFO("Found {0} loops:", loops.size());
    // for (size_t i = 0; i < loops.size(); ++i) {
    //     TD_DECOMP_INFO("Loop {0}: ", i + 1);
    //     for (uint32_t nodeId : loops[i]) {
    //         TD_DECOMP_INFO("{0} ", nodeId);
    //     }
    //     TD_DECOMP_INFO("\n");
    // }
    // DomHelper domHelper(*funct.GetFunctionCFG());
    // domHelper.Compute();
    // m_Dominators = domHelper.GetDoms();

    // Process instructions normally
    for (auto& insn : funcInstructions) {
        auto val = ReadInstruction(funct, insn);
        if (val) {
            ast_.push_back(std::make_unique<AST>(std::move(*val)));
        } else {
            continue;
        }
    }

    return std::move(ast_);
}

ASTPtr Codegen::CreateForLoop(
    std::shared_ptr<BasicBlock> loopHeader,
    const std::map<int, std::shared_ptr<BasicBlock>>& cfgBlocks,
    const std::map<uint32_t, uint32_t>& dominators) {
    std::cerr << "constructForLoop called for block " << loopHeader->m_ID
              << std::endl;

    // 1. Identify Potential Loop Header (using dominance)
    //    The loopHeader *dominates* all blocks in the loop.  We've already got
    //    the loopHeader.

    // 2. Find Loop Blocks (using dominance)
    std::set<int> loopBlockIds;
    std::vector<int> queue;
    queue.push_back(loopHeader->m_ID);
    loopBlockIds.insert(loopHeader->m_ID);
    std::cerr << "  Starting loop block identification from header: "
              << loopHeader->m_ID << std::endl;

    while (!queue.empty()) {
        int currentBlockId = queue.front();
        queue.erase(queue.begin());
        std::cerr << "    Processing block: " << currentBlockId << std::endl;
        if (cfgBlocks.find(currentBlockId) == cfgBlocks.end()) {
            std::cerr << "      Error: cfgBlocks doesn't contain block "
                      << currentBlockId << std::endl;
            continue;
        }
        for (const auto& successor :
             cfgBlocks.at(currentBlockId)
                 ->GetSuccessor()) {  // Corrected: GetSuccessors()
            std::cerr << "      Successor of " << currentBlockId << ": "
                      << successor->m_ID << std::endl;
            if (dominators.count(successor->m_ID) == 0) {
                std::cerr << "        Warning: Dominator information not found "
                             "for block "
                          << successor->m_ID << std::endl;
                continue;
            }
            if (dominators.at(successor->m_ID) == loopHeader->m_ID &&
                loopBlockIds.find(successor->m_ID) == loopBlockIds.end()) {
                std::cerr << "        Adding block " << successor->m_ID
                          << " to loop blocks." << std::endl;
                queue.push_back(successor->m_ID);
                loopBlockIds.insert(successor->m_ID);
            } else if (loopBlockIds.find(successor->m_ID) !=
                       loopBlockIds.end()) {
                std::cerr << "        Block " << successor->m_ID
                          << " already in loop blocks." << std::endl;
            } else {
                std::cerr << "        Block " << successor->m_ID
                          << " is not dominated by header or already in loop."
                          << std::endl;
            }
        }
    }

    std::vector<std::shared_ptr<BasicBlock>> loopBlocks;
    std::cerr << "  Identified loop block IDs: ";
    for (int blockId : loopBlockIds) {
        std::cerr << blockId << " ";
        if (cfgBlocks.count(blockId)) {
            loopBlocks.push_back(cfgBlocks.at(blockId));
        } else {
            std::cerr << "Error: cfgBlocks doesn't contain loop block "
                      << blockId << std::endl;
        }
    }
    std::cerr << std::endl;

    // 3. Find Back Edge (within loop blocks)
    std::shared_ptr<BasicBlock> backEdgeSource = nullptr;
    std::cerr << "  Searching for back edge..." << std::endl;
    for (const auto& loopBlock : loopBlocks) {
        std::cerr << "    Checking block " << loopBlock->m_ID
                  << " for back edge." << std::endl;
        if (loopBlock->m_ID == loopHeader->m_ID) {
            std::cerr << "      Skipping loop header." << std::endl;
            continue;
        }
        for (const auto& successor : loopBlock->GetSuccessor()) {
            std::cerr << "      Successor of " << loopBlock->m_ID << ": "
                      << successor->m_ID << std::endl;
            if (successor->m_ID == loopHeader->m_ID &&
                loopBlockIds.count(loopBlock->m_ID)) {
                std::cerr << "      Found back edge from block "
                          << loopBlock->m_ID << " to header "
                          << loopHeader->m_ID << std::endl;
                backEdgeSource = loopBlock;
                break;
            }
        }
        if (backEdgeSource) break;
    }

    if (!backEdgeSource) {
        std::cerr << "  No back edge found within loop blocks. Not a loop."
                  << std::endl;
        return nullptr;  // Not a loop
    }

    std::cerr << "  Back edge found from block " << backEdgeSource->m_ID
              << std::endl;

    // 4. Identify Condition (in loop header) - as before
    ASTPtr condition = nullptr;
    std::cerr << "  Searching for condition in loop header " << loopHeader->m_ID
              << std::endl;
    for (const auto& insn : loopHeader->GetInstructions()) {
        const Insn& instruction = insn;
        std::cerr << "    Instruction in header: " << instruction.opcode
                  << std::endl;
        if (instruction.opcode >= OPCODE_IFEQ &&
            instruction.opcode <= OPCODE_IFNONNULL) {
            std::vector<StackEntry> stack;
            condition = ConvertConditionToAST(instruction, stack);
            std::cerr << "      Condition found." << std::endl;
            break;
        }
    }
    if (!condition) {
        std::cerr << "  No condition found in loop header. Returning nullptr."
                  << std::endl;
        return nullptr;
    }

    // 5. Find Update (in back edge source) - as before
    ASTPtr update = nullptr;
    std::cerr << "  Searching for update in back edge source "
              << backEdgeSource->m_ID << std::endl;
    for (const auto& insn : backEdgeSource->GetInstructions()) {
        const Insn& instruction = insn;
        std::cerr << "    Instruction in back edge source: "
                  << instruction.opcode << std::endl;
        if (instruction.opcode == OPCODE_IINC ||
            instruction.opcode == OPCODE_ASTORE ||
            instruction.opcode == OPCODE_ISTORE ||
            instruction.opcode == OPCODE_FSTORE ||
            instruction.opcode == OPCODE_DSTORE ||
            instruction.opcode == OPCODE_LSTORE) {
            update = ConvertUpdateToAST(instruction);
            std::cerr << "      Update found." << std::endl;
            break;
        }
    }

    // 6. Extract loop body (using loopBlocks)
    std::vector<ASTPtr> loopBody;
    std::cerr << "  Extracting loop body from blocks: ";
    for (int blockId : loopBlockIds) {
        std::cerr << blockId << " ";
        if (blockId != loopHeader->m_ID && blockId != backEdgeSource->m_ID) {
            std::cerr << "    Converting block " << blockId
                      << " to loop body AST." << std::endl;
            loopBody.push_back(
                ConvertCFGToAST(cfgBlocks.at(blockId), cfgBlocks, dominators));
        } else {
            std::cerr << "    Skipping header or back edge source: " << blockId
                      << std::endl;
        }
    }
    std::cerr << std::endl;

    AST::Loop forLoop{condition, loopBody};
    std::cerr << "  For loop created." << std::endl;
    return std::make_shared<AST>(forLoop);
}

std::optional<AST> Codegen::ReadInstruction(const Function& function,
                                            const Insn& instruction) {
    Function funct = function;
    auto block = funct.GetFunctionCFG()->GetBlockByInsn(instruction);
    switch (instruction.opcode) {
        case OPCODE_BIPUSH: {  // single byte integer push
            int32_t value = static_cast<int32_t>(instruction.Op1.value);
            m_Stack.emplace_back(value);
            break;
        }
        case OPCODE_SIPUSH: {  // short integer push
            int32_t value = static_cast<int32_t>(instruction.Op1.value);
            m_Stack.emplace_back(value);
            break;
        }
        case OPCODE_LDC:  // Load constant
        case OPCODE_LDC_W: {
            ConstPoolInfo cpi = m_ClassFile.m_ConstantPool.m_ConstPoolInfo.at(
                instruction.Op1.value - 1);
            if (cpi.Tag == Tags::String) {
                // If constant is a string, retrieve the actual String and push
                // it onto m_Stack
                auto str = cpi.GetAs<StringInfo>();
                // static_cast<StringInfo*>(cpi.Info.get());
                std::string constStr =
                    GetConstantUTF8FromClass(m_ClassFile, str->stringIndex);
                m_Stack.emplace_back(StackEntry::String{constStr});
            } else if (cpi.Tag == Tags::Int) {
                int32_t value = static_cast<int32_t>(instruction.Op1.value);
                m_Stack.emplace_back(value);
            } else if (cpi.Tag == Tags::Float) {
                float value = static_cast<float>(instruction.Op1.value);
                m_Stack.emplace_back(value);
            }
            break;
        }

            // case OPCODE_LDC2_W: {
            // }

        case OPCODE_ACONST_NULL:  // NULL reference
            m_Stack.emplace_back(StackEntry::Null());
            break;
        // ICONST pushes predefined integer constants onto stack
        case OPCODE_ICONST_M1:
            m_Stack.emplace_back(-1);
            break;

        case OPCODE_ICONST_0:
            m_Stack.emplace_back(0);
            break;

        case OPCODE_ICONST_1:
            m_Stack.emplace_back(1);
            break;

        case OPCODE_ICONST_2:
            m_Stack.emplace_back(2);
            break;

        case OPCODE_ICONST_3:
            m_Stack.emplace_back(3);
            break;

        case OPCODE_ICONST_4:
            m_Stack.emplace_back(4);
            break;

        case OPCODE_ICONST_5:
            m_Stack.emplace_back(5);
            break;
        // FCONST pushes predefined float constants onto stack
        case OPCODE_FCONST_0:
            m_Stack.emplace_back(0.0f);
            break;

        case OPCODE_FCONST_1:
            m_Stack.emplace_back(1.0f);
            break;

        case OPCODE_FCONST_2:
            m_Stack.emplace_back(2.0f);
            break;

        case OPCODE_DCONST_0:
            m_Stack.emplace_back(0.0);
            break;

        case OPCODE_DCONST_1:
            m_Stack.emplace_back(1.0);
            break;
        // LCONST pushes predifned long constants onto stack
        case OPCODE_LCONST_0:
            m_Stack.emplace_back(static_cast<int64_t>(0L));
            break;

        case OPCODE_LCONST_1:
            m_Stack.emplace_back(static_cast<int64_t>(1L));
            break;
        // Loads local variables onto stack
        case OPCODE_ALOAD:
        case OPCODE_FLOAD:
        case OPCODE_DLOAD:
        case OPCODE_ILOAD:
        case OPCODE_LLOAD: {
            Load(instruction.Op1.addr -
                 1);  // Op1.value is variable index in the function's local
                      // variable array
            break;
        }
        case OPCODE_ALOAD_0:
        case OPCODE_FLOAD_0:
        case OPCODE_DLOAD_0:
        case OPCODE_ILOAD_0:
        case OPCODE_LLOAD_0: {
            Load(0);
            break;
        }
        case OPCODE_ALOAD_1:
        case OPCODE_FLOAD_1:
        case OPCODE_DLOAD_1:
        case OPCODE_ILOAD_1:
        case OPCODE_LLOAD_1: {
            Load(1);
            break;
        }
        case OPCODE_ALOAD_2:
        case OPCODE_FLOAD_2:
        case OPCODE_DLOAD_2:
        case OPCODE_ILOAD_2:
        case OPCODE_LLOAD_2: {
            Load(2);
            break;
        }
        case OPCODE_ALOAD_3:
        case OPCODE_FLOAD_3:
        case OPCODE_DLOAD_3:
        case OPCODE_ILOAD_3:
        case OPCODE_LLOAD_3: {
            Load(3);
            break;
        }
        case OPCODE_ISTORE:
        case OPCODE_ASTORE:
        case OPCODE_DSTORE:
        case OPCODE_LSTORE:
        case OPCODE_FSTORE:
            return Store(instruction.Op1.value);
        case OPCODE_ISTORE_0:
        case OPCODE_ASTORE_0:
        case OPCODE_DSTORE_0:
        case OPCODE_LSTORE_0:
        case OPCODE_FSTORE_0:
            return Store(0);
        case OPCODE_ISTORE_1:
        case OPCODE_ASTORE_1:
        case OPCODE_DSTORE_1:
        case OPCODE_LSTORE_1:
        case OPCODE_FSTORE_1:
            return Store(1);
        case OPCODE_ISTORE_2:
        case OPCODE_ASTORE_2:
        case OPCODE_DSTORE_2:
        case OPCODE_LSTORE_2:
        case OPCODE_FSTORE_2:
            return Store(2);
        case OPCODE_ISTORE_3:
        case OPCODE_ASTORE_3:
        case OPCODE_DSTORE_3:
        case OPCODE_LSTORE_3:
        case OPCODE_FSTORE_3:
            return Store(3);
        // Array storage operations
        case OPCODE_AASTORE:    // Reference Array
        case OPCODE_BASTORE:    // Byte or boolean
        case OPCODE_CASTORE:    // char
        case OPCODE_DASTORE:    // double
        case OPCODE_FASTORE:    // float
        case OPCODE_IASTORE:    // int
        case OPCODE_LASTORE:    // long
        case OPCODE_SASTORE: {  // short
            StackEntry val = PopStack();
            StackEntry index = PopStack();
            StackEntry array = PopStack();
            if (array.isArray()) {
                StackEntry::Array arr = array.getArrayValue();
                arr.elements.emplace_back(val);
                m_Stack.emplace_back(
                    StackEntry::Array{arr.type, arr.length, arr.elements});
            } else {
                // if array is not explicitly an array, assume it is an object
                // array.
                auto entry = ConvertStackEntryToAST(array);
                AST::ArrayIndex arrIndex{entry, ConvertStackEntryToAST(index),
                                         array.ty()};
                AST::ReAssignment reAssign{std::make_shared<AST>(arrIndex),
                                           ConvertStackEntryToAST(val)};
                return reAssign;
            }
            break;
        }
        // arithmetic operations opcodes, pushes the respective operation onto
        // the stack
        case OPCODE_IADD:
        case OPCODE_FADD:
        case OPCODE_DADD:
        case OPCODE_LADD:
            BinaryOper(BinaryOp::Add);
            break;
        case OPCODE_ISUB:
        case OPCODE_FSUB:
        case OPCODE_DSUB:
        case OPCODE_LSUB:
            BinaryOper(BinaryOp::Sub);
            break;
        case OPCODE_IMUL:
        case OPCODE_FMUL:
        case OPCODE_DMUL:
        case OPCODE_LMUL:
            BinaryOper(BinaryOp::Mul);
            break;
        case OPCODE_IDIV:
        case OPCODE_FDIV:
        case OPCODE_DDIV:
        case OPCODE_LDIV:
            BinaryOper(BinaryOp::Div);
            break;
        case OPCODE_IREM:
        case OPCODE_FREM:
        case OPCODE_DREM:
        case OPCODE_LREM:
            BinaryOper(BinaryOp::Rem);
            break;
        // Bitwise shift operations
        // Shift left a << b
        case OPCODE_ISHL:
        case OPCODE_LSHL:
            BinaryOper(BinaryOp::Shl);
            break;
        // Shift right a >> b
        case OPCODE_ISHR:
        case OPCODE_LSHR:
            BinaryOper(BinaryOp::Shr);
            break;
        // Unsigned shift right a >>> b
        case OPCODE_IUSHR:
        case OPCODE_LUSHR:
            BinaryOper(BinaryOp::UShr);
            break;
        // Bitwise logical operations
        case OPCODE_IXOR:
        case OPCODE_LXOR:
            BinaryOper(BinaryOp::Xor);
            break;
        case OPCODE_IAND:
        case OPCODE_LAND:
            BinaryOper(BinaryOp::And);
            break;
        case OPCODE_IOR:
        case OPCODE_LOR:
            BinaryOper(BinaryOp::Or);
            break;
        case OPCODE_FCMPG:
        case OPCODE_DCMPG:
            BinaryOper(BinaryOp::GreaterThan);
            break;
        case OPCODE_FCMPL:
        case OPCODE_DCMPL:
            BinaryOper(BinaryOp::LessThan);
            break;
        case OPCODE_INEG:
        case OPCODE_FNEG:
        case OPCODE_DNEG:
        case OPCODE_LNEG: {
            StackEntry val = PopStack();
            m_Stack.emplace_back(StackEntry::UnaryOperation{
                UnaryOp::Neg, std::make_unique<StackEntry>(std::move(val))});
            break;
        }
        // Process opcode for type checking
        case OPCODE_INSTANCEOF: {
            StackEntry obj1 = PopStack();  // pop object reference
            std::string obj2 = GetConstantClassFromClass(
                m_ClassFile,
                instruction.Op1
                    .value);  // Retrieve class name from constant pool
            StackEntry::Class obj2Class{obj2};
            StackEntry::BinaryOperation binOp{
                std::make_shared<StackEntry>(obj1), BinaryOp::InstanceOf,
                std::make_shared<StackEntry>(obj2Class)};
            m_Stack.emplace_back(binOp);
            break;
        }
        case OPCODE_LCMP: {
            break;
        }
        // Process opcode for return statements
        case OPCODE_RETURN: {
            AST::Return retStmt{nullptr};
            return AST(retStmt);
        }
        case OPCODE_ARETURN:
        case OPCODE_IRETURN:
        case OPCODE_FRETURN:
        case OPCODE_DRETURN:
        case OPCODE_LRETURN: {
            return AST::Return{ConvertStackEntryToAST(PopStack())};
        }
        // Process opcode for dynamic method calls
        case OPCODE_INVOKEDYNAMIC: {
            auto methStack = GetDynamicInvokeRefFromIndex(
                m_ClassFile, instruction.Op1.value);

            if (methStack.m_Class == "StringConcatFactory" &&
                methStack.m_Name == "makeConcatWithConstants") {
                auto args = methStack.unparsed_args;
                StringConcatFactory(args);
                // return nullptr;
                break;
            }

            if (methStack.desc.args.empty()) {
                break;
            }

            std::vector<StackEntry> args;

            for (int i = 0; i < methStack.desc.args.size(); i++) {
                args.emplace_back(PopStack());
            }

            std::string name = methStack.m_Class + "." + methStack.m_Name;

            StackEntry::Function func{getLastClassNamePart(name), args,
                                      methStack.desc.returnType};
            m_Stack.emplace_back(func);
            break;
        }
        // Process opcodes for interface method calls
        case OPCODE_INVOKEINTERFACE: {
            auto methStack =
                GetMethodRefFromIndex(m_ClassFile, instruction.Op1.value);

            if (methStack.desc.args.empty()) {
                break;
            }

            std::vector<StackEntry> args;

            for (int i = 0; i < methStack.desc.args.size(); i++) {
                args.emplace_back(PopStack());
            }

            std::string name = methStack.m_Class + "." + methStack.m_Name;

            StackEntry::Function func{getLastClassNamePart(name), args,
                                      methStack.desc.returnType};
            m_Stack.emplace_back(func);
            break;
        }
        // Process opcode for constructors and private method calls
        case OPCODE_INVOKESPECIAL: {
            MethodRefStack methStack =
                GetMethodRefFromIndex(m_ClassFile, instruction.Op1.value);
            std::vector<StackEntry> args;
            for (auto& descArgs : methStack.desc.args) {
                args.emplace_back(PopStack());
            }

            auto obj = PopStack();
            std::string name = "";

            if (methStack.m_Name == "<init>" ||
                methStack.m_Name == "<clinit>") {
                if (obj.isClass()) {
                    name = obj.getClassValue().m_ClassName;
                }
            } else {
                if (obj.isClass()) {
                    name = obj.getClassValue().m_ClassName + "." +
                           methStack.m_Name;
                }
            }

            StackEntry::Function func{name, args,
                                      Type::ClassNameType(methStack.m_Class)};
            m_Stack.emplace_back(func);
            break;
        }
        // Process opcode for static method calls
        case OPCODE_INVOKESTATIC: {
            MethodRefStack methStack =
                GetMethodRefFromIndex(m_ClassFile, instruction.Op1.value);
            std::vector<StackEntry> args;
            for (auto& descArgs : methStack.desc.args) {
                args.emplace_back(PopStack());
            }

            std::string funcName = methStack.m_Class + "." + methStack.m_Name;
            StackEntry::Function func{funcName, args,
                                      methStack.desc.returnType};

            if (methStack.desc.returnType.kind == Type::Void) {
                AST::Object object{methStack.m_Class};
                std::vector<ASTPtr> astArgs;
                for (auto& stacks : args) {
                    astArgs.emplace_back(ConvertStackEntryToAST(stacks));
                }
                AST::FunctionCall funcCall{methStack.m_Name, astArgs,
                                           methStack.desc.returnType};
                AST::MethodCall call{std::make_shared<AST>(object),
                                     std::make_shared<AST>(funcCall)};
                return call;
            }
            m_Stack.emplace_back(func);
            break;
        }
        // Process opcode for virtual method calls
        case OPCODE_INVOKEVIRTUAL: {
            MethodRefStack methStack =
                GetMethodRefFromIndex(m_ClassFile, instruction.Op1.value);
            std::vector<StackEntry> args;
            for (auto& descArgs : methStack.desc.args) {
                args.emplace_back(PopStack());
            }

            auto object = PopStack();
            std::string name = "";
            if (object.isClass()) {
                name = object.getClassValue().m_ClassName;
            } else if (object.isField()) {
                name = object.getFieldValue().field_name;
            }

            std::string funcName = name + "." + methStack.m_Name;
            StackEntry::Function func{funcName, args,
                                      methStack.desc.returnType};
            if (methStack.desc.returnType.kind == Type::Void) {
                auto obj = ConvertStackEntryToAST(object);
                std::vector<ASTPtr> astArgs;
                for (auto& stacks : args) {
                    astArgs.emplace_back(ConvertStackEntryToAST(stacks));
                }
                AST::FunctionCall funcCall{methStack.m_Name, astArgs,
                                           methStack.desc.returnType};
                AST::MethodCall call{std::make_shared<AST>(obj),
                                     std::make_shared<AST>(funcCall)};
                return call;
            }
            m_Stack.emplace_back(func);
            break;
        }
        // Getters and setters for static and instance fields. I.e. obj.field =
        // value;
        case OPCODE_GETSTATIC: {
            auto fieldRefStack =
                GetFieldRefFromIndex(m_ClassFile, instruction.Op1.value);
            StackEntry::Class classStack{fieldRefStack.m_Class};

            StackEntry::Field field{std::make_shared<StackEntry>(classStack),
                                    fieldRefStack.m_Name, fieldRefStack.m_type};
            m_Stack.emplace_back(field);
            break;
        }
        case OPCODE_GETFIELD: {
            auto fieldRefStack =
                GetFieldRefFromIndex(m_ClassFile, instruction.Op1.value);
            auto obj = PopStack();
            if (obj.isIdentifier()) {
                StackEntry::Field field{std::make_shared<StackEntry>(obj),
                                        fieldRefStack.m_Name,
                                        fieldRefStack.m_type};
                m_Stack.emplace_back(field);
            }
            break;
        }

        case OPCODE_PUTSTATIC: {
            auto val = PopStack();
            FieldRefStack fieldRefStack =
                GetFieldRefFromIndex(m_ClassFile, instruction.Op1.value);
            AST::Object obj{fieldRefStack.m_Class};
            AST::FieldAssignment fieldAssign{
                std::make_shared<AST>(obj), fieldRefStack.m_Name,
                fieldRefStack.m_type, ConvertStackEntryToAST(val)};
            return fieldAssign;
        }
        case OPCODE_PUTFIELD: {
            auto val = PopStack();
            auto obj = PopStack();
            FieldRefStack fieldRefStack =
                GetFieldRefFromIndex(m_ClassFile, instruction.Op1.value);

            if (m_InsideInit) {
                // m_Fie
            }
            // AST::Object obj{fieldRefStack.m_Class};
            AST::FieldAssignment fieldAssign{
                ConvertStackEntryToAST(obj), fieldRefStack.m_Name,
                fieldRefStack.m_type, ConvertStackEntryToAST(val)};
            return fieldAssign;
        }
        // Process opcodes for Array operations
        // Process opcode for primitive array
        case OPCODE_NEWARRAY: {
            Type ty = getType(instruction.Op1.value);

            auto count = std::make_shared<StackEntry>(PopStack());
            std::vector<StackEntry> arr;
            m_Stack.emplace_back(StackEntry::Array{ty, count, arr});

            break;
        }
        // Process opcode for object arrays
        case OPCODE_ANEWARRAY: {
            auto ty = FieldDescriptor::newDescriptor(
                          GetConstantClassFromClass(m_ClassFile,
                                                    instruction.Op1.value))
                          .m_Ty;
            auto count = std::make_shared<StackEntry>(PopStack());
            std::vector<StackEntry> arr;
            m_Stack.emplace_back(StackEntry::Array{ty, count, arr});
            break;
        }
        case OPCODE_MULTIANEWARRAY: {
            break;
        }
        case OPCODE_ARRAYLENGTH: {
            StackEntry val = PopStack();
            StackEntry::UnaryOperation unaryOp{
                UnaryOp::ArrayLength, std::make_shared<StackEntry>(val)};
            m_Stack.emplace_back(unaryOp);
            break;
        }
        // No operation
        case OPCODE_NOP: {
            break;
        }
        // pop value from stack
        case OPCODE_POP: {
            auto value = m_Stack.back();
            m_Stack.pop_back();
            return ConvertStackEntryToAST(value);
        }
        // pop 2 values from stack
        case OPCODE_POP2: {
            auto val1 = PopStack();
            if (val1.ty().kind == Type::Long ||
                val1.ty().kind == Type::Double) {
            } else {
                return ConvertStackEntryToAST(PopStack());
            }
            return ConvertStackEntryToAST(val1);
        }
        // Process opcode for incrementing variable
        case OPCODE_IINC: {
            if (m_LocalVariables.size() <= 0) {
                break;
            } else {
                TD_DECOMP_ERROR("localvar size: {0}", m_LocalVariables.size());
                int localIdx = ((instruction.Op2.value >> 8) & 0xFF);
                int32_t amount = instruction.Op2.value & 0xFF;
                TD_DECOMP_ERROR("localIdx {0}", localIdx);

                if (localIdx > m_LocalVariables.size()) {
                    break;
                } else {
                    StackEntry val = m_LocalVariables.at(localIdx);

                    if (!val.isInteger()) {
                        val = StackEntry(-1);
                    }

                    if (val.isInteger()) {
                        val = StackEntry(amount);
                    }

                    if (val.isIdentifier()) {
                        if (amount == 1) {
                            AST::UnaryOperation unaryOp{
                                UnaryOp::PlusPlus, ConvertStackEntryToAST(val)};
                            ASTPtr variable = std::make_unique<AST>(
                                std::move(unaryOp));  // Identifier AST
                            return variable;
                        } else {
                            ASTPtr variable =
                                ConvertStackEntryToAST(std::move(val));
                            AST::BinaryOperation binaryOp{
                                std::move(variable), BinaryOp::Add,
                                std::make_unique<AST>(amount)};
                            AST::ReAssignment reAssign{
                                std::move(variable),
                                std::make_unique<AST>(std::move(binaryOp))};
                            return reAssign;
                        }
                    }
                }
            }
        }
        // Process opcode for branch if reference are equal
        case OPCODE_IF_ACMPEQ: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for branch if reference are not equal
        case OPCODE_IF_ACMPNE: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for branch if equal to zero
        case OPCODE_IFEQ: {
            auto cond = ConvertStackEntryToAST(PopStack());
            int64_t pos = m_CurrentPos + instruction.Op1.addr;
            std::vector<ASTPtr> then;
            std::vector<Insn>::iterator currToken = funcInstructions.begin();
            std::vector<Insn>::iterator endToken = funcInstructions.end();

            while (currToken != endToken) {
                Insn insn = *currToken++;

                std::optional<AST> ast = ReadInstruction(funct, insn);
                if (ast.has_value()) {
                    then.emplace_back(std::make_unique<AST>(std::move(*ast)));
                } else {
                    continue;
                }

                if (m_CurrentPos <= pos) {
                    break;
                }
            }

            if (then.size() == 1) {
                if (then[0]
                        ->isIfStatement()) {  // Check if AST is an If condition
                    auto ifAst = then[0]->getIfNode();
                    auto this_cond = ifAst.cond;
                    auto this_then = ifAst.then;
                    then = this_then;
                    AST::BinaryOperation binOp{cond, BinaryOp::LogicalAnd,
                                               this_cond};
                    cond = std::make_shared<AST>(binOp);
                    // cond = std::make_shared<AST>(AST::BinaryOperation(cond,
                    // BinaryOp::LogicalAnd, ifAst->cond));
                }
            }
            AST::If ifStatement{cond, then};
            return std::make_shared<AST>(ifStatement);
        }
        // Process opcode for branch if greater than or equal to zero
        case OPCODE_IFGE: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for branch if greater than zero
        case OPCODE_IFGT: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for branch if less than or equal to zero
        case OPCODE_IFLE: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for branch if less than zero
        case OPCODE_IFLT: {
            // TODO: Not Implemented
            break;
        }
        // opcode for branch if not equal to zero
        case OPCODE_IFNE: {
            auto raw_cond = PopStack();
            int64_t pos = m_CurrentPos + instruction.Op1.addr;
            std::vector<ASTPtr> then;
            std::vector<Insn>::iterator currToken = funcInstructions.begin();
            std::vector<Insn>::iterator endToken = funcInstructions.end();

            while (currToken != endToken) {
                Insn insn = *currToken++;

                std::optional<AST> ast = ReadInstruction(funct, insn);
                if (ast.has_value()) {
                    then.emplace_back(std::make_unique<AST>(std::move(*ast)));
                } else {
                    continue;
                }
                if (m_CurrentPos <= pos) {
                    break;
                }
            }

            ASTPtr cond;
            if (then.size() != 1) {
                AST::UnaryOperation unOp{UnaryOp::Negate,
                                         ConvertStackEntryToAST(raw_cond)};
                cond = std::make_shared<AST>(unOp);
            } else {
                AST first_then = then[0];
                if (first_then.isIfStatement()) {
                    auto this_if = first_then.getIfNode();
                    then = this_if.then;
                    AST::BinaryOperation binOp{ConvertStackEntryToAST(raw_cond),
                                               BinaryOp::LogicalOr,
                                               std::move(this_if.cond)};
                    cond = std::make_unique<AST>(binOp);
                } else {
                    AST::UnaryOperation unOp{UnaryOp::Negate,
                                             ConvertStackEntryToAST(raw_cond)};
                    cond = std::make_unique<AST>(unOp);
                }
            }
            AST::If ifStatement{cond, then};
            return std::make_shared<AST>(ifStatement);
        }
        // Process opcode for branch if equal, val1 == val2
        case OPCODE_IF_ICMPEQ: {
            StackEntry val2 = PopStack();
            StackEntry val1 = PopStack();
            AST::BinaryOperation binaryOp{ConvertStackEntryToAST(val1),
                                          BinaryOp::NotEqual,
                                          ConvertStackEntryToAST(val2)};
            ASTPtr cond = std::make_shared<AST>(binaryOp);
            int64_t pos = m_CurrentPos + instruction.Op1.addr;
            std::vector<ASTPtr> then;

            std::vector<Insn>::iterator currToken = funcInstructions.begin();
            std::vector<Insn>::iterator endToken = funcInstructions.end();

            while (currToken != endToken) {
                Insn insn = *currToken++;

                std::optional<AST> ast = ReadInstruction(funct, insn);
                if (ast.has_value()) {
                    then.emplace_back(std::make_unique<AST>(std::move(*ast)));
                } else {
                    continue;
                }
                if (m_CurrentPos <= pos) {
                    break;
                }
            }

            if (then.size() == 1) {
                if (then[0]
                        ->isIfStatement()) {  // Check if AST is an If condition
                    auto ifAst = then[0]->getIfNode();
                    auto this_cond = ifAst.cond;
                    auto this_then = ifAst.then;
                    then = this_then;
                    AST::BinaryOperation binOp{cond, BinaryOp::LogicalAnd,
                                               this_cond};
                    cond = std::make_shared<AST>(binOp);
                }
            }

            AST::If ifStatement{cond, then};
            return std::make_shared<AST>(ifStatement);
        }
        // Process opcode for branch if Not equal, val1 != val2
        case OPCODE_IF_ICMPNE: {
            StackEntry val2 = PopStack();
            StackEntry val1 = PopStack();
            AST::BinaryOperation binaryOp{ConvertStackEntryToAST(val1),
                                          BinaryOp::Equal,
                                          ConvertStackEntryToAST(val2)};
            ASTPtr cond = std::make_shared<AST>(binaryOp);
            int64_t pos = m_CurrentPos + instruction.Op1.addr;
            std::vector<ASTPtr> then;

            std::vector<Insn>::iterator currToken = funcInstructions.begin();
            std::vector<Insn>::iterator endToken = funcInstructions.end();

            while (currToken != endToken) {
                Insn insn = *currToken++;

                std::optional<AST> ast = ReadInstruction(funct, insn);
                if (ast.has_value()) {
                    then.emplace_back(std::make_unique<AST>(std::move(*ast)));
                } else {
                    continue;
                }
                if (m_CurrentPos <= pos) {
                    break;
                }
            }

            if (then.size() == 1) {
                if (then[0]
                        ->isIfStatement()) {  // Check if AST is an If condition
                    auto ifAst = then[0]->getIfNode();
                    auto this_cond = ifAst.cond;
                    auto this_then = ifAst.then;
                    then = this_then;
                    AST::BinaryOperation binOp{cond, BinaryOp::LogicalAnd,
                                               this_cond};
                    cond = std::make_shared<AST>(binOp);
                }
            }

            AST::If ifStatement{cond, then};
            return std::make_shared<AST>(ifStatement);
        }
        // Process opcode for branch if greater than equal to, val1 >= val2
        case OPCODE_IF_ICMPGE: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for branch if greater than, val1 > val2
        case OPCODE_IF_ICMPGT: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for branch if less than or equal to, val1 <= val2
        case OPCODE_IF_ICMPLE: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for branch if less than, val1 < val2
        case OPCODE_IF_ICMPLT: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for branch if not null
        case OPCODE_IFNONNULL: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for branch if null
        case OPCODE_IFNULL: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for branch always instrucion
        case OPCODE_GOTO: {
            int targetBlockID = instruction.Op1.addr;
            auto main_block =
                funct.GetFunctionCFG()->GetBlockByInsn(instruction);
            if (main_block == nullptr) {
                break;
            }
            size_t currentBlockID = main_block->m_ID;
            if (IsBackEdge(instruction, m_Dominators, currentBlockID)) {
                std::cout << "IS BACK EDGE" << std::endl;
                // auto loopNode = BuildLoopAST(insn, domTree);
            } else {
                std::cout << "NOT A BACK EDGE" << std::endl;
            }
            // TODO: Not Implemented
            break;
        }
        // Process opcode for branch always (wide index) instruction
        case OPCODE_GOTO_W: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for jump subroutine
        case OPCODE_JSR: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for jump subroutine (wide index)
        case OPCODE_JSR_W: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for return from subroutine
        case OPCODE_RET: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for convert int to byte
        case OPCODE_I2B: {
            Cast(Type::Byte);
            break;
        }
        // Process opcode for convert int to char
        case OPCODE_I2C: {
            Cast(Type::Char);
            break;
        }
        // Process opcode for convert into to short
        case OPCODE_I2S: {
            Cast(Type::Short);
            break;
        }
        // Process opcodes for converting to double
        case OPCODE_I2D:
        case OPCODE_F2D:
        case OPCODE_L2D: {
            Cast(Type::Double);
            break;
        }
        // Process opcodes for converting to long
        case OPCODE_I2L:
        case OPCODE_F2L:
        case OPCODE_D2L: {
            Cast(Type::Long);
            break;
        }
        // Process opcodes for converting to int
        case OPCODE_F2I:
        case OPCODE_D2I:
        case OPCODE_L2I: {
            Cast(Type::Int);
            break;
        }
        // Process opcodes for converting to float
        case OPCODE_I2F:
        case OPCODE_D2F:
        case OPCODE_L2F: {
            Cast(Type::Float);
            break;
        }
        // Process opcode for creating new objects
        case OPCODE_NEW: {
            auto obj =
                GetConstantClassFromClass(m_ClassFile, instruction.Op1.value);
            StackEntry::New newObj{obj};
            m_Stack.emplace_back(StackEntry(newObj));
            break;
        }
        // Process opcode for duplicating top stack value
        case OPCODE_DUP: {
            StackEntry val = PopStack();
            if (val.isArray() || val.isNew()) {
                m_Stack.emplace_back(val);
            } else {
                m_Stack.emplace_back(val);
                m_Stack.emplace_back(val);
            }
            break;
        }
        // Process opcode for duplicating the top value and inserting it below
        // the second value
        case OPCODE_DUP_X1: {
            StackEntry val1 = PopStack();
            StackEntry val2 = PopStack();
            m_Stack.emplace_back(StackEntry(val1));
            m_Stack.emplace_back(StackEntry(val2));
            m_Stack.emplace_back(StackEntry(val1));
            break;
        }
        // Process opcode for Duplicating the top value and inserting it below
        // the third value
        case OPCODE_DUP_X2: {
            StackEntry val1 = PopStack();
            StackEntry val2 = PopStack();
            StackEntry val3 = PopStack();
            m_Stack.emplace_back(StackEntry(val1));
            m_Stack.emplace_back(StackEntry(val3));
            m_Stack.emplace_back(StackEntry(val2));
            m_Stack.emplace_back(StackEntry(val1));
            break;
        }
        // Process opcode for duplicating the top two values
        case OPCODE_DUP2: {
            StackEntry val1 = PopStack();
            StackEntry val2 = PopStack();
            m_Stack.emplace_back(StackEntry(val2));
            m_Stack.emplace_back(StackEntry(val1));
            m_Stack.emplace_back(StackEntry(val2));
            m_Stack.emplace_back(StackEntry(val1));
            break;
        }
        // Process opcode for duplicating the top two values and inserting them
        // below the third value
        case OPCODE_DUP2_X1: {
            StackEntry val1 = PopStack();
            StackEntry val2 = PopStack();
            StackEntry val3 = PopStack();
            m_Stack.emplace_back(StackEntry(val2));
            m_Stack.emplace_back(StackEntry(val1));
            m_Stack.emplace_back(StackEntry(val3));
            m_Stack.emplace_back(StackEntry(val2));
            m_Stack.emplace_back(StackEntry(val1));
            break;
        }
        // Inserts the duplicated top two values below the fourth value
        case OPCODE_DUP2_X2: {
            StackEntry val1 = PopStack();
            StackEntry val2 = PopStack();
            StackEntry val3 = PopStack();
            StackEntry val4 = PopStack();
            m_Stack.emplace_back(StackEntry(val2));
            m_Stack.emplace_back(StackEntry(val1));
            m_Stack.emplace_back(StackEntry(val4));
            m_Stack.emplace_back(StackEntry(val3));
            m_Stack.emplace_back(StackEntry(val2));
            m_Stack.emplace_back(StackEntry(val1));
            break;
        }
        // Swaps the top two stack values
        case OPCODE_SWAP: {
            StackEntry val1 = PopStack();
            StackEntry val2 = PopStack();
            m_Stack.emplace_back(StackEntry(val1));
            m_Stack.emplace_back(StackEntry(val2));
            break;
        }
        default:
            // Handle other instructions
            return std::nullopt;
    }

    // if(block->)

    return std::nullopt;
}

ASTPtr Codegen::ConvertCFGToAST(
    std::shared_ptr<BasicBlock> block,
    const std::map<int, std::shared_ptr<BasicBlock>>& cfgBlocks,
    const std::map<uint32_t, uint32_t>& dominators) {
    ASTPtr forLoop = CreateForLoop(block, cfgBlocks, dominators);
    if (forLoop) {
        // ASTPtr astPtr = std::make_shared<AST>(std::move(forLoop));
        return forLoop;
    } else {
        // If it's not a for loop, convert the current block's instructions
        std::vector<ASTPtr> instructions;
        for (const auto& insn : block->GetInstructions()) {
            instructions.push_back(ConvertInstructionToAST(
                insn));  // You'll need a function like this
        }
        if (instructions.empty()) {
            return nullptr;  // Or some representation of an empty block
        } else if (instructions.size() == 1) {
            return instructions[0];
        } else {
            return std::make_shared<AST>(AST::StatementList{
                instructions});  // Assuming you have an AST::Block type
        }
    }
}

ASTPtr Codegen::ConvertInstructionToAST(const Insn& instruction) {
    switch (instruction.opcode) {
        case OPCODE_BIPUSH: {  // single byte integer push
            int32_t value = static_cast<int32_t>(instruction.Op1.value);
            m_Stack.emplace_back(value);
            break;
        }
        case OPCODE_SIPUSH: {  // short integer push
            int32_t value = static_cast<int32_t>(instruction.Op1.value);
            m_Stack.emplace_back(value);
            break;
        }
        case OPCODE_LDC:  // Load constant
        case OPCODE_LDC_W: {
            ConstPoolInfo cpi = m_ClassFile.m_ConstantPool.m_ConstPoolInfo.at(
                instruction.Op1.value - 1);
            if (cpi.Tag == Tags::String) {
                // If constant is a string, retrieve the actual String and push
                // it onto m_Stack
                auto str = cpi.GetAs<StringInfo>();
                // static_cast<StringInfo*>(cpi.Info.get());
                std::string constStr =
                    GetConstantUTF8FromClass(m_ClassFile, str->stringIndex);
                m_Stack.emplace_back(StackEntry::String{constStr});
            } else if (cpi.Tag == Tags::Int) {
                int32_t value = static_cast<int32_t>(instruction.Op1.value);
                m_Stack.emplace_back(value);
            } else if (cpi.Tag == Tags::Float) {
                float value = static_cast<float>(instruction.Op1.value);
                m_Stack.emplace_back(value);
            }
            break;
        }

            // case OPCODE_LDC2_W: {
            // }

        case OPCODE_ACONST_NULL:  // NULL reference
            m_Stack.emplace_back(StackEntry::Null());
            return std::make_shared<AST>(std::monostate{});
        // ICONST pushes predefined integer constants onto stack
        case OPCODE_ICONST_M1: {
            m_Stack.emplace_back(-1);
            return std::make_shared<AST>(-1);
        }
        case OPCODE_ICONST_0: {
            m_Stack.emplace_back(0);
            return std::make_shared<AST>(0);
        }
        case OPCODE_ICONST_1: {
            m_Stack.emplace_back(1);
            return std::make_shared<AST>(1);
        }
        case OPCODE_ICONST_2: {
            m_Stack.emplace_back(2);
            return std::make_shared<AST>(2);
        }
        case OPCODE_ICONST_3: {
            m_Stack.emplace_back(3);
            return std::make_shared<AST>(3);
        }
        case OPCODE_ICONST_4: {
            m_Stack.emplace_back(4);
            return std::make_shared<AST>(4);
        }
        case OPCODE_ICONST_5: {
            m_Stack.emplace_back(5);
            return std::make_shared<AST>(5);
        }
        // FCONST pushes predefined float constants onto stack
        case OPCODE_FCONST_0:
            m_Stack.emplace_back(0.0f);
            break;

        case OPCODE_FCONST_1:
            m_Stack.emplace_back(1.0f);
            break;

        case OPCODE_FCONST_2:
            m_Stack.emplace_back(2.0f);
            break;

        case OPCODE_DCONST_0:
            m_Stack.emplace_back(0.0);
            break;

        case OPCODE_DCONST_1:
            m_Stack.emplace_back(1.0);
            break;
        // LCONST pushes predifned long constants onto stack
        case OPCODE_LCONST_0:
            m_Stack.emplace_back(static_cast<int64_t>(0L));
            break;

        case OPCODE_LCONST_1:
            m_Stack.emplace_back(static_cast<int64_t>(1L));
            break;
        // Loads local variables onto stack
        case OPCODE_ALOAD:
        case OPCODE_FLOAD:
        case OPCODE_DLOAD:
        case OPCODE_ILOAD:
        case OPCODE_LLOAD: {
            return LoadLocalAST(
                instruction.Op1.value);  // Op1.value is variable index in the
                                         // function's local variable array
        }
        case OPCODE_ALOAD_0:
        case OPCODE_FLOAD_0:
        case OPCODE_DLOAD_0:
        case OPCODE_ILOAD_0:
        case OPCODE_LLOAD_0: {
            return LoadLocalAST(0);
        }
        case OPCODE_ALOAD_1:
        case OPCODE_FLOAD_1:
        case OPCODE_DLOAD_1:
        case OPCODE_ILOAD_1:
        case OPCODE_LLOAD_1: {
            return LoadLocalAST(1);
        }
        case OPCODE_ALOAD_2:
        case OPCODE_FLOAD_2:
        case OPCODE_DLOAD_2:
        case OPCODE_ILOAD_2:
        case OPCODE_LLOAD_2: {
            return LoadLocalAST(2);
        }
        case OPCODE_ALOAD_3:
        case OPCODE_FLOAD_3:
        case OPCODE_DLOAD_3:
        case OPCODE_ILOAD_3:
        case OPCODE_LLOAD_3: {
            return LoadLocalAST(3);
        }
        case OPCODE_ISTORE:
        case OPCODE_ASTORE:
        case OPCODE_DSTORE:
        case OPCODE_LSTORE:
        case OPCODE_FSTORE: {
            ASTPtr store = std::make_shared<AST>(
                std::move(Store(instruction.Op1.value).value()));
            return store;
        }
        case OPCODE_ISTORE_0:
        case OPCODE_ASTORE_0:
        case OPCODE_DSTORE_0:
        case OPCODE_LSTORE_0:
        case OPCODE_FSTORE_0: {
            ASTPtr store = std::make_shared<AST>(std::move(Store(0).value()));
            return store;
        }
        case OPCODE_ISTORE_1:
        case OPCODE_ASTORE_1:
        case OPCODE_DSTORE_1:
        case OPCODE_LSTORE_1:
        case OPCODE_FSTORE_1: {
            ASTPtr store = std::make_shared<AST>(std::move(Store(1).value()));
            return store;
        }
        case OPCODE_ISTORE_2:
        case OPCODE_ASTORE_2:
        case OPCODE_DSTORE_2:
        case OPCODE_LSTORE_2:
        case OPCODE_FSTORE_2: {
            ASTPtr store = std::make_shared<AST>(std::move(Store(2).value()));
            return store;
        }
        case OPCODE_ISTORE_3:
        case OPCODE_ASTORE_3:
        case OPCODE_DSTORE_3:
        case OPCODE_LSTORE_3:
        case OPCODE_FSTORE_3: {
            ASTPtr store = std::make_shared<AST>(std::move(Store(3).value()));
            return store;
        }
        // Array storage operations
        case OPCODE_AASTORE:    // Reference Array
        case OPCODE_BASTORE:    // Byte or boolean
        case OPCODE_CASTORE:    // char
        case OPCODE_DASTORE:    // double
        case OPCODE_FASTORE:    // float
        case OPCODE_IASTORE:    // int
        case OPCODE_LASTORE:    // long
        case OPCODE_SASTORE: {  // short
            StackEntry val = PopStack();
            StackEntry index = PopStack();
            StackEntry array = PopStack();
            if (array.isArray()) {
                StackEntry::Array arr = array.getArrayValue();
                arr.elements.emplace_back(val);
                m_Stack.emplace_back(
                    StackEntry::Array{arr.type, arr.length, arr.elements});
            } else {
                // if array is not explicitly an array, assume it is an object
                // array.
                auto entry = ConvertStackEntryToAST(array);
                AST::ArrayIndex arrIndex{entry, ConvertStackEntryToAST(index),
                                         array.ty()};
                AST::ReAssignment reAssign{std::make_shared<AST>(arrIndex),
                                           ConvertStackEntryToAST(val)};
                ASTPtr reAssignment =
                    std::make_shared<AST>(std::move(reAssign));
                return reAssignment;
            }
            break;
        }
        // arithmetic operations opcodes, pushes the respective operation onto
        // the stack
        case OPCODE_IADD:
        case OPCODE_FADD:
        case OPCODE_DADD:
        case OPCODE_LADD:
            BinaryOper(BinaryOp::Add);
            break;
        case OPCODE_ISUB:
        case OPCODE_FSUB:
        case OPCODE_DSUB:
        case OPCODE_LSUB:
            BinaryOper(BinaryOp::Sub);
            break;
        case OPCODE_IMUL:
        case OPCODE_FMUL:
        case OPCODE_DMUL:
        case OPCODE_LMUL:
            BinaryOper(BinaryOp::Mul);
            break;
        case OPCODE_IDIV:
        case OPCODE_FDIV:
        case OPCODE_DDIV:
        case OPCODE_LDIV:
            BinaryOper(BinaryOp::Div);
            break;
        case OPCODE_IREM:
        case OPCODE_FREM:
        case OPCODE_DREM:
        case OPCODE_LREM:
            BinaryOper(BinaryOp::Rem);
            break;
        // Bitwise shift operations
        // Shift left a << b
        case OPCODE_ISHL:
        case OPCODE_LSHL:
            BinaryOper(BinaryOp::Shl);
            break;
        // Shift right a >> b
        case OPCODE_ISHR:
        case OPCODE_LSHR:
            BinaryOper(BinaryOp::Shr);
            break;
        // Unsigned shift right a >>> b
        case OPCODE_IUSHR:
        case OPCODE_LUSHR:
            BinaryOper(BinaryOp::UShr);
            break;
        // Bitwise logical operations
        case OPCODE_IXOR:
        case OPCODE_LXOR:
            BinaryOper(BinaryOp::Xor);
            break;
        case OPCODE_IAND:
        case OPCODE_LAND:
            BinaryOper(BinaryOp::And);
            break;
        case OPCODE_IOR:
        case OPCODE_LOR:
            BinaryOper(BinaryOp::Or);
            break;
        case OPCODE_FCMPG:
        case OPCODE_DCMPG:
            BinaryOper(BinaryOp::GreaterThan);
            break;
        case OPCODE_FCMPL:
        case OPCODE_DCMPL:
            BinaryOper(BinaryOp::LessThan);
            break;
        case OPCODE_INEG:
        case OPCODE_FNEG:
        case OPCODE_DNEG:
        case OPCODE_LNEG: {
            StackEntry val = PopStack();
            m_Stack.emplace_back(StackEntry::UnaryOperation{
                UnaryOp::Neg, std::make_unique<StackEntry>(std::move(val))});
            break;
        }
        // Process opcode for type checking
        case OPCODE_INSTANCEOF: {
            StackEntry obj1 = PopStack();  // pop object reference
            std::string obj2 = GetConstantClassFromClass(
                m_ClassFile,
                instruction.Op1
                    .value);  // Retrieve class name from constant pool
            StackEntry::Class obj2Class{obj2};
            StackEntry::BinaryOperation binOp{
                std::make_shared<StackEntry>(obj1), BinaryOp::InstanceOf,
                std::make_shared<StackEntry>(obj2Class)};
            m_Stack.emplace_back(binOp);
            break;
        }
        case OPCODE_LCMP: {
            break;
        }
        // Process opcode for return statements
        case OPCODE_RETURN: {
            AST::Return retStmt{nullptr};
            ASTPtr returnAST = std::make_shared<AST>(std::move(retStmt));

            return returnAST;
        }
        case OPCODE_ARETURN:
        case OPCODE_IRETURN:
        case OPCODE_FRETURN:
        case OPCODE_DRETURN:
        case OPCODE_LRETURN: {
            AST::Return ret{ConvertStackEntryToAST(PopStack())};
            ASTPtr returnAST = std::make_shared<AST>(std::move(ret));

            return returnAST;
        }
        // Process opcode for dynamic method calls
        case OPCODE_INVOKEDYNAMIC: {
            auto methStack = GetDynamicInvokeRefFromIndex(
                m_ClassFile, instruction.Op1.value);

            if (methStack.m_Class == "StringConcatFactory" &&
                methStack.m_Name == "makeConcatWithConstants") {
                auto args = methStack.unparsed_args;
                StringConcatFactory(args);
                // return nullptr;
                break;
            }

            if (methStack.desc.args.empty()) {
                break;
            }

            std::vector<StackEntry> args;

            for (int i = 0; i < methStack.desc.args.size(); i++) {
                args.emplace_back(PopStack());
            }

            std::string name = methStack.m_Class + "." + methStack.m_Name;

            StackEntry::Function func{getLastClassNamePart(name), args,
                                      methStack.desc.returnType};
            m_Stack.emplace_back(func);
            break;
        }
        // Process opcodes for interface method calls
        case OPCODE_INVOKEINTERFACE: {
            auto methStack =
                GetMethodRefFromIndex(m_ClassFile, instruction.Op1.value);

            if (methStack.desc.args.empty()) {
                break;
            }

            std::vector<StackEntry> args;

            for (int i = 0; i < methStack.desc.args.size(); i++) {
                args.emplace_back(PopStack());
            }

            std::string name = methStack.m_Class + "." + methStack.m_Name;

            StackEntry::Function func{getLastClassNamePart(name), args,
                                      methStack.desc.returnType};
            m_Stack.emplace_back(func);
            break;
        }
        // Process opcode for constructors and private method calls
        case OPCODE_INVOKESPECIAL: {
            MethodRefStack methStack =
                GetMethodRefFromIndex(m_ClassFile, instruction.Op1.value);
            std::vector<StackEntry> argsStack;
            std::vector<ASTPtr> argsAST;
            for (auto& descArgs : methStack.desc.args) {
                argsStack.emplace_back(PopStack());
                // Convert each argument StackEntry to an ASTPtr
                // argsAST.push_back(ConvertStackEntryToAST(argsStack.back()));
            }

            auto objStackEntry = PopStack();
            std::string methodName = "";
            Type returnType = Type::Void;  // Default return type

            if (methStack.m_Name == "<init>" ||
                methStack.m_Name == "<clinit>") {
                if (objStackEntry.isClass()) {
                    methodName = objStackEntry.getClassValue().m_ClassName;
                    returnType = Type::ClassNameType(
                        methStack
                            .m_Class);  // Constructor returns the object type
                    return std::make_shared<AST>(
                        AST::Object{methodName});  // Represent constructor call
                                                   // as object creation? This
                                                   // depends on your semantics
                } else if (objStackEntry.isIdentifier()) {
                    methodName = objStackEntry.getIdentifierValue().name;
                    returnType = Type::ClassNameType(
                        methStack
                            .m_Class);  // Constructor returns the object type
                    // return std::make_shared<AST>(AST::Object{methodName}); //
                    // Represent constructor call as object creation? This
                    // depends on your semantics
                }
            } else {
                if (objStackEntry.isClass()) {
                    methodName = objStackEntry.getClassValue().m_ClassName +
                                 "." + methStack.m_Name;
                    returnType = Type::ClassNameType(
                        methStack
                            .m_Class);  // Assuming method returns the class
                                        // type for now - adjust based on
                                        // methStack.desc.return_type
                }
            }

            if (!methodName.empty()) {
                return std::make_shared<AST>(
                    AST::FunctionCall{methodName, argsAST, returnType});
            } else {
                // Handle cases where method name couldn't be determined
                std::cerr << "Warning: Could not determine method name for "
                             "INVOKESPECIAL"
                          << std::endl;
                return nullptr;  // Or some other appropriate error handling
            }
            break;
        }
        // Process opcode for static method calls
        case OPCODE_INVOKESTATIC: {
            MethodRefStack methStack =
                GetMethodRefFromIndex(m_ClassFile, instruction.Op1.value);
            std::vector<StackEntry> args;
            for (auto& descArgs : methStack.desc.args) {
                args.emplace_back(PopStack());
            }

            std::string funcName = methStack.m_Class + "." + methStack.m_Name;
            StackEntry::Function func{funcName, args,
                                      methStack.desc.returnType};

            if (methStack.desc.returnType.kind == Type::Void) {
                AST::Object object{methStack.m_Class};
                std::vector<ASTPtr> astArgs;
                for (auto& stacks : args) {
                    astArgs.emplace_back(ConvertStackEntryToAST(stacks));
                }
                AST::FunctionCall funcCall{methStack.m_Name, astArgs,
                                           methStack.desc.returnType};
                AST::MethodCall call{std::make_shared<AST>(object),
                                     std::make_shared<AST>(funcCall)};
                ASTPtr methodCall = std::make_shared<AST>(std::move(call));
                return methodCall;
            }
            m_Stack.emplace_back(func);
            break;
        }
        // Process opcode for virtual method calls
        case OPCODE_INVOKEVIRTUAL: {
            MethodRefStack methStack =
                GetMethodRefFromIndex(m_ClassFile, instruction.Op1.value);
            std::vector<StackEntry> args;
            for (auto& descArgs : methStack.desc.args) {
                args.emplace_back(PopStack());
            }

            auto object = PopStack();
            std::string name = "";
            if (object.isClass()) {
                name = object.getClassValue().m_ClassName;
            } else if (object.isField()) {
                name = object.getFieldValue().field_name;
            }

            std::string funcName = name + "." + methStack.m_Name;
            StackEntry::Function func{funcName, args,
                                      methStack.desc.returnType};
            if (methStack.desc.returnType.kind == Type::Void) {
                auto obj = ConvertStackEntryToAST(object);
                std::vector<ASTPtr> astArgs;
                for (auto& stacks : args) {
                    astArgs.emplace_back(ConvertStackEntryToAST(stacks));
                }
                AST::FunctionCall funcCall{methStack.m_Name, astArgs,
                                           methStack.desc.returnType};
                AST::MethodCall call{std::make_shared<AST>(obj),
                                     std::make_shared<AST>(funcCall)};
                ASTPtr methodCall = std::make_shared<AST>(std::move(call));

                return methodCall;
            }
            m_Stack.emplace_back(func);
            break;
        }
        // Getters and setters for static and instance fields. I.e. obj.field =
        // value;
        case OPCODE_GETSTATIC: {
            auto fieldRefStack =
                GetFieldRefFromIndex(m_ClassFile, instruction.Op1.value);
            StackEntry::Class classStack{fieldRefStack.m_Class};

            StackEntry::Field field{std::make_shared<StackEntry>(classStack),
                                    fieldRefStack.m_Name, fieldRefStack.m_type};
            m_Stack.emplace_back(field);
            break;
        }
        case OPCODE_GETFIELD: {
            auto fieldRefStack =
                GetFieldRefFromIndex(m_ClassFile, instruction.Op1.value);
            auto obj = PopStack();
            if (obj.isIdentifier()) {
                StackEntry::Field field{std::make_shared<StackEntry>(obj),
                                        fieldRefStack.m_Name,
                                        fieldRefStack.m_type};
                m_Stack.emplace_back(field);
            }
            break;
        }

        case OPCODE_PUTSTATIC: {
            auto val = PopStack();
            FieldRefStack fieldRefStack =
                GetFieldRefFromIndex(m_ClassFile, instruction.Op1.value);
            AST::Object obj{fieldRefStack.m_Class};
            AST::FieldAssignment fieldAssign{
                std::make_shared<AST>(obj), fieldRefStack.m_Name,
                fieldRefStack.m_type, ConvertStackEntryToAST(val)};
            ASTPtr fieldAssignment =
                std::make_shared<AST>(std::move(fieldAssign));
            return fieldAssignment;
        }
        case OPCODE_PUTFIELD: {
            auto val = PopStack();
            auto obj = PopStack();
            FieldRefStack fieldRefStack =
                GetFieldRefFromIndex(m_ClassFile, instruction.Op1.value);

            if (m_InsideInit) {
                // m_Fie
            }
            // AST::Object obj{fieldRefStack.m_Class};
            AST::FieldAssignment fieldAssign{
                ConvertStackEntryToAST(obj), fieldRefStack.m_Name,
                fieldRefStack.m_type, ConvertStackEntryToAST(val)};
            ASTPtr fieldAssignment =
                std::make_shared<AST>(std::move(fieldAssign));
            return fieldAssignment;
        }
        // Process opcodes for Array operations
        // Process opcode for primitive array
        case OPCODE_NEWARRAY: {
            Type ty = getType(instruction.Op1.value);

            auto count = std::make_shared<StackEntry>(PopStack());
            std::vector<StackEntry> arr;
            m_Stack.emplace_back(StackEntry::Array{ty, count, arr});

            break;
        }
        // Process opcode for object arrays
        case OPCODE_ANEWARRAY: {
            auto ty = FieldDescriptor::newDescriptor(
                          GetConstantClassFromClass(m_ClassFile,
                                                    instruction.Op1.value))
                          .m_Ty;
            auto count = std::make_shared<StackEntry>(PopStack());
            std::vector<StackEntry> arr;
            m_Stack.emplace_back(StackEntry::Array{ty, count, arr});
            break;
        }
        case OPCODE_MULTIANEWARRAY: {
            break;
        }
        case OPCODE_ARRAYLENGTH: {
            StackEntry val = PopStack();
            StackEntry::UnaryOperation unaryOp{
                UnaryOp::ArrayLength, std::make_shared<StackEntry>(val)};
            m_Stack.emplace_back(unaryOp);
            break;
        }
        // No operation
        case OPCODE_NOP: {
            break;
        }
        // pop value from stack
        case OPCODE_POP: {
            auto value = m_Stack.back();
            m_Stack.pop_back();
            return ConvertStackEntryToAST(value);
        }
        // pop 2 values from stack
        case OPCODE_POP2: {
            auto val1 = PopStack();
            if (val1.ty().kind == Type::Long ||
                val1.ty().kind == Type::Double) {
            } else {
                return ConvertStackEntryToAST(PopStack());
            }
            return ConvertStackEntryToAST(val1);
        }
        // Process opcode for incrementing variable
        case OPCODE_IINC: {
            TD_DECOMP_INFO("IINC");
            ;
            int localIdx = (instruction.Op2.value >> 8) & 0xFF;
            int32_t amount = instruction.Op2.value & 0xFF;
            StackEntry val = m_LocalVariables.at(localIdx);

            if (!val.isInteger()) {
                val = StackEntry(-1);
            }

            if (val.isInteger()) {
                val = StackEntry(amount);
            }

            if (val.isIdentifier()) {
                TD_DECOMP_INFO("IDENTIFIER");
                if (amount == 1) {
                    AST::UnaryOperation unaryOp{UnaryOp::PlusPlus,
                                                ConvertStackEntryToAST(val)};
                    ASTPtr variable = std::make_unique<AST>(
                        std::move(unaryOp));  // Identifier AST
                    return variable;
                } else {
                    ASTPtr variable = ConvertStackEntryToAST(std::move(val));
                    AST::BinaryOperation binaryOp{
                        std::move(variable), BinaryOp::Add,
                        std::make_unique<AST>(amount)};
                    AST::ReAssignment reAssign{
                        std::move(variable),
                        std::make_unique<AST>(std::move(binaryOp))};
                    ASTPtr reAssignment =
                        std::make_unique<AST>(std::move(reAssign));
                    return reAssignment;
                }
            }
        }
        // Process opcode for branch if reference are equal
        case OPCODE_IF_ACMPEQ: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for branch if reference are not equal
        case OPCODE_IF_ACMPNE: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for branch if equal to zero
        case OPCODE_IFEQ: {
            auto cond = ConvertStackEntryToAST(PopStack());
            int64_t pos = m_CurrentPos + instruction.Op1.addr;
            std::vector<ASTPtr> then;
            std::vector<Insn>::iterator currToken = funcInstructions.begin();
            std::vector<Insn>::iterator endToken = funcInstructions.end();

            while (currToken != endToken) {
                Insn insn = *currToken++;

                std::optional<AST> ast = ConvertInstructionToAST(insn);
                if (ast.has_value()) {
                    then.emplace_back(std::make_unique<AST>(std::move(*ast)));
                } else {
                    continue;
                }

                if (m_CurrentPos <= pos) {
                    break;
                }
            }

            if (then.size() == 1) {
                if (then[0]
                        ->isIfStatement()) {  // Check if AST is an If condition
                    auto ifAst = then[0]->getIfNode();
                    auto this_cond = ifAst.cond;
                    auto this_then = ifAst.then;
                    then = this_then;
                    AST::BinaryOperation binOp{cond, BinaryOp::LogicalAnd,
                                               this_cond};
                    cond = std::make_shared<AST>(binOp);
                    // cond = std::make_shared<AST>(AST::BinaryOperation(cond,
                    // BinaryOp::LogicalAnd, ifAst->cond));
                }
            }
            AST::If ifStatement{cond, then};
            return std::make_shared<AST>(ifStatement);
        }
        // Process opcode for branch if greater than or equal to zero
        case OPCODE_IFGE: {
            auto cond = ConvertStackEntryToAST(PopStack());
            int64_t pos = m_CurrentPos + instruction.Op1.addr;
            std::vector<ASTPtr> then;
            std::vector<Insn>::iterator currToken = funcInstructions.begin();
            std::vector<Insn>::iterator endToken = funcInstructions.end();

            while (currToken != endToken) {
                Insn insn = *currToken++;

                std::optional<AST> ast = ConvertInstructionToAST(insn);
                if (ast.has_value()) {
                    then.emplace_back(std::make_unique<AST>(std::move(*ast)));
                } else {
                    continue;
                }

                if (m_CurrentPos <= pos) {
                    break;
                }
            }

            if (then.size() == 1) {
                if (then[0]
                        ->isIfStatement()) {  // Check if AST is an If condition
                    auto ifAst = then[0]->getIfNode();
                    auto this_cond = ifAst.cond;
                    auto this_then = ifAst.then;
                    then = this_then;
                    AST::BinaryOperation binOp{cond, BinaryOp::GreaterEqualThan,
                                               this_cond};
                    cond = std::make_shared<AST>(binOp);
                    // cond = std::make_shared<AST>(AST::BinaryOperation(cond,
                    // BinaryOp::LogicalAnd, ifAst->cond));
                }
            }
            AST::If ifStatement{cond, then};
            return std::make_shared<AST>(ifStatement);
        }
        // Process opcode for branch if greater than zero
        case OPCODE_IFGT: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for branch if less than or equal to zero
        case OPCODE_IFLE: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for branch if less than zero
        case OPCODE_IFLT: {
            // TODO: Not Implemented
            break;
        }
        // opcode for branch if not equal to zero
        case OPCODE_IFNE: {
            auto raw_cond = PopStack();
            int64_t pos = m_CurrentPos + instruction.Op1.addr;
            std::vector<ASTPtr> then;
            std::vector<Insn>::iterator currToken = funcInstructions.begin();
            std::vector<Insn>::iterator endToken = funcInstructions.end();

            while (currToken != endToken) {
                Insn insn = *currToken++;

                std::optional<AST> ast = ConvertInstructionToAST(insn);
                if (ast.has_value()) {
                    then.emplace_back(std::make_unique<AST>(std::move(*ast)));
                } else {
                    continue;
                }
                if (m_CurrentPos <= pos) {
                    break;
                }
            }

            ASTPtr cond;
            if (then.size() != 1) {
                AST::UnaryOperation unOp{UnaryOp::Negate,
                                         ConvertStackEntryToAST(raw_cond)};
                cond = std::make_shared<AST>(unOp);
            } else {
                AST first_then = then[0];
                if (first_then.isIfStatement()) {
                    auto this_if = first_then.getIfNode();
                    then = this_if.then;
                    AST::BinaryOperation binOp{ConvertStackEntryToAST(raw_cond),
                                               BinaryOp::LogicalOr,
                                               std::move(this_if.cond)};
                    cond = std::make_unique<AST>(binOp);
                } else {
                    AST::UnaryOperation unOp{UnaryOp::Negate,
                                             ConvertStackEntryToAST(raw_cond)};
                    cond = std::make_unique<AST>(unOp);
                }
            }
            AST::If ifStatement{cond, then};
            return std::make_shared<AST>(ifStatement);
        }
        // Process opcode for branch if equal, val1 == val2
        case OPCODE_IF_ICMPEQ: {
            StackEntry val2 = PopStack();
            StackEntry val1 = PopStack();
            AST::BinaryOperation binaryOp{ConvertStackEntryToAST(val1),
                                          BinaryOp::NotEqual,
                                          ConvertStackEntryToAST(val2)};
            ASTPtr cond = std::make_shared<AST>(binaryOp);
            int64_t pos = m_CurrentPos + instruction.Op1.addr;
            std::vector<ASTPtr> then;

            std::vector<Insn>::iterator currToken = funcInstructions.begin();
            std::vector<Insn>::iterator endToken = funcInstructions.end();

            while (currToken != endToken) {
                Insn insn = *currToken++;

                std::optional<AST> ast = ConvertInstructionToAST(insn);
                if (ast.has_value()) {
                    then.emplace_back(std::make_unique<AST>(std::move(*ast)));
                } else {
                    continue;
                }
                if (m_CurrentPos <= pos) {
                    break;
                }
            }

            if (then.size() == 1) {
                if (then[0]
                        ->isIfStatement()) {  // Check if AST is an If condition
                    auto ifAst = then[0]->getIfNode();
                    auto this_cond = ifAst.cond;
                    auto this_then = ifAst.then;
                    then = this_then;
                    AST::BinaryOperation binOp{cond, BinaryOp::LogicalAnd,
                                               this_cond};
                    cond = std::make_shared<AST>(binOp);
                }
            }

            AST::If ifStatement{cond, then};
            return std::make_shared<AST>(ifStatement);
        }
        // Process opcode for branch if Not equal, val1 != val2
        case OPCODE_IF_ICMPNE: {
            StackEntry val2 = PopStack();
            StackEntry val1 = PopStack();
            AST::BinaryOperation binaryOp{ConvertStackEntryToAST(val1),
                                          BinaryOp::Equal,
                                          ConvertStackEntryToAST(val2)};
            ASTPtr cond = std::make_shared<AST>(binaryOp);
            int64_t pos = m_CurrentPos + instruction.Op1.addr;
            std::vector<ASTPtr> then;

            std::vector<Insn>::iterator currToken = funcInstructions.begin();
            std::vector<Insn>::iterator endToken = funcInstructions.end();

            while (currToken != endToken) {
                Insn insn = *currToken++;

                std::optional<AST> ast = ConvertInstructionToAST(insn);
                if (ast.has_value()) {
                    then.emplace_back(std::make_unique<AST>(std::move(*ast)));
                } else {
                    continue;
                }
                if (m_CurrentPos <= pos) {
                    break;
                }
            }

            if (then.size() == 1) {
                if (then[0]
                        ->isIfStatement()) {  // Check if AST is an If condition
                    auto ifAst = then[0]->getIfNode();
                    auto this_cond = ifAst.cond;
                    auto this_then = ifAst.then;
                    then = this_then;
                    AST::BinaryOperation binOp{cond, BinaryOp::LogicalAnd,
                                               this_cond};
                    cond = std::make_shared<AST>(binOp);
                }
            }

            AST::If ifStatement{cond, then};
            return std::make_shared<AST>(ifStatement);
        }
        // Process opcode for branch if greater than equal to, val1 >= val2
        case OPCODE_IF_ICMPGE: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for branch if greater than, val1 > val2
        case OPCODE_IF_ICMPGT: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for branch if less than or equal to, val1 <= val2
        case OPCODE_IF_ICMPLE: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for branch if less than, val1 < val2
        case OPCODE_IF_ICMPLT: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for branch if not null
        case OPCODE_IFNONNULL: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for branch if null
        case OPCODE_IFNULL: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for branch always instrucion
        case OPCODE_GOTO: {
            // int targetBlockID = instruction.Op1.addr;
            // auto block = funct.GetFunctionCFG()->GetBlockByInsn(instruction);
            // size_t currentBlockID = block->m_ID;
            // if(IsBackEdge(instruction, m_Dominators, currentBlockID)) {
            //     std::cout << "IS BACK EDGE" << std::endl;
            //     // auto loopNode = BuildLoopAST(insn, domTree);
            // } else {
            //     std::cout << "NOT A BACK EDGE" << std::endl;
            // }
            // TODO: Not Implemented
            break;
        }
        // Process opcode for branch always (wide index) instruction
        case OPCODE_GOTO_W: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for jump subroutine
        case OPCODE_JSR: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for jump subroutine (wide index)
        case OPCODE_JSR_W: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for return from subroutine
        case OPCODE_RET: {
            // TODO: Not Implemented
            break;
        }
        // Process opcode for convert int to byte
        case OPCODE_I2B: {
            Cast(Type::Byte);
            break;
        }
        // Process opcode for convert int to char
        case OPCODE_I2C: {
            Cast(Type::Char);
            break;
        }
        // Process opcode for convert into to short
        case OPCODE_I2S: {
            Cast(Type::Short);
            break;
        }
        // Process opcodes for converting to double
        case OPCODE_I2D:
        case OPCODE_F2D:
        case OPCODE_L2D: {
            Cast(Type::Double);
            break;
        }
        // Process opcodes for converting to long
        case OPCODE_I2L:
        case OPCODE_F2L:
        case OPCODE_D2L: {
            Cast(Type::Long);
            break;
        }
        // Process opcodes for converting to int
        case OPCODE_F2I:
        case OPCODE_D2I:
        case OPCODE_L2I: {
            Cast(Type::Int);
            break;
        }
        // Process opcodes for converting to float
        case OPCODE_I2F:
        case OPCODE_D2F:
        case OPCODE_L2F: {
            Cast(Type::Float);
            break;
        }
        // Process opcode for creating new objects
        case OPCODE_NEW: {
            auto obj =
                GetConstantClassFromClass(m_ClassFile, instruction.Op1.value);
            StackEntry::New newObj{obj};
            m_Stack.emplace_back(StackEntry(newObj));
            break;
        }
        // Process opcode for duplicating top stack value
        case OPCODE_DUP: {
            StackEntry val = PopStack();
            if (val.isArray() || val.isNew()) {
                m_Stack.emplace_back(val);
            } else {
                m_Stack.emplace_back(val);
                m_Stack.emplace_back(val);
            }
            break;
        }
        // Process opcode for duplicating the top value and inserting it below
        // the second value
        case OPCODE_DUP_X1: {
            StackEntry val1 = PopStack();
            StackEntry val2 = PopStack();
            m_Stack.emplace_back(StackEntry(val1));
            m_Stack.emplace_back(StackEntry(val2));
            m_Stack.emplace_back(StackEntry(val1));
            break;
        }
        // Process opcode for Duplicating the top value and inserting it below
        // the third value
        case OPCODE_DUP_X2: {
            StackEntry val1 = PopStack();
            StackEntry val2 = PopStack();
            StackEntry val3 = PopStack();
            m_Stack.emplace_back(StackEntry(val1));
            m_Stack.emplace_back(StackEntry(val3));
            m_Stack.emplace_back(StackEntry(val2));
            m_Stack.emplace_back(StackEntry(val1));
            break;
        }
        // Process opcode for duplicating the top two values
        case OPCODE_DUP2: {
            StackEntry val1 = PopStack();
            StackEntry val2 = PopStack();
            m_Stack.emplace_back(StackEntry(val2));
            m_Stack.emplace_back(StackEntry(val1));
            m_Stack.emplace_back(StackEntry(val2));
            m_Stack.emplace_back(StackEntry(val1));
            break;
        }
        // Process opcode for duplicating the top two values and inserting them
        // below the third value
        case OPCODE_DUP2_X1: {
            StackEntry val1 = PopStack();
            StackEntry val2 = PopStack();
            StackEntry val3 = PopStack();
            m_Stack.emplace_back(StackEntry(val2));
            m_Stack.emplace_back(StackEntry(val1));
            m_Stack.emplace_back(StackEntry(val3));
            m_Stack.emplace_back(StackEntry(val2));
            m_Stack.emplace_back(StackEntry(val1));
            break;
        }
        // Inserts the duplicated top two values below the fourth value
        case OPCODE_DUP2_X2: {
            StackEntry val1 = PopStack();
            StackEntry val2 = PopStack();
            StackEntry val3 = PopStack();
            StackEntry val4 = PopStack();
            m_Stack.emplace_back(StackEntry(val2));
            m_Stack.emplace_back(StackEntry(val1));
            m_Stack.emplace_back(StackEntry(val4));
            m_Stack.emplace_back(StackEntry(val3));
            m_Stack.emplace_back(StackEntry(val2));
            m_Stack.emplace_back(StackEntry(val1));
            break;
        }
        // Swaps the top two stack values
        case OPCODE_SWAP: {
            StackEntry val1 = PopStack();
            StackEntry val2 = PopStack();
            m_Stack.emplace_back(StackEntry(val1));
            m_Stack.emplace_back(StackEntry(val2));
            break;
        }
        default:
            // Handle other instructions
            return nullptr;
    }

    return nullptr;
}

/**
 * Store function. Pops value from the stack, determines the variable type,
 * stores the value in m_LocalVariables, then generates an AST node.
 *
 * @param idx uint8_t index of m_LocalVariables where the value will be stored
 */
std::optional<ASTPtr> Codegen::Store(uint8_t idx) {
    if (m_Stack.empty()) throw std::runtime_error("Empty stack");
    // Get value from stack
    StackEntry val = PopStack();
    Type ty = val.ty();
    int index = static_cast<int>(idx);

    std::string ident;
    // Identify and label variable type
    switch (ty.kind) {
        case Type::Boolean:
            ident = "bo" + std::to_string(index);
            break;
        case Type::Byte:
            ident = "by" + std::to_string(index);
            break;
        case Type::Char:
            ident = "ch" + std::to_string(index);
            break;
        case Type::Short:
            ident = "sh" + std::to_string(index);
            break;
        case Type::Int:
            ident = "i" + std::to_string(index);
            break;
        case Type::Float:
            ident = "f" + std::to_string(index);
            break;
        case Type::Double:
            ident = "d" + std::to_string(index);
            break;
        case Type::Long:
            ident = "l" + std::to_string(index);
            break;
        case Type::ClassName: {
            if (ty.class_name == "java/lang/String") {
                ident = "s" + std::to_string(index);
            } else {
                ident = "obj" + std::to_string(index);
            }
            break;
        }
        case Type::Reference:
            ident = "arr" + std::to_string(index);
            break;
        case Type::Void:
            throw std::runtime_error("Cannot store variable of type void");
        default:
            throw std::runtime_error("Unknown type");
    }
    // Store value in local variable map
    auto it = m_LocalVariables.find(index);
    StackEntry::Identifier identifier{ident, ty};
    m_LocalVariables[index] = StackEntry(identifier);

    if (it != m_LocalVariables.end()) {
        // Construct ReAssignment node
        AST::Identifier identifier{ident, ty};
        ASTPtr variable = std::make_unique<AST>(identifier);  // Identifier AST
        ASTPtr value = ConvertStackEntryToAST(val);           // Value AST
        AST::ReAssignment reAssign{variable, value};
        return std::make_unique<AST>(reAssign);
    } else {
        // Construct Assignment node
        ASTPtr value = ConvertStackEntryToAST(std::move(val));  // Value AST
        AST::Assignment assign{ty, ident, std::move(value)};
        return std::make_unique<AST>(assign);
    }
}
/**
 * Function to convert a StackEntry to equivalent AST node
 *
 * @param entry StackEntry to be identified and converted to AST node
 */
ASTPtr Codegen::ConvertStackEntryToAST(StackEntry entry) {
    // Identify entry type and create appropriate AST node
    if (entry.isInteger()) {
        return std::make_shared<AST>(entry.getIntegerValue());
    } else if (entry.isFloat()) {
        return std::make_shared<AST>(entry.getFloatValue());
    } else if (entry.isString()) {
        return std::make_shared<AST>(std::string(entry.getStringValue()));
    } else if (entry.isIdentifier()) {
        auto identifier = entry.getIdentifierValue();
        AST::Identifier astIdentifier{identifier.name, identifier.type};
        return std::make_shared<AST>(astIdentifier);
    } else if (entry.isField()) {
        auto field = entry.getFieldValue();
        auto obj = ConvertStackEntryToAST(StackEntry(field.obj->value));
        AST::FieldAccess fieldAst{obj, field.field_name, field.field_type};
        return std::make_shared<AST>(fieldAst);
    } else if (entry.isClass()) {
        auto field = entry.getClassValue();
        return std::make_shared<AST>(field.m_ClassName);
    } else if (entry.isFunction()) {
        auto function = entry.getFunctionValue();
        std::vector<ASTPtr> arguments;
        for (auto& args : function.arguments) {
            arguments.emplace_back(ConvertStackEntryToAST(args));
        }
        AST::FunctionCall functionAst{function.name, arguments,
                                      function.return_type};
        return std::make_shared<AST>(functionAst);
    } else if (entry.isArray()) {
        auto arrayStack = entry.getArrayValue();
        std::vector<ASTPtr> elements;

        for (auto it : arrayStack.elements) {
            elements.emplace_back(
                std::make_shared<AST>(ConvertStackEntryToAST(it)));
        }

        AST::Array arr{arrayStack.type,
                       ConvertStackEntryToAST(*arrayStack.length.get()),
                       elements};
        return std::make_shared<AST>(arr);
    } else if (entry.isBinaryOp()) {
        auto binOp = entry.getBinaryOpValue();

        AST::BinaryOperation bin{ConvertStackEntryToAST(*binOp.lhs), binOp.op,
                                 ConvertStackEntryToAST(*binOp.rhs)};
        return std::make_shared<AST>(bin);
    } else if (entry.isUnaryOp()) {
        auto unaryOp = entry.getUnaryOpValue();

        AST::UnaryOperation unary{unaryOp.op,
                                  ConvertStackEntryToAST(*unaryOp.value)};
        return std::make_shared<AST>(unary);
    } else if (entry.isCast()) {
        auto castOp = entry.getCastValue();
        AST::Cast cast{castOp.target_type,
                       ConvertStackEntryToAST(*castOp.value)};
        return std::make_shared<AST>(cast);
    }
    // Handle additional cases based on `StackEntry` types
    throw std::runtime_error("Unsupported StackEntry type");
}

bool Codegen::IsBackEdge(
    const Insn& instruction,
    const std::map<size_t, std::set<size_t>>& dominatorTree,
    size_t currentBlockID) {
    // Assuming `instruction` has a target block in `Op1`
    size_t targetBlockID = instruction.Op1.value;

    auto it = dominatorTree.find(currentBlockID);
    if (it != dominatorTree.end()) {
        const auto& dominatorSet = it->second;
        if (dominatorSet.find(targetBlockID) != dominatorSet.end()) {
            return true;
        }
    }

    return false;
    // // Iterate over the dominator tree to check for back edges
    // for (const auto& entry : dominatorTree) {
    //     size_t dominatedBlockID = entry.first;
    //     const auto& dominatorSet = entry.second;

    //     if (dominatorSet.find(targetBlockID) != dominatorSet.end()) {
    //         // If the target block is in the dominator set of the current
    //         block,
    //         // it indicates a back edge (loop).
    //         return true;
    //     }
    // }
    // return false;
}

/**
 * Function to process conversion bytecode instructions
 *
 * @param ty Reference to the Type that is the target conversion type
 */
void Codegen::Cast(const Type& ty) {
    if (m_Stack.empty()) throw std::runtime_error("Empty stack");
    StackEntry val = PopStack();  // Get value to be converted
    StackEntry::Cast cast{
        ty, std::make_shared<StackEntry>(
                std::move(val))};         // Convert the value to the given type
    m_Stack.push_back(StackEntry(cast));  // Return converted value to the stack
}

// Function to process load bytecode instructions
void Codegen::Load(uint8_t idx) {
    int index = static_cast<int>(idx);
    for (const auto& pair : m_LocalVariables) {
        std::cout << "Available index: " << pair.first << "\n";
    }
    auto it = m_LocalVariables.find(index);
    if (it == m_LocalVariables.end()) {
        throw std::runtime_error("Local variable not found at index: " +
                                 std::to_string(index));
    }
    m_Stack.push_back(StackEntry(it->second));
}

ASTPtr Codegen::LoadLocalAST(uint8_t idx) {
    int index = static_cast<int>(idx);
    auto it = m_LocalVariables.find(index);
    if (it == m_LocalVariables.end())
        throw std::runtime_error("Local variable not found at index: " +
                                 std::to_string(index));

    m_Stack.push_back(StackEntry(it->second));
    return ConvertStackEntryToAST(it->second);
}

// Function to process binary operator bytecode instructions
void Codegen::BinaryOper(BinaryOp op) {
    if (m_Stack.size() < 2)
        throw std::runtime_error("Not enough values on the stack");
    StackEntry val2 = PopStack();
    StackEntry val1 = PopStack();
    StackEntry::BinaryOperation binOp{
        std::make_shared<StackEntry>(std::move(val1)), op,
        std::make_shared<StackEntry>(std::move(val2))};
    m_Stack.push_back(StackEntry(binOp));
}

StackEntry Codegen::PopStack() {
    if (m_Stack.empty()) throw std::runtime_error("Stack underflow");
    StackEntry val = std::move(m_Stack.back());
    m_Stack.pop_back();
    return val;
}

/**
 * Retrieves a UTF-8 encoded string from the Java class file constant pool
 * and converts it into a std::string.
 *
 * @param classFile Reference to the parsed Java class file structure
 * @param idx Index of UTF-8 constant pool entry
 * @return Decoded str::string containing UTF-8 value
 */
std::string GetConstantUTF8FromClass(const ClassFile& classFile, int idx) {
    int constantIdx = idx - 1;

    auto cpInfo = classFile.m_ConstantPool.m_ConstPoolInfo.at(constantIdx);
    auto* utf8Info = cpInfo.GetAs<UTF8Info>();

    std::stringstream s;
    for (int i = 0; i < utf8Info->length; i++) {
        s << utf8Info->bytes[i];
    }

    return s.str();
}

/**
 * Retrieves a fully qualified class name from the Java class file constant
 * pool. This function extracts a CONSTANT_Class entry, resolves the associated
 * UTF-8 string, and returns it as a std::string.
 *
 * @param classFile Reference to the parsed Java class file structure.
 * @param idx Index of the CONSTANT_Class entry in the constant pool.
 * @return className The fully qualified class name as a std::string.
 */
std::string GetConstantClassFromClass(const ClassFile& classFile, int idx) {
    int constantIdx = idx - 1;
    auto cpInfo = classFile.m_ConstantPool.m_ConstPoolInfo.at(constantIdx);
    auto* classInfo = cpInfo.GetAs<ClassInfo>();
    std::string className =
        GetConstantUTF8FromClass(classFile, classInfo->nameIndex);
    return className;
}

/**
 * Retrieves the name and type of a field or method from the Java class file
 * constant pool. This function extracts a CONSTANT_NameAndType entry, resolves
 * its UTF-8 name and descriptor, and returns a structured PoolKindTypeNameField
 * object.
 *
 * @param classFile Reference to the parsed Java class file structure.
 * @param idx Index of the CONSTANT_NameAndType entry in the constant pool.
 * @return typeName A PoolKindTypeNameField struct containing the field/method
 * name and its type.
 */
PoolKindTypeNameField GetNameAndTypeFromClassField(const ClassFile& classFile,
                                                   int idx) {
    int constantIdx = idx - 1;
    auto cpInfo = classFile.m_ConstantPool.m_ConstPoolInfo.at(constantIdx);
    auto* nameAndTypeInfo = cpInfo.GetAs<NameAndTypeInfo>();
    std::string name =
        GetConstantUTF8FromClass(classFile, nameAndTypeInfo->nameIndex);
    std::string desc =
        GetConstantUTF8FromClass(classFile, nameAndTypeInfo->descriptorIndex);
    PoolKindTypeNameField typeName{name,
                                   FieldDescriptor::newDescriptor(desc).m_Ty};
    return typeName;
}

/**
 * Retrieves the name and type (descriptor) of a method from the Java class file
 * constant pool. This function extracts a CONSTANT_NameAndType entry, resolves
 * its UTF-8 method name and descriptor, and returns a structured
 * PoolKindTypeNameMethod object.
 *
 * @param classFile Reference to the parsed Java class file structure.
 * @param idx Index of the CONSTANT_NameAndType entry in the constant pool.
 * @return typeName A PoolKindTypeNameMethod struct containing the method name
 * and its descriptor.
 */
PoolKindTypeNameMethod GetNameAndTypeFromClassMethod(const ClassFile& classFile,
                                                     int idx) {
    int constantIdx = idx - 1;
    auto cpInfo = classFile.m_ConstantPool.m_ConstPoolInfo.at(constantIdx);
    auto* nameAndTypeInfo = cpInfo.GetAs<NameAndTypeInfo>();
    // cpInfo.asNameAndType();
    std::string name =
        GetConstantUTF8FromClass(classFile, nameAndTypeInfo->nameIndex);
    std::string desc =
        GetConstantUTF8FromClass(classFile, nameAndTypeInfo->descriptorIndex);
    PoolKindTypeNameMethod typeName{name,
                                    MethodDescriptor::newDescriptor(desc)};
    return typeName;
}

/**
 * Retrieves field reference information from the Java class file constant pool.
 * This function extracts a CONSTANT_Fieldref entry, resolves the associated
 * class name and field name/type information, and returns a structured
 * FieldRefStack object.
 *
 * @param classFile Reference to the parsed Java class file structure.
 * @param idx Index of the CONSTANT_Fieldref entry in the constant pool.
 * @return FieldRefStack A FieldRefStack struct containing the class name, field
 * name, and field type.
 */
FieldRefStack GetFieldRefFromIndex(const ClassFile& classFile, int idx) {
    ConstPoolInfo info = classFile.m_ConstantPool.m_ConstPoolInfo.at(idx - 1);
    if (info.Tag == Tags::Field) {
        auto field = info.GetAs<FieldRefInfo>();
        // static_cast<FieldRefInfo*>(info.Info.get());
        std::string classInf =
            GetConstantClassFromClass(classFile, field->classIndex);
        PoolKindTypeNameField nameInf =
            GetNameAndTypeFromClassField(classFile, field->nameAndTypeIndex);
        return FieldRefStack{getLastClassNamePart(classInf), nameInf.m_Name,
                             nameInf.m_Type};
    }
}

MethodRefStack GetMethodRefFromIndex(const ClassFile& classFile, int idx) {
    ConstPoolInfo info = classFile.m_ConstantPool.m_ConstPoolInfo.at(idx - 1);
    if (info.Tag == Tags::Method) {
        auto method = info.GetAs<MethodRefInfo>();
        // static_cast<MethodRefInfo*>(info.Info.get());
        std::string classInf =
            GetConstantClassFromClass(classFile, method->classIndex);
        PoolKindTypeNameMethod methPool =
            GetNameAndTypeFromClassMethod(classFile, method->nameAndTypeIndex);
        return MethodRefStack{getLastClassNamePart(classInf), methPool.m_Name,
                              methPool.m_Type};
    } else if (info.Tag == Tags::InterfaceMethod) {
        auto methodInner = info.GetAs<InterfaceMethodRef>();
        // static_cast<InterfaceMethodRef*>(info.Info.get());
        std::string classInf =
            GetConstantClassFromClass(classFile, methodInner->classIndex);
        PoolKindTypeNameMethod methPool = GetNameAndTypeFromClassMethod(
            classFile, methodInner->nameAndTypeIndex);
        return MethodRefStack{getLastClassNamePart(classInf), methPool.m_Name,
                              methPool.m_Type};
    }
}

/**
 * Retrieves method reference information from the Java class file constant
 * pool. This function extracts a CONSTANT_Methodref or
 * CONSTANT_InterfaceMethodref entry, resolves the associated class name, method
 * name, and method descriptor, and returns a structured MethodRefStack object.
 *
 * @param classFile Reference to the parsed Java class file structure.
 * @param idx The 1-based index of the method reference entry in the constant
 * pool.
 * @return *bm A MethodRefStack struct containing the class name, method name,
 * and method descriptor.
 */
BootstrapMethods GetBootstrapMethod(const ClassFile& classFile) {
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

/**
 * Retrieves dynamic method invocation reference information from the Java class
 * file constant pool. This function extracts a CONSTANT_InvokeDynamic entry,
 * resolves its associated bootstrap method, method name, and method descriptor,
 * and returns a structured DynamicInvokeRefStack object.
 *
 * @param classFile Reference to the parsed Java class file structure.
 * @param idx The 1-based index of the CONSTANT_InvokeDynamic entry in the
 * constant pool.
 * @return DynamicInvokeRefStack struct containing the class name, method name,
 * method descriptor, and bootstrap method arguments.
 */
DynamicInvokeRefStack GetDynamicInvokeRefFromIndex(const ClassFile& classFile,
                                                   int idx) {
    ConstPoolInfo info = classFile.m_ConstantPool.m_ConstPoolInfo.at(idx - 1);
    if (info.Tag == Tags::InvokeDynamic) {
        auto method = info.GetAs<InvokeDynamicInfo>();
        // static_cast<InvokeDynamicInfo*>(info.Info.get());
        PoolKindTypeNameMethod methPool =
            GetNameAndTypeFromClassMethod(classFile, method->nameAndTypeIndex);
        BootstrapMethodsInner bootstrapMethod =
            GetBootstrapMethod(classFile).bootstrapsMethods.at(
                method->bootstrapMethodAttrIndex);
        auto cpi = classFile.m_ConstantPool.m_ConstPoolInfo.at(
            bootstrapMethod.bootstrapMethodRef);
        if (cpi.Tag == Tags::Method) {
            auto methodInner = cpi.GetAs<MethodRefInfo>();
            // static_cast<MethodRefInfo*>(cpi.Info.get());
            PoolKindTypeNameMethod methodPool = GetNameAndTypeFromClassMethod(
                classFile, method->nameAndTypeIndex);
            std::string classInf =
                GetConstantClassFromClass(classFile, methodInner->classIndex);
            BootstrapMethodsInner bootstrapMethod =
                GetBootstrapMethod(classFile).bootstrapsMethods.at(
                    method->bootstrapMethodAttrIndex);
            return DynamicInvokeRefStack{getLastClassNamePart(classInf),
                                         methPool.m_Name, methPool.m_Type,
                                         bootstrapMethod.bootstrapArgs};
        }
    }
}

/**
 * Retrieves a string from the Java class file constant pool.
 * This function extracts a CONSTANT_String entry, resolves its associated UTF-8
 * value, and returns it as a std::string.
 *
 * @param classFile Reference to the parsed Java class file structure.
 * @param idx The 1-based index of the CONSTANT_String entry in the constant
 * pool.
 * @return stringName The extracted string as a std::string.
 */
std::string GetStringFromIndex(const ClassFile& classFile, int idx) {
    int constantIdx = idx - 1;
    auto cpInfo = classFile.m_ConstantPool.m_ConstPoolInfo.at(constantIdx);
    auto* classInfo = cpInfo.GetAs<StringInfo>();
    // cpInfo.asStringInfo();
    std::string stringName =
        GetConstantUTF8FromClass(classFile, classInfo->stringIndex);
    return stringName;
}

/**
 * Handles dynamic string concatenation via `StringConcatFactory` in Java's
 * `InvokeDynamic`. This function processes string concatenation arguments,
 * extracting constant strings from the constant pool and combining them into a
 * sequence of `BinaryOperation` nodes representing
 * `+` operations. The final concatenated result is pushed onto the operand
 * stack.
 *
 * @param unparsedArgs A vector of constant pool indices representing the string
 * parts involved in the concatenation.
 */
void Codegen::StringConcatFactory(const std::vector<uint16_t>& unparsedArgs) {
    std::vector<StackEntry> args;  // Store parsed string arguments and dynamic
                                   // operands for concatenation.
    // Iterate over each constant pool index provided in unparsedArgs.
    for (auto arg : unparsedArgs) {
        std::string s = GetStringFromIndex(m_ClassFile, arg);
        InvokeDynamicArgs parser(s);

        while (auto kindOpt = parser.next()) {
            auto kind = kindOpt.value();
            // If the argument comes from the constant pool, store it as a
            // string.
            if (kind.type == InvokeDynamicArgs::ArgType::Pool) {
                args.emplace_back(StackEntry::String{s});
            } else {
                args.emplace_back(PopStack());
            }
        }
    }
    // Construct the initial binary operation from the last two arguments.
    StackEntry::BinaryOperation entry{
        std::make_shared<StackEntry>(args.back()), BinaryOp::Add,
        std::make_shared<StackEntry>(args[args.size() - 2])};
    args.pop_back();
    args.pop_back();
    // Chain remaining arguments into a sequence of binary additions.
    while (!args.empty()) {
        StackEntry::BinaryOperation binOp{
            std::make_shared<StackEntry>(entry), BinaryOp::Add,
            std::make_shared<StackEntry>(args.back())};
        entry = binOp;
        args.pop_back();
    }

    m_Stack.emplace_back(entry);
}

ASTPtr Codegen::ConvertConditionToAST(const Insn& instruction,
                                      std::vector<StackEntry>& stack) {
    ASTPtr cond;
    if (instruction.opcode >= OPCODE_IF_ICMPEQ &&
        instruction.opcode <= OPCODE_IF_ICMPLE) {
        if (stack.size() >= 2) {
            StackEntry val2 = stack.back();
            stack.pop_back();
            StackEntry val1 = stack.back();
            stack.pop_back();
            BinaryOp op;

            switch (instruction.opcode) {
                case OPCODE_IF_ICMPEQ:
                    op = BinaryOp::Equal;
                    break;
                case OPCODE_IF_ICMPNE:
                    op = BinaryOp::NotEqual;
                    break;
                case OPCODE_IF_ICMPLT:
                    op = BinaryOp::LessThan;
                    break;
                case OPCODE_IF_ICMPGE:
                    op = BinaryOp::GreaterEqualThan;
                    break;
                case OPCODE_IF_ICMPGT:
                    op = BinaryOp::GreaterThan;
                    break;
                case OPCODE_IF_ICMPLE:
                    op = BinaryOp::LessEqualThan;
                    break;
            }

            AST::BinaryOperation binOp{ConvertStackEntryToAST(val1), op,
                                       ConvertStackEntryToAST(val2)};
            cond = std::make_shared<AST>(binOp);
        }
    } else if (instruction.opcode >= OPCODE_IFEQ &&
               instruction.opcode <= OPCODE_IFLE) {
        if (!stack.empty()) {
            StackEntry val = stack.back();
            stack.pop_back();
            BinaryOp op;
            switch (instruction.opcode) {
                case OPCODE_IFEQ:
                    op = BinaryOp::Equal;
                    break;
                case OPCODE_IFNE:
                    op = BinaryOp::NotEqual;
                    break;
                case OPCODE_IFLT:
                    op = BinaryOp::LessThan;
                    break;
                case OPCODE_IFGE:
                    op = BinaryOp::GreaterEqualThan;
                    break;
                case OPCODE_IFGT:
                    op = BinaryOp::GreaterThan;
                    break;
                case OPCODE_IFLE:
                    op = BinaryOp::LessEqualThan;
                    break;
            }
            ASTPtr zero = std::make_shared<AST>(0);
            cond = std::make_shared<AST>(
                AST::BinaryOperation{ConvertStackEntryToAST(val), op, zero});
        }
    } else if (instruction.opcode == OPCODE_IFNULL) {
        if (!stack.empty()) {
            StackEntry val = stack.back();
            stack.pop_back();
            ASTPtr null_val = std::make_shared<AST>(std::string("null"));
            cond = std::make_shared<AST>(AST::BinaryOperation{
                ConvertStackEntryToAST(val), BinaryOp::Equal, null_val});
        }
    } else if (instruction.opcode == OPCODE_IFNONNULL) {
        if (!stack.empty()) {
            StackEntry val = stack.back();
            stack.pop_back();
            ASTPtr null_val = std::make_shared<AST>(std::string("null"));
            cond = std::make_shared<AST>(AST::BinaryOperation{
                ConvertStackEntryToAST(val), BinaryOp::NotEqual, null_val});
        }
    }
    return cond;
}

ASTPtr Codegen::ConvertUpdateToAST(const Insn& instruction) {
    if (instruction.opcode == OPCODE_IINC) {
        int localIdx = (instruction.Op2.value >> 8) & 0xFF;
        int32_t amount = instruction.Op2.value & 0xFF;
        StackEntry val = m_LocalVariables.at(localIdx);

        if (!val.isInteger()) {
            val = StackEntry(-1);
        }

        if (val.isInteger()) {
            val = StackEntry(amount);
        }

        // if(val.isIdentifier()) {
        //     std::cout << "IDENTIFIER" << std::endl;
        //     if(amount == 1) {
        //         AST::UnaryOperation unaryOp{UnaryOp::PlusPlus,
        //         ConvertStackEntryToAST(val)}; ASTPtr variable =
        //         std::make_unique<AST>(std::move(unaryOp)); // Identifier AST
        //         return variable;
        //     } else {
        //         ASTPtr variable = ConvertStackEntryToAST(std::move(val));
        //         AST::BinaryOperation binaryOp{std::move(variable),
        //         BinaryOp::Add, std::make_unique<AST>(amount)};
        //         AST::ReAssignment reAssign{std::move(variable),
        //         std::make_unique<AST>(std::move(binaryOp))}; return reAssign;
        //     }
        // }
        // std::string varName = "local" +
        // std::to_string(instruction.Op1.value); ASTPtr var =
        // std::make_shared<AST>(AST::Identifier{varName, Type()}); ASTPtr
        // const_val =
        // std::make_shared<AST>(static_cast<int32_t>(instruction.Op2.value));
        ASTPtr variable = ConvertStackEntryToAST(std::move(val));
        return std::make_shared<AST>(AST::BinaryOperation{
            std::move(variable), BinaryOp::Add, std::make_unique<AST>(amount)});
    } else if (instruction.opcode == OPCODE_ASTORE ||
               instruction.opcode == OPCODE_ISTORE ||
               instruction.opcode == OPCODE_FSTORE ||
               instruction.opcode == OPCODE_DSTORE ||
               instruction.opcode == OPCODE_LSTORE) {
        return Store(instruction.Op1.value).value();
        // std::string varName = "local" +
        // std::to_string(instruction.Op1.value); ASTPtr var =
        // std::make_shared<AST>(AST::Identifier{varName, Type()}); ASTPtr val =
        // std::make_shared<AST>(AST::Identifier{"stack1", Type()}); //get from
        // the stack.  This is a problem. return
        // std::make_shared<AST>(AST::ReAssignment{var, val});
    }
    return nullptr;
}
}  // namespace TitaniumDecompiler
