# Maze Solver
It is a program that solves mazes.

It generates a maze based on a seed, then displays a window displaying a specified search algorithm solving the maze in real time.

## Running

The executable is in the `build` directory.

Run with `help` to see the program's parameters.

```PowerShell
# Windows
.\build\Windows\solve_maze.exe help
```

```bash
# Linux
./build/Linux/solve_maze help
```

<!-- `size=550 seed=7722214 grid=square search=greedy delay=0` -->

## Building

To build, you need to [install the neccessary dependencies for your platform](#dependencies).

Run `scons` on the command-line in the root of the project to build the executable in the generated `build` directory, regardless of the platform.

To also generate compile commands to be used for intellisense in an editor such as VSCode, run `scons .`.
The `compile_commands.json` are put into the `build/<platform>` directory.

To clean the build, run `scons -c` or `scons -c .` respectively.

> Note: If you want to move the Windows executable `solve_maze.exe` to a different location,
> move the `SDL2.dll` along with it to the same directory.
> The executable needs that dynamic-link library to run.

## Dependencies

### C++ Compiler

The following are the C++ compilers that can build successfully for each platform. Other untested C++ compilers may work.

#### C++ Compiler for Windows

The Windows build uses the command-line toolset from the
[Build Tools for Visual Studio](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022).

#### C++ Compiler for Linux & Unix

This project builds with `g++`.

### Build System: SCons (Software Construction)
This project uses [SCons](https://scons.org/) as its build system, so [Python](https://www.python.org/) is required to build it.

SCons and other Python dependencies are listed in [`requirements.txt`](./requirements.txt) at the root of the project. You can use [`pip`](https://pip.pypa.io/en/stable/) to install them.

#### SCons for Windows
```PowerShell
py -m pip install --requirement requirements.txt
```

#### SCons for Linux & Unix
```bash
python3 -m pip install --requirement requirements.txt
```

### SDL (Simple DirectMedia Layer)
This project requires [SDL](https://www.libsdl.org/). The method of downloading SDL depends on the platform.

#### SDL for 64-bit Windows

Run the [`fetch_libraries.py`](./fetch_libraries.py) Python script from the root of the project to fetch the SDL2 library for Windows. This script fetches and unpacks [`SDL2-devel-2.28.5-VC.zip`](https://github.com/libsdl-org/SDL/releases/release-2.28.5/) from GitHub.

```PowerShell
py fetch_libraries.py
```

#### SDL for Linux & Unix

These commands are copied from the [official SDL installation guide](https://wiki.libsdl.org/SDL2/Installation).

Debian-based systems (including Ubuntu):
```bash
sudo apt-get install libsdl2-dev
```

Red Hat-based systems (including Fedora):
```bash
sudo dnf install SDL2-devel
```

Gentoo:
```bash
sudo emerge libsdl2
```
