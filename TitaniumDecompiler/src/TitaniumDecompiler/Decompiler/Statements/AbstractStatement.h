#pragma once

#include <stdint.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "EdgeDirection.h"

namespace TitaniumDecompiler {
enum class StatementType : uint8_t { ROOT, BASIC_BLOCK, SEQUENCE, DUMMY_EXIT, GENERAL, IF, DO, SWITCH, SYNCHORNIZED, TRY_CATCH, CATCH_ALL };

class AbstractStatement {
public:
    ~AbstractStatement() = default;
    void ClearTempInfo();

protected:
    AbstractStatement(StatementType type, int id);
    AbstractStatement(StatementType type);

public:
    StatementType m_Type;
    int m_Id;
};

// Base class for statements
class Statement {
public:
    virtual void Print() const = 0;
};

// Assignment statement
class AssignmentStatement : public Statement {
private:
    std::string lhs;
    std::string rhs;

public:
    AssignmentStatement(const std::string& left, const std::string& right) : lhs(left), rhs(right) {}

    void Print() const override { std::cout << lhs << " = " << rhs << ";\n"; }
};

// If statement (conditional)
class IfStatement : public Statement {
private:
    std::string condition;
    std::vector<std::shared_ptr<Statement>> trueBranch;
    std::vector<std::shared_ptr<Statement>> falseBranch;

public:
    IfStatement(const std::string& cond) : condition(cond) {}

    void AddTrueBranch(const std::shared_ptr<Statement>& stmt) { trueBranch.push_back(stmt); }

    void AddFalseBranch(const std::shared_ptr<Statement>& stmt) { falseBranch.push_back(stmt); }

    void Print() const override {
        std::cout << "if (" << condition << ") {\n";
        for (const auto& stmt : trueBranch) {
            stmt->Print();
        }
        if (!falseBranch.empty()) {
            std::cout << "} else {\n";
            for (const auto& stmt : falseBranch) {
                stmt->Print();
            }
        }
        std::cout << "}\n";
    }
};
}  // namespace TitaniumDecompiler
