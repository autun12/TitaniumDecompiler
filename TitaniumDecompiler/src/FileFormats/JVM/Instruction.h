#pragma once

#include <stdint.h>

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "CodeConstants.h"

namespace TitaniumDecompiler {

class Instruction {
public:
    Instruction();
    ~Instruction() = default;
};

enum Group { GROUP_GENERAL = 1, GROUP_JUMP = 2, GROUP_SWITCH = 3, GROUP_INVOCATION = 4, GROUP_FIELDACCESS = 5, GROUP_RETURN = 6 };

struct Operand {
    uint8_t offset;
    uint64_t addr;
    uint64_t value;
};

struct Insn {
    uint8_t opcode;
    std::string opcodeName;
    Operand ops[7];
#define Op1 ops[0]  ///< first operand
#define Op2 ops[1]  ///< second operand
#define Op3 ops[2]  ///< third operand
#define Op4 ops[3]  ///< fourth operand
#define Op5 ops[4]  ///< fifth operand
#define Op6 ops[5]  ///< sixth operand
#define Op7 ops[6]  ///< seventh operand
#define Op8 ops[7]  ///< eighth operand
    uint8_t group;

    bool operator<(const Insn& other) const { return std::tie(opcode, opcodeName, group) < std::tie(other.opcode, other.opcodeName, other.group); }

    bool CanFallThrough() {
        return opcode != OPCODE_GOTO && opcode != OPCODE_GOTO_W && opcode != OPCODE_RET && !(opcode >= OPCODE_IRETURN && opcode <= OPCODE_RETURN) &&
               opcode != OPCODE_ATHROW && opcode != OPCODE_JSR && opcode != OPCODE_TABLESWITCH && opcode != OPCODE_LOOKUPSWITCH;
    }
};

typedef std::multimap<uint32_t, Insn> InstrMap;

std::string ReadInstruction(const std::vector<uint8_t>& bytes);
// std::vector<Insn> ReadInstructions(const std::vector<uint8_t>& bytes);
InstrMap ReadInstructions(const std::vector<uint8_t>& bytes);
uint64_t ReferencesInstruction(const std::vector<uint8_t>& bytes, uint32_t pc);
uint8_t LoadDataConstant(const std::vector<uint8_t>& bytes, uint32_t pc);
uint8_t LoadDataConstantWide(const std::vector<uint8_t>& bytes, uint32_t pc);
int16_t IfStatement(const std::vector<uint8_t>& bytes, uint32_t pc);
uint16_t IncrementInstruction(const std::vector<uint8_t>& bytes, uint32_t pc);
int16_t GotoInstruction(const std::vector<uint8_t>& bytes, uint32_t pc);
uint8_t BiPushInstruction(const std::vector<uint8_t>& bytes, uint32_t pc);
std::string ParseLookupSwitch(const std::vector<uint8_t>& bytes, uint32_t pc);
bool LoadIndex(Insn& insn, const std::vector<uint8_t>& bytes, uint32_t pc);
}  // namespace TitaniumDecompiler
