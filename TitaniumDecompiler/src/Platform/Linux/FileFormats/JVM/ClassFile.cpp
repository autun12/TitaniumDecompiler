#include "ClassFile.h"
#include <iostream>
#include "../../../../Utils/FileReaderError.h"

namespace TitaniumDecompiler {

ClassFile::ClassFile() {}

void ClassFile::OpenClassFile(const std::filesystem::path& path) { 
    m_Reader.Read(path);
    Parse(m_Reader.GetBuffer());
}

void ClassFile::Parse(std::vector<uint8_t> data) {
    m_Magic = m_Reader.ReadUInt32();
    
    if(m_Magic != 0xCAFEBABE) {
        throw FileReaderError("Class file does not start with magic bytes");
    }

    m_Minor = m_Reader.ReadUInt16();
    m_Major = m_Reader.ReadUInt16();
    
}
}