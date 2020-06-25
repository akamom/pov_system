#include <iostream>
#include "hello.h"

int main(int, char**) {
    /// ALTE METHODE
    //auto pxl = pixel::decodeTwoSteps("/Users/i519401/Downloads/test.png");
    //forloop(pxl.size()){
    //    std::cout << (int)pxl[i] << ((i+1)%4==0 ? "\n":",");
    //}

    /// MEINE NEUE METHODE
    auto pixels = std::vector<color>();
    bool err = pixel::decodeWithState("/Users/i519401/Downloads/test.png", pixels);
    for(auto col : pixels)
        col.print();
}
