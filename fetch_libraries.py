from urllib.request import urlretrieve as download
from pathlib import Path
from zipfile import ZipFile
from shutil import copytree as copyRecursively

# Create a `cache` directory to store downloaded files.
Path("cache/").mkdir(parents=True, exist_ok=True)

for url, includeDir, libDir, platform, simpleName in (

    # This is information for downloading SDL for 64-bit Windows.
    (
        "https://github.com/libsdl-org/SDL/releases/download/release-2.28.5/SDL2-devel-2.28.5-VC.zip",
        "SDL2-2.28.5/include/",
        "SDL2-2.28.5/lib/x64/",
        'Windows',
        "SDL2",
    ),

):
    basename = url[url.rindex("/")+1:]

    zipFilePath = Path(F"cache/{basename}")

    # Download the ZIP file containing SDL.
    if not zipFilePath.exists(): download(url=url, filename=zipFilePath)

    with ZipFile(zipFilePath) as zipFile:

        includeMembers = []; libMembers = []

        for member in zipFile.namelist():
            if member.startswith(includeDir): includeMembers.append(member)
            if member.startswith(libDir): includeMembers.append(member)

        # Extract all header files.
        zipFile.extractall(members=includeMembers, path="cache/")
        
        # Extract all library binaries.
        zipFile.extractall(members=libMembers, path="cache/")
        
        # Copy header files to relevant directory.
        if not (libraryPath := Path(F"library/{platform}/include/{simpleName}")).exists():
            copyRecursively(dst=str(libraryPath), src=F"cache/{includeDir}/")

        # Copy library binaries to relevant directory.
        if not (libraryPath := Path(F"library/{platform}/lib/{simpleName}")).exists():    
            copyRecursively(dst=libraryPath, src=F"cache/{libDir}/")
