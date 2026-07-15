#pragma once

namespace TitaniumDecompiler {

class BinaryExpr;
class NumberExpr;
class VariableExpr;

class ExprVisitor {
public:
    virtual ~ExprVisitor() {}

    virtual void Visit(const NumberExpr& binary) = 0;
    virtual void Visit(const VariableExpr& binary) = 0;
    virtual void Visit(const BinaryExpr& binary) = 0;
};
}  // namespace TitaniumDecompiler