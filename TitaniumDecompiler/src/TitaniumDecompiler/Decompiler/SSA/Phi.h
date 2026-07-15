#pragma once

namespace TitaniumDecompiler {

struct Phi {
private:
    std::shared_ptr<BasicBlock> m_Block;
};

}  // namespace TitaniumDecompiler