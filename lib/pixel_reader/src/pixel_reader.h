#pragma once
#include "lodepng.h"
#include <iostream>

typedef std::vector<unsigned char> imgVector;
#define forloop(len) for(int i = 0; i<len; i++)

// basic color struct
struct color{
    color(int r, int g, int b, int a) : R{r}, G{g}, B{b}, A{a} {}
    int R, G, B, A;
    void print(){
        std::cout << ("\033[1;31mR: \033[0m") << R << "\t\033[1;32mG: \033[0m" << G << "\t\033[1;96mB: \033[0m" << B << "\tA: \033[0m" << A << std::endl;
    }
};

namespace pixel {
    /**
     * @brief sample function to load png
     * 
     * @param filename 
     * @return imgVector 
     */
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

    // width and height restriction
    const int heightRestriction = 10;
    const int widthRestriction = 10;
    
    /**
     * @brief Load PNG file from disk using a State, normally needed for more advanced usage. Sample function
     * 
     * @param filename 
     * @param pixels 
     * @return true 
     * @return false 
     */
    bool decodeWithState(const char* filename, std::vector<color> &pixels) {
        std::vector<unsigned char> png;
        std::vector<unsigned char> image; //the raw pixels
        unsigned width, height;
        lodepng::State state; //optionally customize this one

        unsigned error = lodepng::load_file(png, filename); //load the image file with given filename
        if(!error) error = lodepng::decode(image, width, height, state, png);
        //std::cout << "width= " << width << " height= " << height << std::endl;
        if (width != widthRestriction || height != heightRestriction){
            std::cout << "\033[1;202mImage dimensions are width=" << width << " , height=" << height << " . \nHowever, dimensions must be width=" << widthRestriction << " and height=" << heightRestriction << "\033[0m\n";
            return false;
        }

        //if there's an error, display it
        if(error){
            std::cout << "\033[1;202mDECODER ERROR: " << error << ": "<< lodepng_error_text(error) << "\033[0m\n";
            return false;
        }

        if(image.size()%4 != 0){
            std::cout << "\033[1;202mCONVERSION ERROR: inconsistent pixel count!\033[0m\n";
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
