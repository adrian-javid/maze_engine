from platform import system as getPlatform

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
    CPPPATH=[Dir("source"), Dir(F"library/{PLATFORM}/include/")],
    LIBPATH=[Dir(F"library/{PLATFORM}/lib/")],
    LIBS=["SDL2"],
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
