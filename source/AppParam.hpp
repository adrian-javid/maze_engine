#ifndef ProgramParam_hpp
#define ProgramParam_hpp

#include <string>
#include <unordered_map>
#include <optional>
#include "Util.hpp"

namespace Project {
    struct AppParam {
        std::string const description;
        std::string argument;
        using Acceptable = std::unordered_map<std::string, std::string>;
        std::optional<Acceptable> const acceptable;
        static std::unordered_map<std::string, AppParam> const &parseArgv(int const argc, char const *const *const argv);

        template<typename T>
        static T castArg(std::string const &arg) {
            static_assert(
                std::is_same_v<T, int> or
                std::is_same_v<T, unsigned int> or
                std::is_same_v<T, bool>,
                "Unsupported type."
            );
            
            constexpr auto getTypeName = []() constexpr -> std::string {
                if constexpr (std::is_same_v<T, int>)
                    return "int";
                else if constexpr (std::is_same_v<T, unsigned int>)
                    return "unsigned int";
                else if constexpr (std::is_same_v<T, bool>)
                    return "bool";
                else
                    return "";
            };

            T value{};
            if ((std::istringstream(arg) >> value).fail()) Util::errOutLn(
                "Unable to convert `" + arg + "` to `" + getTypeName + "` value."
            );

            return value;
        }

        private:
            static std::unordered_map<std::string, AppParam> config;
    };
}

#endif
