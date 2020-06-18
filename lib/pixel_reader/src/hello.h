#pragma once
#include "lodepng.h"
#include <iostream>

typedef std::vector<unsigned char> imgVector;
#define forloop(len) for(int i = 0; i<len; i++)

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
    imgVector decodeWithState(const char* filename) {
    imgVector png;
    imgVector image; //the raw pixels
    unsigned width, height;
    lodepng::State state; //optionally customize this one

    unsigned error = lodepng::load_file(png, filename); //load the image file with given filename
    if(!error) error = lodepng::decode(image, width, height, state, png);
    std::cout << "width= " << width << " height= " << height << std::endl;

    //if there's an error, display it
    if(error){
        std::cout << "decoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
        return imgVector();
    }

    return image;

    //the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
    //State state contains extra information about the PNG such as text chunks, ...
    }
    
}
