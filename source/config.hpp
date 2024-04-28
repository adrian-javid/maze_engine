#ifndef config_hpp
#define config_hpp

#include <string>
#include <unordered_map>

namespace Project::Global {
    struct Param {
        std::string const description;
        std::string argument;
        std::unordered_map<std::string, std::string> const describeValue;
        Param() = delete;
    };

    extern std::unordered_map<std::string, Param> config;
}

#endif
