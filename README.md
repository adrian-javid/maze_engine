# Maze Solver
It is a program that solves mazes.

## Dependencies

This project uses [SCons](https://scons.org/) as its build system, so [Python](https://www.python.org/) is required to build.

SCons and other Python dependencies are listed in [`requirements.txt`](./requirements.txt) at the root of the project. You can use [`pip`](https://pip.pypa.io/en/stable/) to install them.

Unix:
```bash
python3 -m pip install --requirement requirements.txt
```

Windows:
```PowerShell
py -m pip install --requirement requirements.txt
```

This project requires [Simple DirectMedia Layer](https://www.libsdl.org/) (SDL). The method of downloading SDL depends on the platform.

### SDL on 64-bit Windows
Run the [`fetch_libraries.py`](./fetch_libraries.py) Python script from the root of the project to fetch the SDL2 library for Windows. This command runs a script that fetches and unpacks [`SDL2-devel-2.28.5-VC.zip` from GitHub](https://github.com/libsdl-org/SDL/releases/release-2.28.5/).

```PowerShell
py fetch_libraries.py
```

## SDL on Linux/Unix

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
