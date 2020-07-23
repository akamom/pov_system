#include "imageselection.h"

void ImageSelection_c::printSelection(){
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

void ImageSelection_c::addImage(const std::string abs_file_path){
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

void ImageSelection_c::removeImage(const int atIndex){
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

std::string ImageSelection_c::pixelsToBoolString(const std::vector<color> pix) const{
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

std::string ImageSelection_c::pixelsToPixelsString(const std::vector<color> pix) const{
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