#include "filesys.h"

bool filesys::isPngFile(const std::string filepath){
    std::ostringstream os;
    auto size = filepath.length()-1;
    os << filepath[size-3] << filepath[size-2] << filepath[size-1] << filepath[size-0];
    if (os.str() == ".png")
        return true;
    return false;
}

// https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
void filesys::listItemsInDirectoryAlt(const std::string path){
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (path.c_str())) != NULL) {
        /* print all the files and directories within directory */
        std::cout << "items in directory " << path << std::endl;
        auto counter = 0;
        while ((ent = readdir (dir)) != NULL) {
            auto filename = ent->d_name;
            switch (ent->d_type) {
                case DT_DIR:
                    // Directory
                    std::cout << "\033[1;34m" << counter << ":\t" << filename << "\t" << "\033[0m" << std::endl;
                    break;
                default:
                    // all other than directory case DT_REG for regular file
                    std::ostringstream os;
                    os << path << filename;
                    auto is_png = isPngFile(os.str());
                    if(is_png){
                        std::cout << "\033[1;31m" << counter << ":\t" << filename << "\t" << "\033[0m" << std::endl;
                    }else{
                        std::cout << "\033[1;30m" << counter << ":\t" << filename << "\t" << "\033[0m" << std::endl;
                    }
                    break;
            }
            ++counter;
        }
        closedir (dir);
    } else {
        /* could not open directory */
        // perror ("");
        // return EXIT_FAILURE;
        std::cout << "Error listing items!\n";
    }
}

// https://stackoverflow.com/questions/18100097/portable-way-to-check-if-directory-exists-windows-linux-c
bool filesys::directoryExists(const std::string dir){
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
bool filesys::fileExists(const std::string name) {
    struct stat buffer;   
    return (stat (name.c_str(), &buffer) == 0); 
}