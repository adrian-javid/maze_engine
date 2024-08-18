from urllib.request import urlretrieve as download
from pathlib import Path
from zipfile import ZipFile
from shutil import copytree as copyRecursively


# Create a directory to store downloaded files.
downloadDir: str = "downloaded"
Path(F"{downloadDir}/").mkdir(parents=True, exist_ok=True)

SDL2_VERSION: str = "2.30.6"

for url, includeDir, libDir, platform, simpleName in (

    # This is information for downloading SDL for 64-bit Windows.
    (
        F"https://github.com/libsdl-org/SDL/releases/download/release-{SDL2_VERSION}/SDL2-devel-{SDL2_VERSION}-VC.zip",
        F"SDL2-{SDL2_VERSION}/include/",
        F"SDL2-{SDL2_VERSION}/lib/x64/",
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
        if not (libraryPath := Path(F"library/{platform}/include/{simpleName}")).exists():
            copyRecursively(dst=str(libraryPath), src=F"{downloadDir}/{includeDir}/")

        # Copy library binaries to relevant directory.
        if not (libraryPath := Path(F"library/{platform}/lib/{simpleName}")).exists():
            copyRecursively(dst=libraryPath, src=F"{downloadDir}/{libDir}/")
