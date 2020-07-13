#include <string>
#include <iostream>

using namespace std;

static struct s_colorValues
{
    int red, green, blue;
};

#ifdef _WIN32 || _WIN64
#include "Windows.h"
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#elif __APPLE__ || __MACH__
#elif __linux__
#else
#endif

/*
 * GET OS: https://stackoverflow.com/questions/15580179/how-do-i-find-the-name-of-an-operating-system
 * LINUX: https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
 * WINDOWS: https://stackoverflow.com/questions/4053837/colorizing-text-in-the-console-with-c
 * MAC: https://stackoverflow.com/questions/33309136/change-color-in-os-x-console-output
 * ANSI COLORS (for all OS): https://en.wikipedia.org/wiki/ANSI_escape_code
 */

std::string getOsName()
{
#ifdef _WIN32
    return "Windows 32-bit";
#elif _WIN64
    return "Windows 64-bit";
#elif __APPLE__ || __MACH__
    return "Mac OSX";
#elif __linux__
    return "Linux";
#elif __FreeBSD__
    return "FreeBSD";
#elif __unix || __unix__
    return "Unix";
#else
    return "Other";
#endif
}

int main()
{
    //string os = getOsName();
    cout << "\033[1;31mbold red text\033[0m\n";
	
    return 1;
}
