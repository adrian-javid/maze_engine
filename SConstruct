from platform import system as getPlatform

NATIVE_PLATFORM = getPlatform()

libs = ["SDL2"]

match NATIVE_PLATFORM:
    case 'Windows':
        cxxFlags = [
            "/permissive-", # stricter conformance to C++ standard
            "/std:c++17",
            "/EHsc",
        ]
        linkFlags = ["/SUBSYSTEM:CONSOLE"]
        libs = [*libs, "SDL2main", "shell32"]
    case 'Linux':
        cxxFlags = ["-std=c++17"] + [
            '-Wall', '-Wextra', '-Wpedantic', '-Wdeprecated',
            '-Wconversion', '-Wunused', '-Wformat=2', '-Wreorder', '-Wuninitialized',

            '-Wold-style-cast', '-Wcast-qual', '-Wcast-function-type',
            '-Wcast-align', '-Wunused-macros', '-Wzero-as-null-pointer-constant',

            '-Wundef', # undefined identifier in `#if` directive
            '-Wnon-virtual-dtor', # virtual functions but non-virtual destructor
            '-Wdouble-promotion', # float implicitly promoted to double
            '-Walloca', # usage of `alloca`
            '-Wimplicit-fallthrough', # unannotated fall-through between switch labels

            '-Wno-error=deprecated-declarations',
        ]
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
    COMPILATIONDB_PATH_FILTER=F"build/{NATIVE_PLATFORM}/*"
)

env.Tool('compilation_db')

if NATIVE_PLATFORM == 'Linux': env.ParseConfig('sdl2-config --cflags --libs')

program, compilationDatabase = SConscript(
    "source/SConscript",
    exports={"env": env, "PLATFORM": NATIVE_PLATFORM},
    variant_dir=F"build/{NATIVE_PLATFORM}",
    must_exist=True,
    duplicate=0,
)

Default(program)
