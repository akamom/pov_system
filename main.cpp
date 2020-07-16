#include <iostream>
#include "pixel_reader.h"
#include <Map>

typedef std::map<std::string, std::string> cmdMap_t;

// system specific console commands
int getOs(){
#ifdef _WIN32
    return 1;
#elif _WIN64
    return 2;
#elif __APPLE__ || __MACH__
    return 3;
#elif __linux__
    return 4;
#elif __FreeBSD__
    return 5;
#elif __unix || __unix__
    return 6;
#else
    return 7;
#endif
}

// system spezifische commands
char* listItemsCommand = "";
char* gotoDirectoryCommand = "";

const int os = getOs();
void setCommandIdentifiers(){
    if (os == 1 || os == 2){
        listItemsCommand = "dir";
        gotoDirectoryCommand = "cd ";
    }else if (os == 3 || os == 4){
        listItemsCommand = "ls";
        gotoDirectoryCommand = "cd ";
    }
}

void listItemsInDirectory(){
    if (listItemsCommand == "")
        return;
    std::system(listItemsCommand);
}

void gotoDirectory(char* dir){
    if (gotoDirectoryCommand == "")
        return;
    std::system(strcat(gotoDirectoryCommand, dir));
}

// current path
// TODO: hier noch einen standart pfad angeben!
std::string currentPath = "C:/irgendeinPfadDenEsNichtAufAllenSystemenGibt/";
// all commands will be <rootCommand> + <command>
std::string rootCommand = "pov";
// commands as map
// key: string as command, value: string as description
cmdMap_t commandMap = {
    {"help", "lists all commands"},
    {"goto", "sets the current working path"},
    {"add", "adds image to selection"},
    {"remove", "removes image from selection either by index or by name"},
    {"list", "lists all selected images and index"}
};

void printHelp(){
    std::cout << "POV Help Page\n" << "Current Path: " << currentPath << std::endl;
    std::cout << "Selected Items: \n";
    for (cmdMap_t::iterator it = commandMap.begin(); it != commandMap.end(); ++it){
        std::cout << "\033[1;31m"<< it->first <<": \033[0m" << it->second << std::endl;
    }
}

// siehe unten. So ähnlich aber dann halt in der Map
void* exeHelp = &printHelp;
bool evaluateCommand(std::string cmd){
    // TODO: optionen des Commands wie z.B. pov goto <Pfad> müssen extrahiert werden
    // TODO: in die commandMap iwie void* einfügen und falls cmd == key, dann void* methode aufrufen?
    if (cmd == (rootCommand+"help")){
        //&exeHelp;
        printHelp();
        return true;
    } else if (cmd == (rootCommand+"goto")){
        // test!
        //gotoDirectory("C:/Users/olitw/Downloads");
        currentPath = "C:/Users/olitw/Downloads";
        return true;
    }

    return false;
}

int main(int, char**) {
    if (os > 4)
        return 1;
    setCommandIdentifiers();

    // bilddaten auslesen
    auto pixels = std::vector<color>();
    bool err = pixel::decodeWithState("C:/Users/olitw/Downloads/password-manager-icon.png", pixels);
    
    // print all pixels
    //for(auto col : pixels){
    //    col.print();
    //}
    listItemsInDirectory();

    std::cout << "\033[1menter 'q' to quit. enter 'povhelp' to list available commands\033[0m\n";
    // last user input
    std::string lastInput = "";
    // programm loop
    do{
        std::cout << "<" << currentPath << ">: ";
        std::cin >> lastInput;
        if (lastInput == "q")
            break;
        
        //TODO: bearbeitung des commands
        //TODO: wenn string input durch leerzeichen getrennt gehts net oder was? xD
        auto cmdfound = evaluateCommand(lastInput);
        if (!cmdfound)
            std::cout << "Command"<< lastInput <<" not found\n";

        std::cout << "\n";
    }while(lastInput != "q");
}
