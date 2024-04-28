#ifndef ProgramParam_hpp
#define ProgramParam_hpp

#include <string>
#include <unordered_map>
#include <optional>

namespace Project {
    struct AppParam {
        std::string const description;
        std::string argument;
        using Acceptable = std::unordered_map<std::string, std::string>;
        std::optional<Acceptable> const acceptable;
        static std::unordered_map<std::string, AppParam> const &parseArgv(int const argc, char const *const *const argv);

        private:
            static std::unordered_map<std::string, AppParam> config;
    };
}

#endif
