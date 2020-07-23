/*
--- Code Quality TODOs
TODO: Code auf mehrere files verteilen (vorallem selection und parser Klasse in cpp und h Dateien aufteilen)
TODO: Console Farben testen und vllt sogar rausnehmen

--- Funktionale TODOs
TODO: list files in directory
TODO: Bluetooth?
TODO: dirent.h auf Windows ausprobieren (list items in directory)
TODO: parameter like onlypng (with no value) or somth for dir coommand so that only png is listed (or for removing all selected)
x TODO: parameter sollten entweder so "set=" oder auch so "s=" ansprechbar sein
*/

#include "filesys.h"
#include "stringop.h"
#include "commandparser.h"
#include "imageselection.h"

// if 'DEBUG' more verbose console
#define DEBUG

// key: commandname, value: description, parameterlist
typedef std::map<std::string, std::pair<std::string, std::vector<std::string>>> cmdMap_t;

// current path
// TODO: hier noch einen standart pfad angeben!
#ifdef DEBUG
std::string currentPath = "/Users/i519401/Development/personal/C++/VPRProjekt/pov_system/resources/";
#else
std::string currentPath = "NO PATH";
#endif
// all commands will be <rootCommand> + <command>
const std::string rootCommand = "pov";
const std::string quitCommand = "q";

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

/*#include <unistd.h>
char cwd;
auto c = getcwd(&cwd, sizeof(cwd))*/

/* class ImageSelection_c{
public:
    ImageSelection_c(){
        // m_pixelSelection = std::map<std::string, std::vector<color>>();
        m_pixelSelection = std::map<std::string, std::string>();
        m_pathSelection = std::map<std::string, std::string>();
    }
    ~ImageSelection_c(){}

    void addImage(const std::string abs_file_path){
        if (m_pixelSelection.size() >= MAX_SELECTED){
            std::cout << "maximum number of selected items reached. Cannot add more than " << MAX_SELECTED << std::endl;
            return;
        }
        #ifdef DEBUG
        std::cout << abs_file_path << std::endl;
        #endif
        
        // file already in selection
        if (m_pixelSelection.find(abs_file_path) != m_pixelSelection.end()){
            std::cout << abs_file_path << " is already selected\n";
            return;
        }

        // extract pixel data
        auto pixels = std::vector<color>();
        bool succeeded = pixel::decodeWithState(abs_file_path.c_str(), pixels);
        // check ob pixel extrahiert werden konnten
        if (!succeeded){
            std::cout << "cannot add " << abs_file_path << " to selection\n";
            return;
        }

        // auto transformed = pixelsToBoolString(pixels);
        auto transformed = pixelsToPixelsString(pixels);

        // add to selection
        //m_pixelSelection.insert({abs_file_path, pixels});
        m_pixelSelection.insert({abs_file_path, transformed});
        std::cout << "added image " << abs_file_path << " to selection\n";
    }
    void addImage(const std::string fileName, const int atIndex){}
    void removeImage(const std::string fileName){}
    void removeImage(const int atIndex){
        if (atIndex >= m_pixelSelection.size()){
            std::cout << "index out of bounds\n";
            return;
        }
        auto index = 0;
        for(std::map<std::string, std::string>::iterator it = m_pixelSelection.begin(); it != m_pixelSelection.end(); ++it){
            if (atIndex == index){
                std::cout << "removed image " << it->first << " from selection\n";
                m_pixelSelection.erase(it);
                return;
            }
            ++index;
        }
    }

    void printSelection(){
        int index = 0;
        std::cout << "---- current image selection: ----\n";
        //for(std::map<std::string, std::vector<color>>::iterator it = m_pixelSelection.begin(); it != m_pixelSelection.end(); ++it){
        //    std::cout << index << ": " << it->first << std::endl;
        //    ++index;
        //}
        for(std::map<std::string, std::string>::iterator it = m_pixelSelection.begin(); it != m_pixelSelection.end(); ++it){
            std::cout << index << ": " << it->first << "\nFarben: " << it->second << std::endl;
            ++index;
        }
        std::cout << "----------------------------------\n";
    }
private:
    std::map<std::string, std::string>          m_pathSelection;
    // selection of images. key: path+imagename, value: conversed pixels
    std::map<std::string, std::string>  m_pixelSelection;
    // std::map<std::string, std::vector<color>>   m_pixelSelection;

    const size_t MAX_SELECTED = 1;

    std::string pixelsToBoolString(std::vector<color> pix){
        //auto transformed = std::vector<bool>();
        std::string asstring = "";
        for (auto &a:pix){
            size_t rgbscalar = a.R + a.B + a.G; // ignore alpha
            bool t = rgbscalar != 0;
            //transformed.push_back(t); // only if black then 0, all other colors will be 1
            //asstring.push_back(t ? '\u25fb' : '\u25fc');
            asstring.push_back(t ? '1' : '0');
            asstring.push_back(';');
        }
        //return transformed;
        return asstring;
    }

    std::string pixelsToPixelsString(std::vector<color> pix){
        std::string asstring = "";
        for (auto &a:pix){
            std::ostringstream os;
            os << a.R << "," << a.G << "," << a.B << ";";
            asstring.append(os.str());
        }
        // append ending char
        asstring.push_back('.');
        return asstring;
    }
}; */

/*
class ParameterParser_c{
public:
    std::string baseCommand;
    std::string originalCommand;

    ParameterParser_c(std::string _cmd) : originalCommand(_cmd), baseCommand(""){
        m_parameters = std::map<std::string, std::string>();
        getParametersFromCommand();
    }

    ~ParameterParser_c(){}

    bool getParameter(std::string param_name, std::string &param_value, std::string alt_param_name = "") const{
        if (m_parameters.empty())
            return false;
        std::map<std::string, std::string>::const_iterator it = m_parameters.find(param_name); //m_parameters.at(param_name);
        // meaning key non existent
        if (it == m_parameters.end()){
            it = m_parameters.find(alt_param_name);
            auto alt_param_empty = alt_param_name.empty();
            if (!alt_param_empty && it != m_parameters.end()){
                param_value = it->second;
                return true;
            }
            std::cout << "parameter " << param_name << (alt_param_empty ? "" : " or alternatively ") << (alt_param_empty ? "" : alt_param_name) <<  " does not exist!\n";
            return false;
        }
        param_value = it->second;
        return true;
    }
private:
    std::map<std::string, std::string> m_parameters;

    const std::string splitSymbol = "=";
    void getParametersFromCommand(){
        auto rawSplit = stringop::split(this->originalCommand, " ");
        // not even baseCommand input
        #ifdef DEBUG
        for (auto e : rawSplit){
            std::cout << e << std::endl;
        }
        #endif
        if (rawSplit.size() < 2)
            return;
        // first 2 are mandatory commands
        // baseCommand is mandatory 2 commands
        this->baseCommand = (stringop::trimString(rawSplit[0]) + " " + stringop::trimString(rawSplit[1]));
        // if only basecommand exists
        if (rawSplit.size() <= 2)
            return;

        for(std::vector<std::string>::iterator it = rawSplit.begin()+2; it != rawSplit.end(); ++it){
            auto paramSplit = stringop::split(*it, this->splitSymbol, true);
            #ifdef DEBUG
            std::cout << "param: " << *it << std::endl;
            for (auto e : paramSplit){
                std::cout << "paramsplit " << e << std::endl;
            }
            #endif
            if (paramSplit.size() > 2)
                continue;
            auto name = stringop::trimString(paramSplit.at(0));
            std::string value;
            if (paramSplit.size() == 1)
                value = "NONE";
            else
                value = stringop::trimString(paramSplit.at(1));
            #ifdef DEBUG
            std::cout << "found Parameter: " << name << "='" << value << "'"<< std::endl;
            #endif
            m_parameters.insert({name, value});
        }
    }
};
*/

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

// commands as map (only used in help screen!)
// key: string as command, value: string as description
cmdMap_t commandMap = {
    {"help", {"lists all commands", {}}},
    {"dir", {"sets the current working path or lists items in current dir", {"optional: <set/s=[path]>: path as value, sets working dir"}}},
    {"add", {"adds image to selection", {"mandatory: <file/f=[filename]>: file in current path"}}},
    {"remove", {"removes image from selection either by index or by name", {"mandatory: <index/i=[index]>: which index to delete from selection"}}},
    {"status", {"lists all selected images and index", {}}},
    {"start", {"uploads selection to arduino", {}}}
};

void printHelp(){
    std::cout << "-----------POV Help Page------------\n" << "Current Path: " << currentPath << std::endl;
    for (cmdMap_t::iterator it = commandMap.begin(); it != commandMap.end(); ++it){
        std::cout << "\033[1;31m"<< it->first <<": \033[0m" << it->second.first << std::endl;
        for (int i = 0; i<it->second.second.size(); i++){
            std::cout << "\t" << it->second.second[i] << std::endl;
        }
    }
    std::cout << "--------------------------------------\n";
}

auto selection = ImageSelection_c();

bool evaluateCommand(std::string cmd){
    auto paramParser = ParameterParser_c(cmd);
    auto base = paramParser.baseCommand;
    #ifdef DEBUG
        std::cout << base << std::endl;
    #endif
    // TODO: optionen des Commands wie z.B. pov goto <Pfad> müssen extrahiert werden
    // TODO: in die commandMap iwie void* einfügen und falls cmd == key, dann void* methode aufrufen?
    if (base == (rootCommand+" help")){
        //(void)*exeHelp();
        printHelp();
        return true;
    } else if (base == (rootCommand+" dir")){
        std::string path = "";
        bool gotValue = paramParser.getParameter("set", path, "s");
        // if error, parameter was not found, meaning invalid parameter command
        if (gotValue){
            // set path param passed
            stringop::correctPath(path);
            auto exists = filesys::directoryExists(path);
            if (exists){
                currentPath = path;
                filesys::listItemsInDirectoryAlt(path);
            }
        }else{
            // no params passed -> list all items in current dir
            filesys::listItemsInDirectoryAlt(currentPath);
        }
        return true;
    } else if (base == (rootCommand+" add")){
        std::string filename = "";
        bool gotValue = paramParser.getParameter("file", filename, "f");
        if (!gotValue)
            return false;
        auto splitfilename = stringop::split(filename, ".");
        auto fileextension = splitfilename[splitfilename.size()-1];
        //auto fileextension = *(splitfilename.end()-1);
        #ifdef DEBUG
        for (auto a : splitfilename){
            std::cout << a << std::endl;
        }
        #endif
        if (fileextension != "png")
            filename += ".png";
        #ifdef DEBUG
            std::cout << fileextension << std::endl;
            std::cout << filename << std::endl;
        #endif
        // build full pathname
        auto completeFileName = stringop::trimString(currentPath + filename);
        auto exists = filesys::fileExists(completeFileName);
        // file existent?
        if (!exists){
            std::cout << completeFileName << " does not exist!\n";
        }else
            selection.addImage(completeFileName);
        return true;
    } else if (base == (rootCommand+" remove")){
        std::string index;
        bool gotValue = paramParser.getParameter("index", index, "i");
        if (!gotValue)
            return false;
        #ifdef DEBUG
        std::cout << index << std::endl;
        #endif
        int index_from_string = std::stoi(index);
        selection.removeImage(index_from_string);
        return true;
    } else if (base == (rootCommand+" status")){
        selection.printSelection();
        return true;
    } else if (base == (rootCommand+" start")){
        std::cout << "starting upload to arduino\n";
        return true;
    }

    return false;
}

int main(int, char**) {
    if (os > 4)
        return 1;
    setCommandIdentifiers();

    std::cout << "\033[1m >>>>> POV Enviroment Start\n";
    std::cout << "enter '"<< quitCommand <<"' to quit. enter 'pov help' to list available commands\033[0m\n";
    std::cout << "\033[33m INFO: please input path and filenames enclosed by quotation marks (\")\033[0m\n";
    // last user input
    std::string lastInput = "";
    // programm loop
    do{
        std::cout << "\033[1;32m<" << currentPath << ">: \033[0m";
        std::getline(std::cin, lastInput);
        lastInput = stringop::trimString(lastInput);
        if (lastInput == quitCommand)
            break;
        
        auto cmdfound = evaluateCommand(lastInput);
        if (!cmdfound)
            std::cout << "Command "<< lastInput <<" not found\n";

    }while(lastInput != quitCommand);
    std::cout << "\033[1mQuitting...\n";
    std::cout << ">>>>> POV Enviroment Exit\033[0m\n";
}
