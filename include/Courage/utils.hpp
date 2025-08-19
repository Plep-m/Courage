#pragma once

#include <string>

namespace Courage {
    std::string getVersion();
    void compressString(const std::string& input, std::string& output);
    void decompressString(const std::string& input, std::string& output);
}