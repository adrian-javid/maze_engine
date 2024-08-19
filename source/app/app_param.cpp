#include "app_param.hpp"
#include <sstream>

std::unordered_map<std::string, App::AppParamInfo> App::AppParamInfo::config{
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
		std::nullopt
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
		"10",
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
	{"delay", {
		"Unsigned integer value. Delay to add in milliseconds after accessing a tile in the maze. ",
		"15",
		std::nullopt,
	}},
};

std::string App::AppParamInfo::validParametersStr(bool const verbose) {
		std::ostringstream stream;
		for (auto const &[paramName, info] : config) {
			stream << "\t`" << paramName << "`: " << info.description;
			if (verbose) stream << "\n\tdefault value: `" << info.argument << '`';
			stream << '\n';
			if (verbose and info.acceptable) stream << "\tAcceptable values:\n" << acceptableValuesStr(info.acceptable.value());
			else stream << '\n';
		}
		return stream.str();
}

std::string App::AppParamInfo::acceptableValuesStr(AppParamInfo::Acceptable const &acceptableValues) {
	std::ostringstream stream;
	for (auto const &[value, description] : acceptableValues) {
		stream << "\t\t`" << value << "`: " << description << '\n';
	}

	stream << '\n';

	return stream.str();
}

auto App::AppParamInfo::parseArgv(int const argc, char const *const *const argv) -> std::unordered_map<std::string, AppParamInfo> const & {
	// Check for "help".
	for (int argIndex{1}; argIndex < argc; ++argIndex) {
		if (std::string(argv[argIndex]) == "help") {
			std::cout <<
			"This program generates mazes, then solves them using a chosen algorithm.\n"
			"It opens a window with a view of the algorithm solving the maze in real time.\n"
			"Parameters are set using their name and a value after `=`. For example, `search=dijkstra`.\n\n" <<
			"Parameters:\n" <<
			validParametersStr(true) <<
			"Run without `help` to generate and solve mazes.\n";
			std::exit(EXIT_SUCCESS);
		}
	}

	for (int argIndex{1}; argIndex < argc; ++argIndex) {
		std::string const arg(argv[argIndex]);

		std::size_t const delimPos(arg.find("="));

		if (delimPos == std::string::npos) Util::errOut(
			"Parameter with no value: \"" + arg + "\". Please specify value after `=`.\n" +
			"Example: `" + arg + "=value" + "`.\n\n" +
			helpTipString
		);

		std::string const paramName = arg.substr(0u, delimPos);

		auto const paramPtr = config.find(paramName);
		if (paramPtr == config.end()) Util::errOut(
			"Invalid parameter: `" + paramName + "` from `" + arg + "`.\n"
			"Valid parameters:\n" + validParametersStr() + "\n" + helpTipString
		);

		AppParamInfo &param = paramPtr->second;
		param.argument = arg.substr(delimPos + 1);

		if (not param.acceptable) continue;

		if (param.acceptable.value().find(param.argument) == param.acceptable.value().end()) Util::errOut(
			"Unacceptable value `" + param.argument + "` for parameter `" + paramName + "`.\n" +
			"Acceptable values:\n" + acceptableValuesStr(param.acceptable.value()) + "\n" + helpTipString
		);
	}

	return config;
}
