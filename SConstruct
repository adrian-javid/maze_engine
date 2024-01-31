from platform import system as getPlatform

NATIVE_PLATFORM = getPlatform()

if NATIVE_PLATFORM not in ('Windows', 'Linux'):
    raise RuntimeError(F"Unsupported platform: \"{NATIVE_PLATFORM}\"")

baseEnv = Environment(
    CPPPATH=[Dir("source"), Dir("source/main"), Dir(F"library/{NATIVE_PLATFORM}/include/")],
    LIBPATH=[*Glob(F"library/{NATIVE_PLATFORM}/lib/*")],
    COMPILATIONDB_USE_ABSPATH=False,
    COMPILATIONDB_PATH_FILTER=F"build/{NATIVE_PLATFORM}/*"
)
baseEnv.Tool('compilation_db')

match NATIVE_PLATFORM:
    case 'Windows':
        baseEnv.Append(CXXFLAGS=[
            "/permissive-", # stricter conformance to C++ standard
            "/std:c++17",
            "/EHsc",
        ])
        baseEnv.Append(LINKFLAGS=["/SUBSYSTEM:CONSOLE"])
    case 'Linux':
        baseEnv.Append(CXXFLAGS=["-std=c++17"])

mainEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

mainEnv['LIBS'] = ["SDL2"]
match NATIVE_PLATFORM:
    case 'Windows':
        mainEnv.Append(LIBS=["SDL2main", "shell32"])
        mainEnv.Append(CXXFLAGS=["/W4"])
    case 'Linux':
        mainEnv.ParseConfig("sdl2-config --cflags --libs")
        mainEnv.Append(CXXFLAGS=[
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
        ])

mainProgram, testProgram, compilationDatabase = SConscript(
    "source/SConscript",
    exports={"mainEnv": mainEnv, "libEnv": libEnv, "PLATFORM": NATIVE_PLATFORM},
    variant_dir=F"build/{NATIVE_PLATFORM}",
    must_exist=True,
    duplicate=0,
)

Default(mainProgram)
