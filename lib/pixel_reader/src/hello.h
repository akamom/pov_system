#pragma once
#include "lodepng.h"
#include <iostream>

typedef std::vector<unsigned char> imgVector;
#define forloop(len) for(int i = 0; i<len; i++)

struct color{
    color(int r, int g, int b, int a) : R{r}, G{g}, B{b}, A{a} {}
    int R, G, B, A;
    void print(){
        std::cout << ("R: ") << R << "\tG: " << G << "\tB: " << B << "\tA: " << A << std::endl;
    }
};

namespace pixel {
    imgVector decodeTwoSteps(const char* filename) {
    std::vector<unsigned char> png;
    std::vector<unsigned char> image; //the raw pixels
    unsigned width, height;

    //load and decode
    unsigned error = lodepng::load_file(png, filename);
    if(!error) error = lodepng::decode(image, width, height, png);

    //if there's an error, display it
    if(error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

    return image;

    //the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
    }

    //Example 3
    //Load PNG file from disk using a State, normally needed for more advanced usage.
    bool decodeWithState(const char* filename, std::vector<color> &pixels) {
        std::vector<unsigned char> png;
        std::vector<unsigned char> image; //the raw pixels
        unsigned width, height;
        lodepng::State state; //optionally customize this one

        unsigned error = lodepng::load_file(png, filename); //load the image file with given filename
        if(!error) error = lodepng::decode(image, width, height, state, png);
        std::cout << "width= " << width << " height= " << height << std::endl;

        //if there's an error, display it
        if(error){
            std::cout << "decoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
            return false;
        }

        if(image.size()%4 != 0){
            std::cout << "incorrect pixel count\n";
            return false;
        }

        for(int i = 0; i<image.size(); i+=4){
            color col{(int)image[i], (int)image[i+1], (int)image[i+2], (int)image[i+3]};
            pixels.push_back(col);
        }

        return true;

        //the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
        //State state contains extra information about the PNG such as text chunks, ...
    }
    
}
