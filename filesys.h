#pragma once
// stat works on unix, linux and windows
#include <sys/stat.h>
#include <iostream>
#include <sstream>
#include <string.h>
// only available in c++ 17 and upgraded compiler
// #include <filesystem>

// #include <IOKit>

#ifdef _WIN32 || _WIN64
    #include "resources/dirent.h"
#elif __APPLE__ || __MACH__ || __linux__
    #include <dirent.h>
#endif

class filesys{
public:
    static bool isPngFile(const std::string filepath);
    static void listItemsInDirectoryAlt(const std::string path);
    static bool directoryExists(const std::string dir);
    static bool fileExists(const std::string name);
private:
    filesys(){}
};