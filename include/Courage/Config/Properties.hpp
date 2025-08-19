#pragma once

#include <map>
#include <string>

class Properties {
public:
    explicit Properties(const std::string &filename);

    std::string get(const std::string &key, const std::string &def = "") const;
    int getInt(const std::string &key, int def = 0) const;

private:
    std::map<std::string, std::string> values;

    void generateDefault(const std::string &filename);
    void load(const std::string &filename);
};
