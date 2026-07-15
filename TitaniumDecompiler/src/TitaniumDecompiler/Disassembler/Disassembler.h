#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "CFG.h"
#include "Function.h"
#include "TitaniumDecompiler/Decompiler/SSA.h"
#include "TitaniumDecompiler/Kernel/Loader.h"
#include "TitaniumDecompiler/src/Platform/Linux/FileFormats/JVM/ClassFileParser.h"
#include "TitaniumDecompiler/src/Platform/Linux/FileFormats/JVM/Instruction.h"

namespace TitaniumDecompiler {

typedef std::multimap<std::string, std::string> ClassesMap;
typedef std::map<std::string, std::vector<Function>> ClassesToFuntions;
class Disassembler {
public:
    Disassembler() = default;
    ~Disassembler() = default;
    std::string Disassemble(const std::filesystem::path& path);
    std::string GetClassName();
    // void RemoveEmptyBlocks(CFG graph);
    // bool RemoveEmptyBlocks(CFG graph, std::shared_ptr<BasicBlock> block);
    std::string OutputHeader(const ClassFile& classFile);
    std::vector<std::string> GetFunctionNames() { return m_ClassFunctions; }
    std::vector<std::string> GetClassNames() { return m_ClassFileNames; }
    std::string DisassembleJarFile(const JarFile& jarFile);
    std::string DisassembleClassFile(const ClassFile& classFile, const std::string& className);

    std::string OutputJarClassHeaders(const ClassFile& classFile);
    ClassesMap GetClassesMap() { return m_ClassesMap; }
    std::vector<Function> GetFunctions() { return m_Functions; }
    ClassesToFuntions GetClassesToFunctions() { return m_ClassToFunctions; }

public:
    CFG BuildCFG(const std::vector<uint8_t>& bytes);
    void CreateFunctions(const ClassFile& classFile, const std::string& className);
    std::string GetConstantUTF8FromClass(const ClassFile& classFile, int idx);

    std::string GetConstantClassFromClass(Function* function, const ClassFile& classFile, int idx);
    std::string GetNameAndTypeFromClass(Function* function, const ClassFile& classFile, int idx);
    std::string GetConstantFromClass(const ClassFile& classFile, int idx);

    std::string ParseFunction(const ClassFile& classFile, Function& function);
    std::string ParseFunctionInstructions(Function& function, const ClassFile& classFile, const std::vector<Insn>& instructions);
    std::string InstructionsWithReferences(Function& function, const ClassFile& classFile, const Insn& insn);
    std::string LoadConstFromPool(const Function& function, const ClassFile& classFile, const Insn& insn);
    std::string ParseIfStatement(const Insn& insn);
    std::string ParseIncrement(const Insn& insn);
    std::string ParseBiPushOpCode(const Insn& insn);
    std::string ParseGoto(const Insn& insn);
    BootstrapMethods GetBootstrapMethod(const ClassFile& classFile);

public:
    std::vector<InsnTempWrapper> m_TempInsn;

private:
    Loader m_Loader;
    ClassFileParser m_ClassFileParser;
    JarFileParser m_JarFileParser;
    ClassFile m_ClassFile;
    JarFile m_JarFile;
    CFG m_Cfg;
    std::string m_Output;
    std::vector<std::string> m_ClassFunctions;
    std::vector<std::string> m_ClassFileNames;
    ClassesMap m_ClassesMap;
    ClassesToFuntions m_ClassToFunctions;
    Function m_Function;
    std::vector<Function> m_Functions;
    

};

}  // namespace TitaniumDecompiler
