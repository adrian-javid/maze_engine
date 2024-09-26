#!/usr/bin/env python3

from typing import Literal
import subprocess
from pathlib import Path
import platform
import os

def _main(argList: list[str]) -> None|str:
	buildType: Literal["--release", "--debug"]|None = None

	platformName: str = platform.system()
	
	if (platformName == ""): return F"Failed to get the platform name."

	# Get an iterator to the arguments, and skip the first item.
	argIter = iter(argList); next(argIter)

	for arg in argIter:
		match arg:
			case "--release" | "--debug":
				if buildType is not None:
					return F"Can't set build type to \"{arg}\" because it was already set to \"{buildType}\"."
				buildType = arg
			case _:
				return F"Received unrecognized flag \"{arg}\"."

	if buildType is None: buildType = "--release"

	assert platformName != ""

	baseExecutableName = F"maze_engine{'.exe' if platformName == "Windows" else ''}"

	executablePath: Path = Path(".")/"build"/platformName/buildType[2:]/"run"/baseExecutableName

	if (exitCode := subprocess.run(["scons", executablePath]).returncode) != os.EX_OK:
		return F"Failed to build executable `{executablePath}`; exit code was `{exitCode}`."

	if (exitCode := subprocess.run([executablePath]).returncode) != os.EX_OK:
		return F"Executable `{executablePath}` returned with error code `{exitCode}`."

	return None

import sys

if __name__ == "__main__":
	sys.exit(_main(sys.argv))
