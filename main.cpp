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

#include <iostream>
#include <sstream>
#include "pixel_reader.h"
#include <Map>
#include "filesys.h"

// set to 'DEBUG' for more verbose output
#define PROD

// typedefinition for map that holds commands, description and parameters
// key: commandname, value: description, parameterlist
typedef std::map<std::string, std::pair<std::string, std::vector<std::string>>> cmdMap_t;

// cwd in pov enviroment
#ifdef DEBUG
std::string currentPath = "/Users/i519401/Development/personal/C++/VPRProjekt/pov_system/resources/";
#else
std::string currentPath = "NO PATH";
#endif

// rootcommand for all other commands, like a prefix
const std::string rootCommand = "pov";
// command for quitting the main loop
const std::string quitCommand = "q";

// get operation system
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

/**
 * @brief cuts whitespaces from left and right
 * Source: https://stackoverflow.com/questions/25829143/trim-whitespace-from-a-string/25829178
 * @param str string to be trimmed
 * @return std::string trimmed string
 */
std::string trimString(const std::string str)
{
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

/**
 * @brief replaces backslashes with slashes and sets slash to end if not there
 * 
 * @param path path to be corrected
 */
void correctPath(std::string &path) {
    // used characters
    char lastChar = 'a';
    const char ch1 = '\\';
    const char ch2 = '/';
    // loop through all chars in path
    for (int i = 0; i < path.length(); ++i) {
        if (lastChar == ch1 && path[i] == ch1){
            // remove double backslashes
            path.erase(i, 1);
            continue;
        }
        lastChar = path[i];
        if (path[i] == ch1){
            // replace backslash with slash
            path[i] = ch2;
        }
    }
    // append slash if not there
    if (path[path.length()-1] != '/')
        path.append("/");
}

/**
 * @brief advanced spliiting method
 * 
 * @param s to be split string
 * @param delimiter split by which character
 * @param ignore_inside_string if false, will not split parts in string eclosed by "", only works if delimiter is one space
 * @return std::vector<std::string> the split parts of the string
 */
std::vector<std::string> split(std::string s, std::string delimiter, bool ignore_inside_string = false){
    // temporary replacement char, should be somethin unique
	const auto string_space_replacement = '$';
    // automatically check if there is a need to check for strings
    if (s.find('"') == std::string::npos || delimiter != " ")
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
    // loop through all found delimiter chars
	while ((pos = s.find(delimiter)) != std::string::npos) {
        // create string from 0 to current delimiter pos
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
            // append to return list
			list.push_back(token);
		}
        // remove from char 0 to last position
		s.erase(0, pos + delimiter.length());
	}
	return list;
}

/**
 * @brief selection of images and handler for that
 * 
 */
class ImageSelection_c{
public:
    ImageSelection_c(){
        // init
        m_pixelSelection = std::map<std::string, std::string>();
    }
    ~ImageSelection_c(){}

    /**
     * @brief adds image by filename
     * 
     * @param fileName name of file
     */
    void addImage(const std::string fileName){
        if (m_pixelSelection.size() >= MAX_SELECTED){
            std::cout << "maximum number of selected items reached. Cannot add more than " << MAX_SELECTED << std::endl;
            return;
        }
        // build full pathname
        auto completeFileName = trimString(currentPath + fileName);
        auto exists = filesys::fileExists(completeFileName);
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

        // get correct sending format
        auto transformed = pixelsToPixelsString(pixels);

        // add to selection
        m_pixelSelection.insert({completeFileName, transformed});
        std::cout << "added image " << fileName << " to selection\n";
    }
    
    /**
     * @brief removes image from selection by index
     * 
     * @param atIndex index of item (visible when typed pov status)
     */
    void removeImage(const int atIndex){
        if (atIndex >= m_pixelSelection.size()){
            std::cout << "index out of bounds\n";
            return;
        }
        auto index = 0;
        for(std::map<std::string, std::string>::iterator it = m_pixelSelection.begin(); it != m_pixelSelection.end(); ++it){
            if (atIndex == index){
                m_pixelSelection.erase(it);
                std::cout << "removed image " << it->first << " from selection\n";
                return;
            }
            ++index;
        }
    }

    /**
     * @brief pretty prints all items in selection
     * 
     */
    void printSelection(){
        int index = 0;
        std::cout << "---- current image selection: ----\n";
        for(std::map<std::string, std::string>::iterator it = m_pixelSelection.begin(); it != m_pixelSelection.end(); ++it){
            std::cout << index << ": " << it->first << "\nFarben: " << it->second << std::endl;
            ++index;
        }
        std::cout << "----------------------------------\n";
    }
private:
    // all selected items are stored here
    std::map<std::string, std::string> m_pixelSelection;

    // maximum selected items possible
    const size_t MAX_SELECTED = 1;

    /**
     * @brief converts pixel RGB list to string with bools, seperated by char
     * 3 values RGB > 1 value bool
     * @param pix list of pixels from image
     * @return std::string 
     */
    std::string pixelsToBoolString(std::vector<color> pix){
        std::string asstring = "";
        for (auto &a:pix){
            size_t rgbscalar = a.R + a.B + a.G; // ignore alpha
            bool t = rgbscalar != 0;
            asstring.push_back(t ? '1' : '0');
            asstring.push_back(';');
        }
        //return transformed;
        return asstring;
    }

    /**
     * @brief turns pixel RGB list into string with RGB values
     * 
     * @param pix pixel lsit
     * @return std::string 
     */
    std::string pixelsToPixelsString(std::vector<color> pix){
        std::string asstring = "";
        for (auto &a:pix){
            // build string containing RGB
            std::ostringstream os;
            os << a.R << "," << a.G << "," << a.B << ";";
            asstring.append(os.str());
        }
        // append ending char
        asstring.push_back('.');
        return asstring;
    }
};

/**
 * @brief string parameter parser
 * 
 */
class ParameterParser_c{
public:
    // the base (consisting of 2 words: <root><action cmd>)
    std::string baseCommand;
    // the original input
    std::string originalCommand;

    ParameterParser_c(std::string _cmd) : originalCommand(_cmd), baseCommand(""){
        m_parameters = std::map<std::string, std::string>();
        getParametersFromCommand();
    }

    ~ParameterParser_c(){}

    /**
     * @brief checks whether string contains a parameter and returns it's value
     * 
     * @param param_name the parameters name
     * @param param_value the to be returned value
     * @param alt_param_name an alternative parameter name (optional)
     * @return true the parameter exists
     * @return false does not exist
     */
    bool getParameter(std::string param_name, std::string &param_value, std::string alt_param_name = "") const{
        if (m_parameters.empty())
            return false;
        std::map<std::string, std::string>::const_iterator it = m_parameters.find(param_name);
        // meaning key non existent
        if (it == m_parameters.end()){
            it = m_parameters.find(alt_param_name);
            auto alt_param_empty = alt_param_name.empty();
            if (!alt_param_empty && it != m_parameters.end()){
                // alt param exists, return value
                param_value = it->second;
                return true;
            }
            // param doesnt exist
            std::cout << "parameter " << param_name << (alt_param_empty ? "" : " or alternatively ") << (alt_param_empty ? "" : alt_param_name) <<  " does not exist!\n";
            return false;
        }
        // param exists, return value
        param_value = it->second;
        return true;
    }
private:
    // the list of parameters
    std::map<std::string, std::string> m_parameters;
    
    // parameter <splitSymbol> value
    const std::string splitSymbol = "=";
    /**
     * @brief parses string and retrieves all parameters and base command
     * 
     */
    void getParametersFromCommand(){
        auto rawSplit = split(this->originalCommand, " ");
        #ifdef DEBUG
        for (auto e : rawSplit){
            std::cout << e << std::endl;
        }
        #endif
        // not even base exists, cannot parse
        if (rawSplit.size() < 2)
            return;
        this->baseCommand = (trimString(rawSplit[0]) + " " + trimString(rawSplit[1]));
        // if only basecommand exists
        if (rawSplit.size() <= 2)
            return;

        // retrieve all params
        for(std::vector<std::string>::iterator it = rawSplit.begin()+2; it != rawSplit.end(); ++it){
            auto paramSplit = split(*it, this->splitSymbol, true);
            #ifdef DEBUG
            std::cout << "param: " << *it << std::endl;
            for (auto e : paramSplit){
                std::cout << "paramsplit " << e << std::endl;
            }
            #endif
            if (paramSplit.size() > 2)
                continue;
            auto name = trimString(paramSplit.at(0));
            std::string value;
            if (paramSplit.size() == 1)
                value = "NONE";
            else
                value = trimString(paramSplit.at(1));
            #ifdef DEBUG
            std::cout << "found Parameter: " << name << "='" << value << "'"<< std::endl;
            #endif
            m_parameters.insert({name, value});
        }
    }
};

#pragma region not_used
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

#pragma endregion

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

/**
 * @brief prints all commands
 * 
 */
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

// current selection, later multiple selection can be implemented
auto selection = ImageSelection_c();

/**
 * @brief parses command and acts
 * 
 * @param cmd input by user
 * @return true command exists
 * @return false doesnt exist
 */
bool evaluateCommand(std::string cmd){
    // parse cmd
    auto paramParser = ParameterParser_c(cmd);
    // load base command
    auto base = paramParser.baseCommand;
    #ifdef DEBUG
        std::cout << base << std::endl;
    #endif
    if (base == (rootCommand+" help")){
        // print commands
        printHelp();
        return true;
    } else if (base == (rootCommand+" dir")){
        std::string path = "";
        bool gotValue = paramParser.getParameter("set", path, "s");
        // if error, parameter was not found, meaning invalid parameter command
        if (gotValue){
            // set path param passed
            correctPath(path);
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
        // adds file to selection
        std::string filename = "";
        bool gotValue = paramParser.getParameter("file", filename, "f");
        if (!gotValue)
            return false;
        auto splitfilename = split(filename, ".");
        auto fileextension = splitfilename[splitfilename.size()-1];
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
        
        selection.addImage(filename);
        return true;
    } else if (base == (rootCommand+" remove")){
        // remove file from selection
        std::string index;
        bool gotValue = paramParser.getParameter("index", index, "i");
        if (!gotValue)
            return false;
        int index_from_string = std::stoi(index);
        selection.removeImage(index_from_string);
        return true;
    } else if (base == (rootCommand+" status")){
        // print status
        selection.printSelection();
        return true;
    } else if (base == (rootCommand+" start")){
        // not implemented
        std::cout << "starting upload to arduino\n";
        return true;
    }

    return false;
}

int main(int, char**) {
    // check os
    if (os > 4)
        return 1;
    setCommandIdentifiers();

    // init enviroment screen
    std::cout << "\033[1m >>>>> POV Enviroment Start\n";
    std::cout << "enter '"<< quitCommand <<"' to quit. enter 'pov help' to list available commands\033[0m\n";
    std::cout << "\033[33m INFO: please input path and filenames enclosed by quotation marks (\")\033[0m\n";
    // last user input
    std::string lastInput = "";
    // main loop
    do{
        std::cout << "\033[1;32m<" << currentPath << ">: \033[0m";
        // get user input
        std::getline(std::cin, lastInput);
        lastInput = trimString(lastInput);
        if (lastInput == quitCommand)
            break;
        
        auto cmdfound = evaluateCommand(lastInput);
        if (!cmdfound)
            std::cout << "Command "<< lastInput <<" not found\n";

    }while(lastInput != quitCommand);
    std::cout << "\033[1mQuitting...\n";
    std::cout << ">>>>> POV Enviroment Exit\033[0m\n";
}
