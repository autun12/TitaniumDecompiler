#pragma once

#include "ExprVisitor.h"

namespace TitaniumDecompiler {
class Expr {
public:
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor& visitor) = 0;
};
}  // namespace TitaniumDecompiler