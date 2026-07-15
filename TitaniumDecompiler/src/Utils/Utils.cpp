#include "Utils.h"

namespace TitaniumDecompiler {
std::string getLastClassNamePart(const std::string& full_class_name) {
    size_t pos = full_class_name.find_last_of('/');
    if (pos != std::string::npos) {
        return full_class_name.substr(pos + 1);  // Extract substring after last '/'
    }
    return full_class_name;  // If no '/' is found, return the whole string
}
}  // namespace TitaniumDecompiler