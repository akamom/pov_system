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

// https://stackoverflow.com/questions/25829143/trim-whitespace-from-a-string/25829178
std::string trimString(std::string& str)
{
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

std::vector<std::string> split(std::string s, std::string delimiter){
    std::vector<std::string> list;
    // for char *phrase:
    //std::string s = std::string(phrase);
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        if (token != "")
            list.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    list.push_back(s);
    return list;
}

class ParameterParser_c{
public:
    std::string baseCommand;
    std::string originalCommand;

    ParameterParser_c(std::string _cmd) : originalCommand(_cmd), baseCommand(""){
        m_parameters = std::map<std::string, std::string>();
        getParametersFromCommand();
    }

    bool getParameter(std::string param_name, std::string &param_value) const{
        if (m_parameters.empty())
            return false;
        std::map<std::string, std::string>::const_iterator it = m_parameters.find(param_name); //m_parameters.at(param_name);
        // meaning key non existent
        if (it == m_parameters.end()){
            std::cout << "der parameter " << param_name << " existiert nicht!\n";
            return false;
        }
        param_value = it->second;
        return true;
    }
private:
    std::map<std::string, std::string> m_parameters;

    const std::string splitSymbol = "=";
    void getParametersFromCommand(){
        auto rawSplit = split(this->originalCommand, " ");
        // not even baseCommand input
        if (rawSplit.size() < 2)
            return;
        // first 2 are mandatory commands
        // baseCommand is mandatory 2 commands
        this->baseCommand = (trimString(rawSplit[0]) + " " + trimString(rawSplit[1]));
        // if only basecommand exists
        if (rawSplit.size() <= 2)
            return;

        //rawSplit.erase(rawSplit.begin());
        //rawSplit.erase(rawSplit.begin()+1);
        for(std::vector<std::string>::iterator it = rawSplit.begin()+2; it != rawSplit.end(); ++it){
            auto paramSplit = split(*it, this->splitSymbol);
            if (paramSplit.size() != 2)
                continue;
            auto name = trimString(paramSplit.at(0));
            auto value = trimString(paramSplit.at(1));
            std::cout << "found Parameter: " << name << "='" << value << "'"<< std::endl;
            m_parameters.insert({name, value});
        }
    }
};

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

void listItemsInDirectory(char* dir){
    if (listItemsCommand == "")
        return;
    std::system(strcat(listItemsCommand, dir));
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
const std::string rootCommand = "pov";
const std::string quitCommand = "q";
// commands as map (only used in help screen!)
// key: string as command, value: string as description
cmdMap_t commandMap = {
    {"help", "lists all commands"},
    {"goto", "sets the current working path"},
    {"add", "adds image to selection"},
    {"remove", "removes image from selection either by index or by name"},
    {"list", "lists all selected images and index"}
};

void printHelp(){
    std::cout << "-----------POV Help Page------------\n" << "Current Path: " << currentPath << std::endl;
    std::cout << "Selected Items: \n";
    for (cmdMap_t::iterator it = commandMap.begin(); it != commandMap.end(); ++it){
        std::cout << "\033[1;31m"<< it->first <<": \033[0m" << it->second << std::endl;
    }
    std::cout << "--------------------------------------\n";
}

// siehe unten. So ähnlich aber dann halt in der Map
void* exeHelp = &printHelp;
bool evaluateCommand(std::string cmd){
    auto paramParser = ParameterParser_c(cmd);
    auto base = paramParser.baseCommand;
    std::cout << base << std::endl;
    // TODO: optionen des Commands wie z.B. pov goto <Pfad> müssen extrahiert werden
    // TODO: in die commandMap iwie void* einfügen und falls cmd == key, dann void* methode aufrufen?
    if (base == (rootCommand+" help")){
        //(void)*exeHelp();
        printHelp();
        return true;
    } else if (base == (rootCommand+" goto")){
        // test!
        //gotoDirectory("C:/Users/olitw/Downloads");
        std::string path = "";
        bool gotValue = paramParser.getParameter("path", path);
        std::cout << path << std::endl;
        // if error, parameter was not found, meaning invalid parameter command
        if (!gotValue)
            return false;
        // else path command is there and we use the value
        // TODO: check ob pfad existiert
        currentPath = path;
        //listItemsInDirectory(strdup(path.c_str()));
        return true;
    } else if (base == (rootCommand+" add")){
        return true;
    } else if (base == (rootCommand+" remove")){
        return true;
    }

    return false;
}

int main(int, char**) {
    if (os > 4)
        return 1;
    setCommandIdentifiers();

    /*
    // bilddaten auslesen
    auto pixels = std::vector<color>();
    bool err = pixel::decodeWithState("C:/Users/olitw/Downloads/password-manager-icon.png", pixels);
    
    // print all pixels
    //for(auto col : pixels){
    //    col.print();
    //}
    listItemsInDirectory();
    */

   //for (auto str : split("  pov    is coool", " ")){
   //    std::cout << "'" << str << "'\n";
   //}

    std::cout << "\033[1menter '"<< quitCommand <<"' to quit. enter 'pov help' to list available commands\033[0m\n";
    // last user input
    std::string lastInput = "";
    // programm loop
    do{
        //std::cout << "\033[1;32m<" << currentPath << ">: \033[0m";
        std::cout << "\033[1;32m<" << currentPath << ">: \033[0m";
        std::getline(std::cin, lastInput);
        lastInput = trimString(lastInput);
        if (lastInput == quitCommand)
            break;
        
        //TODO: bearbeitung des commands
        auto cmdfound = evaluateCommand(lastInput);
        if (!cmdfound)
            std::cout << "Command "<< lastInput <<" not found\n";

        std::cout << "\n";
    }while(lastInput != quitCommand);
    std::cout << "Quitting";
}
