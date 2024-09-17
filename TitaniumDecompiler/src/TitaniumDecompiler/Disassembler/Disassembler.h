#include "tdpch.h"
#include <string>
#include <vector>
#include "../Platform/JVM/ClassFileParser.h"

namespace TitaniumDecompiler {

class Disassembler {
public:
    void OpenFile();
private:
    ClassFileParser m_ClassFile;
};

}