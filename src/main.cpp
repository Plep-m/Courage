#include <iostream>
#include "Courage/utils.hpp"
#include "Courage/version.h"

int main() {
    std::cout << "Courage Version: " << Courage::getVersion() << std::endl;
    
    std::string input = "This is a test string to compress using zlib in our C++ project.";
    std::string compressed;
    std::string decompressed;
    
    try {
        Courage::compressString(input, compressed);
        std::cout << "Original size: " << input.size() << std::endl;
        std::cout << "Compressed size: " << compressed.size() << std::endl;
        
        Courage::decompressString(compressed, decompressed);
        std::cout << "Decompressed string: " << decompressed << std::endl;
        
        if (input == decompressed) {
            std::cout << "Compression and decompression successful!" << std::endl;
        } else {
            std::cout << "Error: decompressed string doesn't match original!" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}