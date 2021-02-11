#pragma once

#include <string>

namespace TitaniumRenderer {
class FileDialogs {
public:
    // These return empty strings if cancelled
    static std::string OpenFile(const char* filter);
    // static std::optional<std::string> SaveFile(const char* filter);
};
}