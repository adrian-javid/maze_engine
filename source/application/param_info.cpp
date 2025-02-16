#include "param_info.hpp"

#include <sstream>
#include <unordered_map>
#include <unordered_set>

std::unordered_map<std::string, App::ParamInfo> App::ParamInfo::config{
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
		"Unsigned integer value. Sets the size of the maze."
			"\n\t\tThe size of the maze is determined by counting from the center outwards"
			"\n\t\tan equal amount of tiles to the `size` parameter."
			"\n\t\tThe center tile is included in the count.",
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
	{"wall_prune", {
		"Unsigned integer value. Excess walls will be removed up to a maximum count of this value.",
		"0",
		std::nullopt
	}},
	{"sound", {
		"Sound instrument to use sound effects from.",
		"synthesizer",
		Acceptable{
			{"none", "Do not use sound effects."},
			{"piano", "Use grand piano for sound effects."},
			{"synthesizer", "Use square synthesizer for sound effects."},
		}
	}},
	{"delay", {
		"Unsigned integer value. Delay to add in milliseconds after accessing a tile in the maze. ",
		"60",
		std::nullopt,
	}},
	{"show_maze_generation", {
		"If `true`, the program will first spend time showing the incremental process of generating the maze.",
		"true",
		Acceptable{
			{"true", "Enable maze generation showcase."},
			{"false", "Skip maze generation showcase."}
		},
	}},
};

std::string App::ParamInfo::validParametersStr(bool const verbose) {
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

std::string App::ParamInfo::acceptableValuesStr(ParamInfo::Acceptable const &acceptableValues) {
	std::ostringstream stream;
	for (auto const &[value, description] : acceptableValues) {
		stream << "\t\t`" << value << "`: " << description << '\n';
	}

	stream << '\n';

	return stream.str();
}

auto App::ParamInfo::parseArgv(int const argc, char const *const *const argv) -> std::unordered_map<std::string, ParamInfo> const & {
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

	std::unordered_set<std::string> duplicateDetection;

	for (int argIndex{1}; argIndex < argc; ++argIndex) {
		std::string const arg(argv[argIndex]);

		std::size_t const delimPos(arg.find("="));

		if (delimPos == std::string::npos) errorExit(
			"Parameter with no value: \"" + arg + "\". Please specify value after `=`.\n" +
			"Example: `" + arg + "=value" + "`.\n\n" +
			helpTipString
		);

		std::string const paramName(arg.substr(0u, delimPos));

		if (
			auto const notFound(duplicateDetection.cend());
			duplicateDetection.find(paramName) != notFound
		) errorExit(
			"Repeated parameter \"" + paramName + "\" detected. Repeated parameters are not allowed."
		);

		duplicateDetection.insert(paramName);

		auto const paramPtr(config.find(paramName));
		if (paramPtr == config.end()) errorExit(
			"Invalid parameter: `" + paramName + "` from `" + arg + "`.\n"
			"Valid parameters:\n" + validParametersStr() + "\n" + helpTipString
		);

		ParamInfo &param{paramPtr->second};
		param.argument = arg.substr(delimPos + 1);

		if (not param.acceptable.has_value()) continue;

		if (param.acceptable.value().find(param.argument) == param.acceptable.value().end()) errorExit(
			"Unacceptable value `" + param.argument + "` for parameter `" + paramName + "`.\n" +
			"Acceptable values:\n" + acceptableValuesStr(param.acceptable.value()) + "\n" + helpTipString
		);
	}

	return config;
}
