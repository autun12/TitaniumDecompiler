#include "ClassFile.h"
#include <iostream>

ClassFile::ClassFile(const std::filesystem::path& path) { 
    m_Reader.Read(path);
    Parse(m_Reader.GetBuffer());
}

void ClassFile::Parse(std::vector<uint8_t> data) {
    m_Magic = m_Reader.ReadUInt32();
    std::cout << m_Magic << std::endl;
}