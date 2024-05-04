from platform import system as getPlatform
from warnings import warn
import build_config as C

NATIVE_PLATFORM = getPlatform()

if NATIVE_PLATFORM not in ('Windows', 'Linux'):
    warn(F"Unsupported platform: \"{NATIVE_PLATFORM}\"")

emscriptenRequirementTools = {
    'CXX': 'em++',
    'CC': 'emcc',
    'AR': 'emar',
    'RANLIB': 'emranlib',
}

EMSCRIPTEN = C.make(
    **{toolType: WhereIs(toolName) for toolType, toolName in emscriptenRequirementTools.items()},
    CCFLAGS=['-fexceptions'],
    LINKFLAGS=['-fexceptions'],
    PROGSUFFIX='.html',
)

undiscoveredEmscripten = [
    F"Could not find `{toolName}`."
    for toolType, toolName in emscriptenRequirementTools.items()
    if toolType not in EMSCRIPTEN
]

if undiscoveredEmscripten:
    warn(F"Cannot build web target. {' '.join(undiscoveredEmscripten)}")

baseEnv = Environment(
    CPPPATH=[Dir("source"), Dir("source/main"), Dir(F"library/{NATIVE_PLATFORM}/include/")],
    LIBPATH=[*Glob(F"library/{NATIVE_PLATFORM}/lib/*")],
    COMPILATIONDB_USE_ABSPATH=False,
    COMPILATIONDB_PATH_FILTER=F"build/{NATIVE_PLATFORM}/*",
)
baseEnv.Tool('compilation_db')

linuxLibEnv = C.parse(baseEnv.Clone(), C.GCC_CORE, C.GCC_RELEASE)

webLibEnv = C.parse(linuxLibEnv.Clone(), EMSCRIPTEN)
webMainEnv = C.parse(webLibEnv.Clone(), C.GCC_WARNING)
webMainEnv.Append(CXXFLAGS=['-sUSE_SDL=2'])
webMainEnv.Append(LINKFLAGS=['-sUSE_SDL=2'])

match NATIVE_PLATFORM:
    case 'Windows':
        nativeLibEnv = C.parse(baseEnv.Clone(), C.MSVC_CORE)
        nativeMainEnv = C.parse(nativeLibEnv.Clone(), C.MSVC_SDL2, C.MSVC_WARNING)
    case 'Linux':
        nativeLibEnv = linuxLibEnv
        nativeMainEnv = C.parse(nativeLibEnv.Clone(), C.GCC_WARNING)
        nativeMainEnv.ParseConfig("sdl2-config --cflags --libs")

def runScript(mainEnv, libEnv, platform: str):
    return SConscript(
        "source/SConscript",
        exports={"mainEnv": mainEnv, "libEnv": libEnv, "PLATFORM": platform},
        variant_dir=F"build/{platform}",
        must_exist=True,
        duplicate=0,
    )

mainProgram, testProgram, compilationDatabase = runScript(nativeMainEnv, nativeLibEnv, NATIVE_PLATFORM)

if NATIVE_PLATFORM == "Windows":
    windowsSdl2Dll = Command("build/Windows/main/SDL2.dll", "library/Windows/lib/SDL2/SDL2.dll", Copy("$TARGET", "$SOURCE"))
    Depends(mainProgram, windowsSdl2Dll)

if not undiscoveredEmscripten: runScript(webMainEnv, webLibEnv, "web")

Default(mainProgram)
