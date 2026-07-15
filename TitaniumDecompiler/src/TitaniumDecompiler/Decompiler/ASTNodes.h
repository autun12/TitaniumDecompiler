#pragma once

#include <memory>
#include <string>
#include <vector>

namespace TitaniumDecompiler {
enum class ASTNodeType { SEQUENCE, ASSIGNMENT, CONDITIONAL, LOOP, FUNCTION_CALL, SWITCH_CASE, VARIABLE_ACCESS, RETURN, EXPRESSION };

class ASTNode {
public:
    ASTNodeType m_Type;
    std::string m_Value;
    std::vector<std::shared_ptr<ASTNode>> m_Children;

    ASTNode(ASTNodeType type, std::string val) : m_Type(type), m_Value(std::move(val)) {}

    void addChild(std::shared_ptr<ASTNode> child) { m_Children.push_back(child); }
};
}  // namespace TitaniumDecompiler