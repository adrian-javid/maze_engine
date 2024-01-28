#!/usr/bin/env python3

from urllib.request import urlretrieve as download
from pathlib import Path
from zipfile import ZipFile
from shutil import copytree as copyTree

Path("cache/").mkdir(parents=True, exist_ok=True)

for url, includeDir, libDir, platform, simpleName in (
    (
        "https://github.com/libsdl-org/SDL/releases/download/release-2.28.5/SDL2-devel-2.28.5-VC.zip",
        "SDL2-2.28.5/include/",
        "SDL2-2.28.5/lib/x64/",
        'Windows',
        "SDL2"
    ),
):
    basename = url[url.rindex("/")+1:]

    zipFilePath = Path(F"cache/{basename}")
    if not zipFilePath.exists(): download(url=url, filename=zipFilePath)

    with ZipFile(zipFilePath) as zipFile:
        includeMembers = []; libMembers = []
        for member in zipFile.namelist():
            if member.startswith(includeDir): includeMembers.append(member)
            if member.startswith(libDir): includeMembers.append(member)
        zipFile.extractall(members=includeMembers, path="cache/")
        zipFile.extractall(members=libMembers, path="cache/")
        
        if not (libraryPath := Path(F"library/{platform}/include/{simpleName}")).exists():
            copyTree(dst=str(libraryPath), src=F"cache/{includeDir}/")

        if not (libraryPath := Path(F"library/{platform}/lib/{simpleName}")).exists():    
            copyTree(dst=libraryPath, src=F"cache/{libDir}/")
