#include "Function.h"

#include <memory>

namespace TitaniumDecompiler {
Function::Function(const std::string& functionName) : m_FunctionName(functionName) {}

std::shared_ptr<CFG> Function::GetFunctionCFG() const { return m_CFG; }

std::string Function::GetFunctionName() { return m_FunctionName; }

void Function::SetFunctionCFG(const std::shared_ptr<CFG>& cfg) { m_CFG = cfg; }

void Function::SetFunctionName(const std::string& functionName) { m_FunctionName = functionName; }

void Function::SetMethod(const Methods& method) { m_Method = method; }

Methods Function::GetMethod() { return m_Method; }

void Function::SetMethodDesc(const std::string& methodDesc) { m_MethodDesc = methodDesc; }

std::string Function::GetMethodDesc() { return m_MethodDesc; }

void Function::SetMethodAccessFlags(const std::string& methodAccessFlags) { m_MethodAccessFlags = methodAccessFlags; }

std::string Function::GetMethodAccessFlags() { return m_MethodAccessFlags; }

void Function::SetMaxStack(uint16_t maxStack) { m_MaxStack = maxStack; }

uint16_t Function::GetMaxStack() { return m_MaxStack; }

void Function::SetMaxLocals(uint16_t maxLocals) { m_MaxLocals = maxLocals; }

uint16_t Function::GetMaxLocals() { return m_MaxLocals; }

std::multimap<uint64_t, std::string> Function::GetLocals() { return m_LocalVars; }

void Function::InsertIntoRefConstPool(uint64_t index, std::string constPoolName) { m_RefConstPool.insert({index, constPoolName}); }

std::multimap<uint64_t, std::string> Function::GetRefConstPool() { return m_RefConstPool; }
}  // namespace TitaniumDecompiler
