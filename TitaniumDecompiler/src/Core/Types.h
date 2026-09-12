#pragma once

#include <stdint.h>

#include <string>
#include <vector>

namespace TitaniumDecompiler {
using InstructionIndex = uint32_t;
using BlockId = uint32_t;

struct Symbol {
    std::string Name;
    std::string ParentScope;
    uint64_t Address;
    size_t Size;
    bool IsFunction;
};

struct Section {
    std::string Name;
    uint64_t VirtualAddress;
    std::vector<uint8_t> Bytes;
    bool IsExecutable;
};

struct MetadataRow {
    uint32_t Index;
    std::string Kind;
    std::string Details;
    std::string RawRepresentation;
};

struct MetadataTable {
    std::string TableName;
    std::vector<std::string> ColumnHeaders;
    std::vector<MetadataRow> Rows;
};

struct BasicBlock {
    BlockId id;
    uint64_t startAddr;
    uint64_t endAddr;

    std::vector<InstructionIndex> instructions;
    std::vector<BlockId> successors;
    std::vector<BlockId> predecessors;
};

}  // namespace TitaniumDecompiler
