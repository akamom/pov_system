#include <iostream>
#include "hello.h"

int main(int, char**) {
    //pixel::say_hello();
    //auto pxl = decodeWithState("/Users/i519401/Downloads/test2.png");
    auto pxl = pixel::decodeTwoSteps("/resources/test2.png");
    //for (auto c : pxl){
    //    std::cout << c << std::endl;
    //}
    forloop(pxl.size()){
        std::cout << (int)pxl[i] << "," << ((i+1)%4==0 ? "\n":"");
    }
}
