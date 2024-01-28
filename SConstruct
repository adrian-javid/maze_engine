from platform import system as getPlatform
from urllib.request import urlretrieve as download
from pathlib import Path
from zipfile import ZipFile

Path("cache/").mkdir(parents=True, exist_ok=True)

for url, includeDir, libDir, platform, simpleName in (
    (
        "https://github.com/libsdl-org/SDL/releases/download/release-2.28.5/SDL2-devel-2.28.5-VC.zip",
        "SDL2-2.28.5/include/",
        "SDL2-2.28.5/lib/x64/",
        'Windows',
        "sdl2"
    ),
):
    basename = url[url.rindex("/")+1:]

    zipFilePath = Path(F"cache/{basename}")
    if not zipFilePath.exists(): download(url=url, filename=zipFilePath)

    itemPath = Path(F"cache/{zipFilePath.stem}")
    if not itemPath.exists():
        with ZipFile(zipFilePath) as zipFile:
            includeMembers = []; libMembers = []
            for member in zipFile.namelist():
                if member.startswith(includeDir): includeMembers.append(member)
                if member.startswith(libDir): includeMembers.append(member)
            zipFile.extractall(members=includeMembers, path="cache/")
            zipFile.extractall(members=libMembers, path="cache/")
            n = Command(Dir(F"library/{platform}/include/{simpleName}"), F"cache/{includeDir}/", Copy("$TARGET", "$SOURCE"))
            Command(Dir(F"library/{platform}/lib/{simpleName}"), F"cache/{libDir}/", Copy("$TARGET", "$SOURCE"))

PLATFORM = getPlatform()

match PLATFORM:
    case 'Windows':
        cxxFlags = ["/std:c++17", "/EHsc"]
    case 'Linux':
        cxxFlags = ["-std=c++17"]
    case _:
        raise RuntimeError(F"Unsupported platform: \"{PLATFORM}\"")

env = Environment(
    CXXFLAGS=cxxFlags,
    CPPPATH=[Dir("source"), *Glob(F"library/{PLATFORM}/include/*")],
    LIBPATH=Glob(F"library/{PLATFORM}/lib/*"),
    COMPILATIONDB_USE_ABSPATH=False,
    COMPILATIONDB_PATH_FILTER=F"build/native/*"
)
env.Tool('compilation_db')

program, compilationDatabase = SConscript(
    "source/SConscript",
    exports={"env": env, "PLATFORM": PLATFORM},
    variant_dir=F"build/native",
    must_exist=True,
    duplicate=0,
)

Default(program, compilationDatabase)
