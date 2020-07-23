#pragma once
#include <iostream>
#include <string.h>
#include <vector>

class stringop{
public:
    // https://stackoverflow.com/questions/25829143/trim-whitespace-from-a-string/25829178
    static std::string trimString(const std::string str);
    static void correctPath(std::string &path);
    static std::vector<std::string> split(std::string s, std::string delimiter, bool ignore_inside_string = false);

private:
    stringop(){};
};