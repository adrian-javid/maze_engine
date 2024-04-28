#include "config.hpp"

std::unordered_map<std::string, Project::Global::Param> Project::Global::config {
    {"search", {
        "Search algorithm.",
        "depth",
        {
            {"depth", "Depth-first search."},
            {"breadth", "Breadth-first search."},
            {
                "dijkstra",
                std::string(
                    "Dijkstra's algorithm. "
                    "NOTE: Since Dijkstra's algorithm for unweighted "
                    "graphs is equivalent to breadth-first search, "
                    "`dijkstra` serves as an alias for `breadth`, as this "
                    "program uses mazes with unweighted vertices (maze tiles). "
                ),
            },
            {"greedy", "Greedy best-first search."},
            {"a_star", "A* search algorithm."},
        }
    }},
    {"seed", {
        "Integer value. Seed for random number generator used to generate mazes etc."
        "0",
    }},
    {"grid", {
        "Grid type to use."
        "square",
        {
            {"square", }
        }
    }},
    {"size", {
        "Scale of grid.",
        "5",
    }},
    {"wrap", {
        std::string(
            "Whether tile wrap is allowed on the grid along its perimeter."
            "If this is `true`, algorithms will be able to find solutions that go \"wrap\" "
            "from one edge of the maze to the other."
        ),
        "false",
    }},
};
