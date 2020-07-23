#pragma once
// stat works on unix, linux and windows
#include <sys/stat.h>
#include <iostream>
#include <sstream>
#include <string.h>

// load dirent according to OS
#ifdef _WIN32 || _WIN64
    #include "resources/dirent.h"
#elif __APPLE__ || __MACH__ || __linux__
    #include <dirent.h>
#endif

class filesys{
public:
    /**
     * @brief check if file ist png file
     * 
     * @param filepath 
     * @return true 
     * @return false 
     */
    static bool isPngFile(const std::string filepath);
    /**
     * @brief list all items in directory
     * 
     * @param path 
     */
    static void listItemsInDirectoryAlt(const std::string path);
    /**
     * @brief check if directory exists
     * 
     * @param dir 
     * @return true 
     * @return false 
     */
    static bool directoryExists(const std::string dir);
    /**
     * @brief check if file exists
     * 
     * @param name 
     * @return true 
     * @return false 
     */
    static bool fileExists(const std::string name);
private:
    // prevent creating object
    filesys(){}
};