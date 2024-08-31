"""
	Fetch and unpack the Simple Directmedia Layer library version 2 for Windows.
"""

from urllib.request import urlretrieve as download
from pathlib import Path
from zipfile import ZipFile
from shutil import copytree as copyRecursively

# Create a directory to store downloaded files.
downloadDir: str = "downloaded"
Path(F"{downloadDir}/").mkdir(parents=True, exist_ok=True)

SDL2_VERSION: str = "2.30.6"
SDL2_MIXER_VERSION: str = "2.8.0"

for url, includeDir, libDir, platform, namespace in (

	# This is information for downloading SDL2 for 64-bit Windows.
	(
		F"https://github.com/libsdl-org/SDL/releases/download/release-{SDL2_VERSION}/SDL2-devel-{SDL2_VERSION}-VC.zip",
		F"SDL2-{SDL2_VERSION}/include/",
		F"SDL2-{SDL2_VERSION}/lib/x64/",
		'Windows',
		"SDL2",
	),

	# This is information for downloading SDL2 Mixer for 64-bit Windows.
	(
		F"https://github.com/libsdl-org/SDL_mixer/releases/download/release-{SDL2_MIXER_VERSION}/SDL2_mixer-devel-{SDL2_MIXER_VERSION}-VC.zip",
		F"SDL2_mixer-{SDL2_MIXER_VERSION}/include/",
		F"SDL2_mixer-{SDL2_MIXER_VERSION}/lib/x64/",
		'Windows',
		"SDL2",
	),

):
	basename = url[url.rindex("/")+1:]

	zipFilePath = Path(F"{downloadDir}/{basename}")

	# Download the ZIP file containing SDL.
	if not zipFilePath.exists(): download(url=url, filename=zipFilePath)

	with ZipFile(zipFilePath) as zipFile:

		libraryCore: tuple[str, ...] = tuple(
			member
			for member in zipFile.namelist()
			if member.startswith(includeDir) or member.startswith(libDir)
		)

		# Extract all header files and library binaries.
		zipFile.extractall(members=libraryCore, path=F"{downloadDir}/")

		# Copy header files to relevant directory.
		destinationIncludePath = Path(F"library/{platform}/include/{namespace}")
		copyRecursively(dst=str(destinationIncludePath), src=F"{downloadDir}/{includeDir}/", dirs_exist_ok=True)

		# Copy library binaries to relevant directory.
		destinationLibPath = Path(F"library/{platform}/lib/{namespace}")
		copyRecursively(dst=destinationLibPath, src=F"{downloadDir}/{libDir}/", dirs_exist_ok=True)
