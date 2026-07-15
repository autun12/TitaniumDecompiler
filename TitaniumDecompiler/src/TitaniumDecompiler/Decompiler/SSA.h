#pragma once

#include <stdint.h>

#include <iomanip>
#include <map>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "AST.h"

namespace TitaniumDecompiler {

struct SSAVariable {
    enum Origin { LOCAL, STACK, OPERATION, CONSTANT, UNKNOWN };

    Type type;
    Origin origin;
    std::string originName;
    bool isConstant = false;
    bool isUninitialized = false;
    int originalNumber = -1;  // Or a more specific identifier based on origin
    int version = 0;          // SSA version number

    SSAVariable() : type(Type::BasicType(Type::Void, "")), origin(SSAVariable::Origin::UNKNOWN), originName(""), originalNumber(-1) {}

    // Constructor
    SSAVariable(Type t, Origin o, std::string oName, int origNum = -1) : type(t), origin(o), originName(oName), originalNumber(origNum) {}

    // Equality comparison (important for phi-node arguments)
    bool operator==(const SSAVariable& other) const {
        return type == other.type && origin == other.origin && originName == other.originName && originalNumber == other.originalNumber &&
               version == other.version;
    }

    // Less than operator (useful for using in sets or maps if needed)
    bool operator<(const SSAVariable& other) const {
        if (origin != other.origin) return origin < other.origin;
        if (originName != other.originName) return originName < other.originName;
        if (originalNumber != other.originalNumber) return originalNumber < other.originalNumber;
        return version < other.version;
    }

    std::string toString() const {
        std::stringstream ss;
        ss << originName << "." << version;
        return ss.str();
    }
};

struct SSAInstruction {
    enum Operation {
        // Arithmetic
        ADD,
        SUB,
        MUL,
        DIV,
        REM,
        // Bitwise
        SHL,
        SHR,
        USHR,
        AND,
        OR,
        XOR,
        // Comparison (might produce boolean SSA variables)
        ICMP_EQ,
        ICMP_NE,
        ICMP_LT,
        ICMP_GT,
        ICMP_LE,
        ICMP_GE,
        ICMP_NE_ZERO,
        // Load and Store (local variables)
        LOAD_LOCAL,
        STORE_LOCAL,
        // Load constant
        LOAD_CONST,
        // Stack manipulation (DUP, POP - might not directly translate to SSA instructions?)
        DUP,
        POP,
        // Method invocation
        CALL,
        // Return
        RETURN,
        // Phi function
        PHI,
        // Cast
        CAST,
        // Array operations
        ARRAY_LOAD,
        ARRAY_STORE,
        ARRAY_LENGTH,
        NEW_ARRAY,
        // Field access
        GET_FIELD,
        PUT_FIELD,
        GET_STATIC,
        PUT_STATIC,
        // Instanceof
        INSTANCEOF,
        //
        IF,
        GOTO,
        // ... other bytecode operations as needed
        UNKNOWN
    };

    Operation opcode;
    std::vector<SSAVariable> operands;
    std::optional<SSAVariable> result;  // Most instructions produce a result
    uint16_t branchTarget; // For GOTO

    // Constructor for instructions with a result
    SSAInstruction(Operation op, std::vector<SSAVariable> ops, std::optional<SSAVariable> res) : opcode(op), operands(std::move(ops)), result(res), branchTarget(0) {}

    // Constructor for instructions without a result (e.g., STORE, RETURN)
    SSAInstruction(Operation op, std::vector<SSAVariable> ops) : opcode(op), operands(std::move(ops)), result(std::nullopt), branchTarget(0) {}

    // Constructor for phi function
    SSAInstruction(std::vector<SSAVariable> ops, SSAVariable res) : opcode(PHI), operands(std::move(ops)), result(res) {}
 
    // Constructor for GOTO   
    SSAInstruction(Operation op, std::vector<SSAVariable> ops, std::optional<SSAVariable> res, uint16_t target)
        : opcode(op), operands(std::move(ops)), result(res), branchTarget(target) {}

    // Default constructor
    SSAInstruction(Operation op = UNKNOWN) : opcode(op), branchTarget(0) {}
    std::string toString() const {
        std::stringstream ss;
        ss << std::left << std::setw(12) << opcodeToString(opcode);  // Consistent width for opcode

        if (!operands.empty()) {
            for (size_t i = 0; i < operands.size(); ++i) {
                ss << operands[i].toString();
                if (i < operands.size() - 1) {
                    ss << ", ";
                }
            }
        }

        if (result.has_value()) {
            ss << " -> " << result->toString();
        }
        if (opcode == GOTO) {
            ss << ", target=" << branchTarget;
        }
        return ss.str();
    }

private:
    static std::string opcodeToString(Operation op) {
        switch (op) {
            case Operation::ADD:
                return "ADD";
            case Operation::SUB:
                return "SUB";
            case Operation::MUL:
                return "MUL";
            case Operation::DIV:
                return "DIV";
            case Operation::REM:
                return "REM";
            case Operation::SHL:
                return "SHL";
            case Operation::SHR:
                return "SHR";
            case Operation::USHR:
                return "USHR";
            case Operation::AND:
                return "AND";
            case Operation::OR:
                return "OR";
            case Operation::XOR:
                return "XOR";
            case Operation::ICMP_EQ:
                return "ICMP_EQ";
            case Operation::ICMP_NE:
                return "ICMP_NE";
            case Operation::ICMP_LT:
                return "ICMP_LT";
            case Operation::ICMP_GT:
                return "ICMP_GT";
            case Operation::ICMP_LE:
                return "ICMP_LE";
            case Operation::ICMP_GE:
                return "ICMP_GE";
            case Operation::ICMP_NE_ZERO:
                return "ICMP_NE_ZERO";
            case Operation::LOAD_LOCAL:
                return "LOAD_LOCAL";
            case Operation::STORE_LOCAL:
                return "STORE_LOCAL";
            case Operation::LOAD_CONST:
                return "LOAD_CONST";
            case Operation::DUP:
                return "DUP";
            case Operation::POP:
                return "POP";
            case Operation::CALL:
                return "CALL";
            case Operation::RETURN:
                return "RETURN";
            case Operation::PHI:
                return "PHI";
            case Operation::CAST:
                return "CAST";
            case Operation::ARRAY_LOAD:
                return "ARRAY_LOAD";
            case Operation::ARRAY_STORE:
                return "ARRAY_STORE";
            case Operation::ARRAY_LENGTH:
                return "ARRAY_LENGTH";
            case Operation::NEW_ARRAY:
                return "NEW_ARRAY";
            case Operation::GET_FIELD:
                return "GET_FIELD";
            case Operation::PUT_FIELD:
                return "PUT_FIELD";
            case Operation::GET_STATIC:
                return "GET_STATIC";
            case Operation::PUT_STATIC:
                return "PUT_STATIC";
            case Operation::INSTANCEOF:
                return "INSTANCEOF";
            case Operation::IF:
                return "IF";
            case Operation::GOTO:
                return "GOTO";
            case Operation::UNKNOWN:
                return "UNKNOWN";
            default:
                return "UNKNOWN";
        }
    }
};

struct SSAStatement {
    enum Kind {
        ASSIGNMENT,
        EXPRESSION,
        RETURN_STMT,
        // ... other basic statement types as needed
    };

    Kind kind;
    std::optional<SSAVariable> lhs;
    SSAInstruction instruction;

    SSAStatement(Kind k, std::optional<SSAVariable> left, const SSAInstruction& inst) : kind(k), lhs(left), instruction(inst) {}

    SSAStatement(Kind k, const SSAInstruction& inst) : kind(k), lhs(std::nullopt), instruction(inst) {}

    SSAStatement(Kind k) : kind(k) {}

    std::string toString() const {
        std::stringstream ss;
        switch (kind) {
            case ASSIGNMENT:
                if (lhs.has_value()) {
                    ss << lhs->toString() << " = " << instruction.toString();
                }
                break;
            case EXPRESSION:
                ss << instruction.toString();
                break;
            case RETURN_STMT:
                ss << "RETURN";
                if (lhs.has_value()) {
                    ss << " " << lhs->toString();
                }
                break;
            default:
                ss << "UNKNOWN STATEMENT";
                break;
        }
        return ss.str();
    }
};

struct SSABasicBlock {
public:
    SSABasicBlock(int id) : m_Id(id) {}
    ~SSABasicBlock() = default;

    int GetId() const { return m_Id; }
    const std::vector<SSAInstruction>& GetInstructions() const { return m_Instructions; }
    void AddInstruction(const SSAInstruction& inst) { m_Instructions.push_back(inst); }
    void AddInstructions(const std::vector<SSAInstruction>& insts) { m_Instructions.insert(m_Instructions.end(), insts.begin(), insts.end()); }

    // Add predecessors and successors if needed for SSA CFG
    void AddPredecessor(std::shared_ptr<SSABasicBlock> pred) { m_Predecessors.push_back(pred); }
    void AddSuccessor(std::shared_ptr<SSABasicBlock> succ) { m_Successors.push_back(succ); }

    const std::vector<std::shared_ptr<SSABasicBlock>>& GetPredecessors() const { return m_Predecessors; }
    const std::vector<std::shared_ptr<SSABasicBlock>>& GetSuccessors() const { return m_Successors; }

    std::string toString() const {
        std::stringstream ss;
        ss << "SSABasicBlock " << m_Id << ":\n";
        for (const auto& inst : m_Instructions) {
            ss << "  " << inst.toString() << "\n";
        }
        return ss.str();
    }

private:
    int m_Id;
    std::vector<SSAInstruction> m_Instructions;
    std::vector<std::shared_ptr<SSABasicBlock>> m_Predecessors;
    std::vector<std::shared_ptr<SSABasicBlock>> m_Successors;
};

class SSACFG {
public:
    SSACFG() = default;
    ~SSACFG() = default;

    std::map<int, std::shared_ptr<SSABasicBlock>> GetBlocks() const { return m_Blocks; }
    std::shared_ptr<SSABasicBlock> GetBlock(int id) const {
        auto it = m_Blocks.find(id);
        return (it != m_Blocks.end()) ? it->second : nullptr;
    }

    void AddBlock(std::shared_ptr<SSABasicBlock> block) { m_Blocks[block->GetId()] = block; }

    // Add a way to get the entry block
    std::shared_ptr<SSABasicBlock> GetEntryBlock() const { return m_EntryBlock; }
    void SetEntryBlock(std::shared_ptr<SSABasicBlock> block) { m_EntryBlock = block; }
    std::string toString() const {
        std::stringstream ss;
        ss << "SSACFG:\n";
        for (const auto& [id, block] : m_Blocks) {
            ss << block->toString();  // Use SSABasicBlock::toString()
        }
        return ss.str();
    }

private:
    std::map<int, std::shared_ptr<SSABasicBlock>> m_Blocks;
    std::shared_ptr<SSABasicBlock> m_EntryBlock;
};

struct InsnTempWrapper {
public:
    InsnTempWrapper() = default;
    ~InsnTempWrapper() = default;

public:
    std::string m_Name;
    std::string m_Desc;
    std::string m_ClassName;
    std::string m_ClassInf;
    std::string m_NameInf;
    std::string m_ConstString;
};
}  // namespace TitaniumDecompiler
