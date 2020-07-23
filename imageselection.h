#pragma once
#include <Map>
#include <iostream>
#include <string.h>
#include <vector>
#include <sstream>
#include "lib/pixel_reader/src/pixel_reader.h"

class ImageSelection_c{
public:
    ImageSelection_c(){
        // m_pixelSelection = std::map<std::string, std::vector<color>>();
        m_pixelSelection = std::map<std::string, std::string>();
        m_pathSelection = std::map<std::string, std::string>();
    }
    ~ImageSelection_c(){}

    void addImage(const std::string abs_file_path);
    void addImage(const std::string fileName, const int atIndex){}
    void removeImage(const std::string fileName){}
    void removeImage(const int atIndex);

    void printSelection();
private:
    std::map<std::string, std::string>          m_pathSelection;
    // selection of images. key: path+imagename, value: conversed pixels
    std::map<std::string, std::string>  m_pixelSelection;
    // std::map<std::string, std::vector<color>>   m_pixelSelection;

    const size_t MAX_SELECTED = 1;

    std::string pixelsToBoolString(const std::vector<color> pix) const;
    std::string pixelsToPixelsString(const std::vector<color> pix) const;
};