#pragma once

#include <string>

#ifdef ENABLE_DEBUG
    #include <iostream>
    #define DEBUG_LOG(msg) \
        do { std::cerr << "[DEBUG] " << msg << std::endl; } while(0)
#else
    #define DEBUG_LOG(msg) do {} while(0)
#endif

namespace Courage {
    std::string getVersion();
    void compressString(const std::string& input, std::string& output);
    void decompressString(const std::string& input, std::string& output);
}