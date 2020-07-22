/*
--- Code Quality TODOs
TODO: Code auf mehrere files verteilen (vorallem selection und parser Klasse in cpp und h Dateien aufteilen)
TODO: Console Farben testen und vllt sogar rausnehmen

--- Funktionale TODOs
TODO: list files in directory
*/

#include <iostream>
#include "pixel_reader.h"
#include <Map>
// stat works on unix, linux and windows
#include <sys/stat.h>

// if 'debug' more verbose console
#define DEBUG

// key: commandname, value: description, parameterlist
typedef std::map<std::string, std::pair<std::string, std::vector<std::string>>> cmdMap_t;

// current path
// TODO: hier noch einen standart pfad angeben!
std::string currentPath = "C:/";
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

// https://stackoverflow.com/questions/18100097/portable-way-to-check-if-directory-exists-windows-linux-c
inline bool directoryExists(const std::string &dir){
    struct stat info;
    char *pathname = strdup(dir.c_str());

    if( stat( pathname, &info ) != 0 ){
        std::cout << pathname << " cannot be accessed!\n";
        return false;
    }
    else if( info.st_mode & S_IFDIR )  // S_ISDIR() doesn't exist on my windows 
        return true;
    else{
        std::cout << pathname << " is not directory!\n";
        return false;
    }
}

// https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
inline bool fileExists(const std::string &name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

// https://stackoverflow.com/questions/25829143/trim-whitespace-from-a-string/25829178
std::string trimString(const std::string &str)
{
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

void correctPath(std::string &path) {
    // replaces all occurences of \ with /, and removes multiple backslashes
    char lastChar = 'a';
    const char ch1 = '\\';
    const char ch2 = '/';
    for (int i = 0; i < path.length(); ++i) {
        if (lastChar == ch1 && path[i] == ch1){
            // remove double backslashes
            //path[i] = (char)0;
            path.erase(i, 1);
            continue;
        }
        lastChar = path[i];
        if (path[i] == ch1){
            // replace backslash with slash
            path[i] = ch2;
        }
    }
    if (path[path.length()-1] != '/')
        path.append("/");
}

std::vector<std::string> split(std::string s, std::string delimiter, bool ignore_inside_string = false){
	const auto string_space_replacement = '$';
    // automatically check if there is a need to check for strings
    if (s.find('"') == std::string::npos)
        ignore_inside_string = true;

	if (!ignore_inside_string)
	{
		// replace space inside string with replacement char
		auto is_inside_string = false;
		for(auto i = 0; i<s.length(); i++)
		{
			const auto c = s[i];
			if (c == '"')
			{
				is_inside_string = !is_inside_string;
				s.erase(i, 1);
			}
			if (c == ' ' && is_inside_string)
				s[i] = string_space_replacement;
		}
	}

	// append delimiter to iterate through all split string not all-1
	s.append(delimiter);
	std::vector<std::string> list;
	size_t pos;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		auto token = s.substr(0, pos);
		if (!token.empty())
		{
			if (!ignore_inside_string)
			{
				// replace replacement char with space again
				size_t space_replace_pos;
				while ((space_replace_pos = token.find(string_space_replacement)) != std::string::npos)
				{
					token[space_replace_pos] = ' ';
				}
			}
			list.push_back(token);
		}
		s.erase(0, pos + delimiter.length());
	}
	return list;
}

class ImageSelection_c{
public:
    ImageSelection_c(){
        // m_pixelSelection = std::map<std::string, std::vector<color>>();
        m_pixelSelection = std::map<std::string, std::string>();
        m_pathSelection = std::map<std::string, std::string>();
    }
    ~ImageSelection_c(){}

    void addImage(const std::string fileName){
        // build full pathname
        auto completeFileName = trimString(currentPath + fileName);
        auto exists = fileExists(completeFileName);
        // file existent?
        if (!exists){
            std::cout << completeFileName << " does not exist!\n";
            return;
        }
        // file already in selection
        if (m_pixelSelection.find(completeFileName) != m_pixelSelection.end()){
            std::cout << completeFileName << " is already selected\n";
            return;
        }

        // extract pixel data
        auto pixels = std::vector<color>();
        bool succeeded = pixel::decodeWithState(completeFileName.c_str(), pixels);
        // check ob pixel extrahiert werden konnten
        if (!succeeded){
            std::cout << "cannot add " << completeFileName << " to selection\n";
            return;
        }

        auto tranformed = pixelsToBools(pixels);

        // add to selection
        //m_pixelSelection.insert({completeFileName, pixels});
        m_pixelSelection.insert({completeFileName, tranformed});
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
                m_pixelSelection.erase(it);
                break;
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
            std::cout << index << ": " << it->first << " Farben: " << it->second << std::endl;
            ++index;
        }
        std::cout << "----------------------------------\n";
    }
private:
    std::map<std::string, std::string>          m_pathSelection;
    // selection of images. key: path+imagename, value: conversed pixels
    std::map<std::string, std::string>  m_pixelSelection;
    // std::map<std::string, std::vector<color>>   m_pixelSelection;

    std::string pixelsToBools(std::vector<color> pix){
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
};

class ParameterParser_c{
public:
    std::string baseCommand;
    std::string originalCommand;

    ParameterParser_c(std::string _cmd) : originalCommand(_cmd), baseCommand(""){
        m_parameters = std::map<std::string, std::string>();
        getParametersFromCommand();
    }

    ~ParameterParser_c(){}

    bool getParameter(std::string param_name, std::string &param_value) const{
        if (m_parameters.empty())
            return false;
        std::map<std::string, std::string>::const_iterator it = m_parameters.find(param_name); //m_parameters.at(param_name);
        // meaning key non existent
        if (it == m_parameters.end()){
            std::cout << "parameter " << param_name << " does not exist!\n";
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
        #ifdef DEBUG
        for (auto e : rawSplit){
            std::cout << e << std::endl;
        }
        #endif
        if (rawSplit.size() < 2)
            return;
        // first 2 are mandatory commands
        // baseCommand is mandatory 2 commands
        this->baseCommand = (trimString(rawSplit[0]) + " " + trimString(rawSplit[1]));
        // if only basecommand exists
        if (rawSplit.size() <= 2)
            return;

        for(std::vector<std::string>::iterator it = rawSplit.begin()+2; it != rawSplit.end(); ++it){
            auto paramSplit = split(*it, this->splitSymbol, true);
            #ifdef DEBUG
            std::cout << "param: " << *it << std::endl;
            for (auto e : paramSplit){
                std::cout << "paramsplit " << e << std::endl;
            }
            #endif
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

// commands as map (only used in help screen!)
// key: string as command, value: string as description
cmdMap_t commandMap = {
    {"help", {"lists all commands", {}}},
    {"goto", {"sets the current working path", {"optional: <path=[path]>: path as value"}}},
    {"add", {"adds image to selection", {"mandatory: <file=[filename]>: file in current path", "optional: <index=[index]>: at which index to insert"}}},
    {"remove", {"removes image from selection either by index or by name", {}}},
    {"status", {"lists all selected images and index", {}}}
};

void printHelp(){
    std::cout << "-----------POV Help Page------------\n" << "Current Path: " << currentPath << std::endl;
    std::cout << "Selected Items: \n";
    for (cmdMap_t::iterator it = commandMap.begin(); it != commandMap.end(); ++it){
        std::cout << "\033[1;31m"<< it->first <<": \033[0m" << it->second.first << std::endl;
        for (int i = 0; i<it->second.second.size(); i++){
            std::cout << "\t" << it->second.second[i] << std::endl;
        }
    }
    std::cout << "--------------------------------------\n";
}

auto selection = ImageSelection_c();

// siehe unten. So ähnlich aber dann halt in der Map
void* exeHelp = &printHelp;
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
    } else if (base == (rootCommand+" goto")){
        // test!
        //gotoDirectory("C:/Users/olitw/Downloads");
        std::string path = "";
        bool gotValue = paramParser.getParameter("path", path);
        std::cout << "goto directory: "<< path << std::endl;
        // if error, parameter was not found, meaning invalid parameter command
        if (!gotValue)
            return false;
        // else path command is there and we use the value
        // TODO: check ob pfad existiert
        correctPath(path);
        auto exists = directoryExists(path);
        if (exists)
            currentPath = path;
        //listItemsInDirectory(strdup(path.c_str()));
        return true;
    } else if (base == (rootCommand+" add")){
        std::string filename = "";
        bool gotValue = paramParser.getParameter("file", filename);
        if (!gotValue)
            return false;
        auto splitfilename = split(filename, ".");
        auto fileextension = splitfilename[splitfilename.size()-1];
        //auto fileextension = *(splitfilename.end()-1);
        for (auto a : splitfilename){
            std::cout << a << std::endl;
        }
        if (fileextension != "png")
            filename += ".png";
        #ifdef DEBUG
            std::cout << fileextension << std::endl;
            std::cout << filename << std::endl;
        #endif
        
        selection.addImage(filename);
        return true;
    } else if (base == (rootCommand+" remove")){
        std::string index;
        bool gotValue = paramParser.getParameter("i", index);
        if (!gotValue)
            return false;
        int index_from_string = std::stoi(index);
        selection.removeImage(index_from_string);
        return true;
    } else if (base == (rootCommand+" status")){
        selection.printSelection();
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

    /*
    std::string pat = "D:\\1STUDIUM\\Semester3_klausuren.pdf";
    correctPath(pat);
    std::cout << fileExists(pat) << std::endl;
    std::cout << fileExists("D:/1STUDIUM/Semester3_klauafqfsuren.pdf") << std::endl;
    std::cout << directoryExists("D:/1STUDIUM") << std::endl;
    std::cout << directoryExists("D:/1STUDIUM qgfqg") << std::endl;
    return 1;
    */

    std::cout << "\033[1m >>>>> POV Enviroment Start\n";
    std::cout << "enter '"<< quitCommand <<"' to quit. enter 'pov help' to list available commands\033[0m\n";
    // last user input
    std::string lastInput = "";
    // programm loop
    do{
        std::cout << "\033[1;32m<" << currentPath << ">: \033[0m";
        std::getline(std::cin, lastInput);
        lastInput = trimString(lastInput);
        if (lastInput == quitCommand)
            break;
        
        //TODO: bearbeitung des commands
        auto cmdfound = evaluateCommand(lastInput);
        if (!cmdfound)
            std::cout << "Command "<< lastInput <<" not found\n";

    }while(lastInput != quitCommand);
    std::cout << "\033[1mQuitting...\n";
    std::cout << ">>>>> POV Enviroment Exit\033[0m\n";
}
