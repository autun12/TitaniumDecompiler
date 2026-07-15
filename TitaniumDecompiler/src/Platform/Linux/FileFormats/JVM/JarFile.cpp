#include "JarFile.h"

#include <TitaniumLogger/Logger/Log.h>
#include <stdint.h>

#include <vector>

#include "Utils/zip_file.hpp"

namespace TitaniumDecompiler {
std::vector<uint8_t> stringstreamToVector(std::stringstream& ss) {
    // Convert the contents of the stringstream to a string
    std::string str = ss.str();

    // Create a vector of uint8_t and fill it with the data from the string
    std::vector<uint8_t> data(str.begin(), str.end());

    return data;
}

JarFile JarFileParser::LoadFromJar(const std::filesystem::path& jarPath) {
    miniz_cpp::zip_file f(jarPath.string());
    std::stringstream ss;

    // std::vector<std::string> data = f.namelist();
    std::vector<std::string> data = f.namelist();  // Assume f.namelist() returns a vector<string>

    bool hasClassFile = false;
    for (const auto& item : data) {
        if (item.find(".class") != std::string::npos) {  // Check if ".class" is found in the string
            hasClassFile = true;
            ss.str("");
            ss.clear();
            ss << f.open(item).rdbuf();
            //            std::cout << item << std::endl;
            //            std::cout << ss.str() << std::endl;
            auto jarClass = m_ClassParser.Parse(stringstreamToVector(ss));

            // std::cout << TitaniumDecompiler::GetConstantClass(jarClass.m_ThisClass) << std::endl;
            m_JarFile.m_Classes.push_back(jarClass);

            // std::cout << "Found .class file: " << item << std::endl;
            // break;  // Stop once we find one .class file
        }
    }

    if (!hasClassFile) {
        TD_DECOMP_ERROR("No .class files found.");
    }

    return m_JarFile;
}
}  // namespace TitaniumDecompiler
