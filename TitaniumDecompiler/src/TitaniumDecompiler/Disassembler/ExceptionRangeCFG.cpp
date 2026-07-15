#include "ExceptionRangeCFG.h"

#include <algorithm>
#include <memory>

namespace TitaniumDecompiler {
ExceptionRangeCFG::ExceptionRangeCFG(std::vector<std::shared_ptr<BasicBlock>> protectedRange, std::shared_ptr<BasicBlock> handler,
                                     std::vector<std::string> exceptionType) {
    m_ProtectedRange = protectedRange;
    m_Handler = handler;
    if (exceptionType.empty() != false) {
        m_ExceptionTypes = exceptionType;
    }
}

ExceptionRangeCFG::ExceptionRangeCFG(std::vector<std::shared_ptr<BasicBlock>> protectedRange, std::shared_ptr<BasicBlock> handler, std::string exceptionType) {
    m_ProtectedRange = protectedRange;
    m_Handler = handler;
    if (exceptionType.empty() != false) {
        m_ExceptionTypes.push_back(exceptionType);
    }
}

bool ExceptionRangeCFG::IsCircular() { return std::find(m_ProtectedRange.begin(), m_ProtectedRange.end(), m_Handler) != m_ProtectedRange.end(); }

std::shared_ptr<BasicBlock> ExceptionRangeCFG::GetHandler() { return m_Handler; }

void ExceptionRangeCFG::SetHandler(std::shared_ptr<BasicBlock> handler) { m_Handler = handler; }

std::vector<std::shared_ptr<BasicBlock>> ExceptionRangeCFG::GetProtectedRange() { return m_ProtectedRange; }

std::vector<std::string> ExceptionRangeCFG::GetExceptionTypes() { return m_ExceptionTypes; }

void ExceptionRangeCFG::AddExceptionType(std::string exceptionType) {
    if (m_ExceptionTypes.empty() == true) {
        return;
    }

    if (exceptionType.empty()) {
        // m_ExceptionTypes = nullptr;
    } else {
        m_ExceptionTypes.push_back(exceptionType);
    }
}

}  // namespace TitaniumDecompiler
