#ifndef ProgramParam_hpp
#define ProgramParam_hpp

#include <string>
#include <unordered_map>

namespace Project {
    struct AppParam {
        std::string const description;
        std::string argument;
        std::unordered_map<std::string, std::string> const describeValue;
        static std::unordered_map<std::string, AppParam> parseArgv(int const argc, char const *const *argv);
    };
}

#endif
