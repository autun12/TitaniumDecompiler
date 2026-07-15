#pragma once

#include <string>
#include <vector>

#include "TitaniumDecompiler/Disassembler/BasicBlock.h"

namespace TitaniumDecompiler {
class ExceptionRangeCFG {
public:
    ExceptionRangeCFG(std::vector<std::shared_ptr<BasicBlock>> protectedRange, std::shared_ptr<BasicBlock> handler, std::vector<std::string> exceptionType);
    ExceptionRangeCFG(std::vector<std::shared_ptr<BasicBlock>> protectedRange, std::shared_ptr<BasicBlock> handler, std::string exceptionType);
    ~ExceptionRangeCFG() = default;
    bool IsCircular();
    std::shared_ptr<BasicBlock> GetHandler();
    void SetHandler(std::shared_ptr<BasicBlock> handler);
    std::vector<std::shared_ptr<BasicBlock>> GetProtectedRange();
    std::vector<std::string> GetExceptionTypes();
    void AddExceptionType(std::string exceptionType);

private:
    std::vector<std::shared_ptr<BasicBlock>> m_ProtectedRange;
    std::shared_ptr<BasicBlock> m_Handler;
    std::vector<std::string> m_ExceptionTypes;
};
}  // namespace TitaniumDecompiler
