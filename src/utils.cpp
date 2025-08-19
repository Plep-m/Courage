#include "Courage/utils.hpp"
#include "Courage/version.h"
#include <zlib.h>
#include <stdexcept>
#include <cstring>

namespace Courage {
    std::string getVersion() {
        return COURAGE_VERSION;
    }

    void compressString(const std::string& input, std::string& output) {
        z_stream zs;
        memset(&zs, 0, sizeof(zs));
        
        if (deflateInit(&zs, Z_BEST_COMPRESSION) != Z_OK) {
            throw std::runtime_error("deflateInit failed while compressing.");
        }
        
        zs.next_in = (Bytef*)input.data();
        zs.avail_in = input.size();
        
        int ret;
        char outbuffer[32768];
        std::string outstring;
        
        do {
            zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
            zs.avail_out = sizeof(outbuffer);
            
            ret = deflate(&zs, Z_FINISH);
            
            if (outstring.size() < zs.total_out) {
                outstring.append(outbuffer, zs.total_out - outstring.size());
            }
        } while (ret == Z_OK);
        
        deflateEnd(&zs);
        
        if (ret != Z_STREAM_END) {
            throw std::runtime_error("Exception during zlib compression: " + std::to_string(ret));
        }
        
        output = outstring;
    }

    void decompressString(const std::string& input, std::string& output) {
        z_stream zs;
        memset(&zs, 0, sizeof(zs));
        
        if (inflateInit(&zs) != Z_OK) {
            throw std::runtime_error("inflateInit failed while decompressing.");
        }
        
        zs.next_in = (Bytef*)input.data();
        zs.avail_in = input.size();
        
        int ret;
        char outbuffer[32768];
        std::string outstring;
        
        do {
            zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
            zs.avail_out = sizeof(outbuffer);
            
            ret = inflate(&zs, 0);
            
            if (outstring.size() < zs.total_out) {
                outstring.append(outbuffer, zs.total_out - outstring.size());
            }
        } while (ret == Z_OK);
        
        inflateEnd(&zs);
        
        if (ret != Z_STREAM_END) {
            throw std::runtime_error("Exception during zlib decompression: " + std::to_string(ret));
        }
        
        output = outstring;
    }
}