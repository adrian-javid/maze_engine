#include "AppParam.hpp"
#include "Util.hpp"
#include <sstream>

std::unordered_map<std::string, Project::AppParam> Project::AppParam::config{
    {"search", {
        "Search algorithm.",
        "depth",
        Acceptable{
            {"depth", "Depth-first search."},
            {"breadth", "Breadth-first search."},
            {
                "dijkstra",
                std::string(
                    "Dijkstra's algorithm. Alias for `breadth` because they're equivalent for unweighted graphs."
                ),
            },
            {"greedy", "Greedy best-first search."},
            {"a_star", "A* search algorithm."},
        }
    }},
    {"seed", {
        "Integer value. Seeds the random number generation. This affects how the maze is generated.",
        "0",
    }},
    {"grid", {
        "Grid type to use.",
        "square",
        Acceptable{
            {"square", "Use square maze."},
            {"hexagon", "Use hexagon maze."},
        }
    }},
    {"size", {
        "Unsigned integer value. Controls size of maze. For `square`, this is width and height. For `hexagon`, this is radius.",
        "5",
        std::nullopt,
    }},
    {"wrap", {
        std::string(
            "If `true`, algorithms will be able to find solutions that \"wrap\" "
            "from one edge of the maze to the other."
        ),
        "false",
        Acceptable{
            {"true", "Enable wrapping. (Tiles on edges of maze may be neighbors.)"},
            {"false", "Disable wrapping."}
        },
    }},
};

auto Project::AppParam::parseArgv(int const argc, char const *const *const argv) -> std::unordered_map<std::string, AppParam> const & {
    for (int argIndex{1}; argIndex < argc; ++argIndex) {
        std::string const arg(argv[argIndex]);

        std::size_t const delimPos(arg.find("="));

        if (delimPos == std::string::npos) Util::errOutLn(
            "Parameter with no value: \"" + arg + "\". Please specify value after `=`.\n"
            "Example: `" + arg + "=value" + "`."
        );

        std::string const paramName = arg.substr(0u, delimPos);

        auto const paramPtr = config.find(paramName);
        if (paramPtr == config.end()) Util::errOutLn(
            "Invalid parameter: `" + paramName + "` from `" + arg + "`.\n"
            "Valid parameters:\n" + []() -> std::string {
                std::ostringstream stream;
                for (auto const &[paramName, info] : config) {
                    stream << '\t' << paramName << ": " << info.description << '\n';
                }
                return stream.str();
            }()
        );

        AppParam &param = paramPtr->second;
        param.argument = arg.substr(delimPos + 1);

        if (not param.acceptable) continue;

        if (param.acceptable.value().find(param.argument) == param.acceptable.value().end()) Util::errOutLn(
            "Unnacceptable value `" + param.argument + "` for parameter `" + paramName + "`.\n"
            "Acceptable values:\n" + [&param]() -> std::string {
                std::ostringstream stream;
                for (auto const &[value, description] : param.acceptable.value())
                    stream << '\t' << value << ": " << description << '\n';
                return stream.str();
            }()
        );
    }

    return config;
}
