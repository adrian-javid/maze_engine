# Maze Engine
A program that visualizes the process of generating and solving mazes.

## Running

The executable is in the `build` directory.

Run with `help` to see the program's parameters.

```PowerShell
# Windows release build
.\build\Windows\release\run\maze_engine.exe help

# Windows debug build
.\build\Windows\debug\run\maze_engine.exe help
```

```Shell
# Linux release build
./build/Linux/release/run/maze_engine help

# Linux debug build
/build/Linux/debug/run/maze_engine help
```

<!--
	Interesting program arguments for future reference:
	`size=550 seed=7722214 grid=square search=greedy delay=0`
-->

## Building

To build, you need to [install the neccessary dependencies for your platform](#dependencies).

Running `scons` on the command-line will build the default target, which is the release build for your native platform, `build/<platform>/release/run/maze_engine<file_extension>`.

To build all available targets&mdash;including the test suite and the JSON compilation database&mdash;run `scons .` on the command-line.

The test suite executable is put at `build/<platform>/<build_type>/run/test_suite<file_extension>`.

The JSON compilation database is put at `build/<platform>/<build_type>/compile_commands.json`. You can configure Visual Studio Code to use this database with the Microsoft C++ extension, for example:
> `.vscode/c_cpp_properties.json`
```JSON
{
	"configurations": [
		{
			"name": "Win32",
			"compileCommands": "build\\Windows\\debug\\compile_commands.json",
			"cStandard": "c17", "cppStandard": "c++17"
		},
		{
			"name": "Linux",
			"compileCommands": "build/Linux/debug/compile_commands.json",
			"cStandard": "c17", "cppStandard": "c++17"
		},
		{
			"name": "emscripten",
			"compileCommands": "build/web/release/compile_commands.json",
			"cStandard": "c17", "cppStandard": "c++17"
		}
	],
	"version": 4
}
```

Also, if you would like to build a specific file,
you can run `scons <file>` where `<file>` is a file you want to build.
For example, to build only the compilation database for the debug build on Windows,
you would run this:
```PowerShell
scons .\build\Windows\debug\compile_commands.json
```

To clean the build, run `scons -c` or `scons -c .`, for the default target or all targets, respectively.

> Note: If you want to move the Windows executable `maze_engine.exe` to a different location,
move `SDL2.dll` and `SDL2_mixer.dll` along with it to the same directory.
The executable needs those dynamic-link libraries to run.

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
> Windows usually uses `py` for Python.
```PowerShell
py -m pip install --requirement requirements.txt
```

#### SCons for Linux & Unix
> Linux usually uses `python3` for Python.
```bash
python3 -m pip install --requirement requirements.txt
```

### SDL (Simple DirectMedia Layer)
This project requires [SDL](https://www.libsdl.org/). The method of downloading SDL depends on the platform.

#### SDL for 64-bit Windows

Run the [`fetch_libraries.py`](./fetch_libraries.py) Python script from the root of the project to fetch the SDL2 library for Windows. This script fetches and unpacks the main SDL2 library from the [SDL releases on GitHub](https://github.com/libsdl-org/SDL/releases) and the SDL2 Mixer library from the [SDL Mixer releases on GitHub](https://github.com/libsdl-org/SDL_mixer/releases).

```PowerShell
py fetch_libraries.py
```

#### SDL for Linux & Unix

These commands are copied from the [official SDL installation guide](https://wiki.libsdl.org/SDL2/Installation#linuxunix).

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
