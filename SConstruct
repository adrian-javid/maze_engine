from platform import system as getPlatform

NATIVE_PLATFORM = getPlatform()

libs = ["SDL2"]

match NATIVE_PLATFORM:
    case 'Windows':
        cxxFlags = ["/std:c++17", "/EHsc"]
        linkFlags = ["/SUBSYSTEM:CONSOLE"]
        libs = [*libs, "SDL2main", "shell32"]
    case 'Linux':
        cxxFlags = ["-std=c++17"]
        linkFlags = []
    case _:
        raise RuntimeError(F"Unsupported platform: \"{NATIVE_PLATFORM}\"")

env = Environment(
    CXXFLAGS=cxxFlags,
    LINKFLAGS=linkFlags,
    CPPPATH=[Dir("source"), Dir(F"library/{NATIVE_PLATFORM}/include/")],
    LIBPATH=[*Glob(F"library/{NATIVE_PLATFORM}/lib/*")],
    LIBS=libs,
    COMPILATIONDB_USE_ABSPATH=False,
    COMPILATIONDB_PATH_FILTER=F"build/native/*"
)

env.Tool('compilation_db')

if NATIVE_PLATFORM == 'Linux': env.ParseConfig('sdl2-config --cflags --libs')

program, compilationDatabase = SConscript(
    "source/SConscript",
    exports={"env": env, "PLATFORM": NATIVE_PLATFORM},
    variant_dir=F"build/native",
    must_exist=True,
    duplicate=0,
)

Default(program)
