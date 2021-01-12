#include "tdpch.h"
#include "TitaniumRenderer/PlatformUtils.h"

#include <GLFW/glfw3.h>
#include "TitaniumRenderer/Application.h"

#define LINUX_PATH_MAX 4096 + 255 + 1

namespace TitaniumRenderer {
bool FileDialogsUtilityCheck();

std::optional<std::string> FileDialogs::OpenFile(const char* filter) {
    if(!FileDialogsUtilityCheck()) {
        return std::nullopt;
    }

    const char* file_extension = std::strchr(filter, 0) + 1;

    FILE *fp;
    char path[LINUX_PATH_MAX];

    std::string cmdline = std::string("zenity --file-selection --modal \"") + std::string(file_extension) + std::string("\" --title=\"Open File\"");
    fp = popen(cmdline.c_str(), "r");
    
    if (fp == NULL) {
        TD_CORE_ERROR("Null file pointer in FileDialogs::OpenFile().\n");
    }

    //Check for File dialog cancellation.
    if(fgets(path, LINUX_PATH_MAX, fp) == NULL) {
        pclose(fp);
        return std::nullopt;
    }

    pclose(fp);

    std::string file_choice = std::string(path);
    int endline_pos = file_choice.find_first_of("\n");
    file_choice.resize(endline_pos);
    
    return file_choice;
}

bool FileDialogsUtilityCheck() {
    FILE *fp;
    char path[LINUX_PATH_MAX];

    fp = popen("which zenity", "r");
    
    if(fp == NULL) {
        TD_CORE_ERROR("Null file pointer in FileDialogs::OpenFile().\n");
    }

    if(fgets(path, LINUX_PATH_MAX, fp) == NULL) {
        TD_CORE_WARN("Zenity not found in path. See TitaniumDecompiler pre-requisites to install.");
        pclose(fp);
        return false;
    }

    pclose(fp);

    return true;
}
}