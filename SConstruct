from platform import system as getPlatform
from warnings import warn
from typing import Literal
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
	CCFLAGS=['-fexceptions', '-pthread'],
	LINKFLAGS=['-fexceptions', '-pthread', "-sPTHREAD_POOL_SIZE=2"],
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
	CPPPATH=[Dir("source"), Dir(F"library/{NATIVE_PLATFORM}/include/")],
	LIBPATH=[*Glob(F"library/{NATIVE_PLATFORM}/lib/*")],
	COMPILATIONDB_USE_ABSPATH=False,
)
baseEnv.Tool('compilation_db')

linuxLibEnv = C.parse(baseEnv.Clone(), C.GCC_CORE, C.GCC_RELEASE)

webLibEnv = C.parse(linuxLibEnv.Clone(), EMSCRIPTEN)
webMainEnv = C.parse(webLibEnv.Clone(), C.GCC_WARNING)
webMainEnv.Append(CXXFLAGS=['-sUSE_SDL=2'])
webMainEnv.Append(LINKFLAGS=['-sUSE_SDL=2'])

def setCompilationDatabasePathFilter(env, buildType: Literal["release", "debug"]):
	# This should be after `Tool('compilation_db')`
	env["COMPILATIONDB_PATH_FILTER"] = F"build/{NATIVE_PLATFORM}/{buildType}/*"
	return env

match NATIVE_PLATFORM:
	case 'Windows':
		nativeLibEnv = C.parse(baseEnv.Clone(), C.MSVC_CORE, C.MSVC_RELEASE)
		setCompilationDatabasePathFilter(
			nativeReleaseMainEnv := C.parse(nativeLibEnv.Clone(), C.MSVC_SDL2, C.MSVC_WARNING),
			buildType="release"
		)
		setCompilationDatabasePathFilter(
			nativeDebugMainEnv := C.parse(baseEnv.Clone(), C.MSVC_CORE, C.MSVC_DEBUG, C.MSVC_SDL2, C.MSVC_WARNING),
			buildType="debug"
		)
	case 'Linux':
		nativeLibEnv = linuxLibEnv
		setCompilationDatabasePathFilter(
			nativeReleaseMainEnv := C.parse(nativeLibEnv.Clone(), C.GCC_WARNING),
			buildType="release"
		).ParseConfig("sdl2-config --cflags --libs")
		setCompilationDatabasePathFilter(
			nativeDebugMainEnv := C.parse(baseEnv.Clone(), C.GCC_CORE, C.GCC_DEBUG, C.GCC_WARNING),
			buildType="debug"
		).ParseConfig("sdl2-config --cflags --libs")

def runScript(mainEnv, libEnv, platform: str, buildType: Literal["release", "debug"]):
	return SConscript(
		"source/SConscript",
		exports={"mainEnv": mainEnv, "libEnv": libEnv, "PLATFORM": platform},
		variant_dir=F"build/{platform}/{buildType}",
		must_exist=True,
		duplicate=0,
	)

mainReleaseProgram, testReleaseProgram, releaseCompilationDatabase = runScript(
	buildType="release",
	mainEnv=nativeReleaseMainEnv,
	libEnv=nativeLibEnv,
	platform=NATIVE_PLATFORM,
)

mainDebugProgram, testDebugProgram, debugCompilationDatabase = runScript(
	buildType="debug",
	mainEnv=nativeDebugMainEnv,
	libEnv=nativeLibEnv,
	platform=NATIVE_PLATFORM,
)

if NATIVE_PLATFORM == "Windows":
	for mainProgram, buildType in ((mainReleaseProgram, "release"), (mainDebugProgram, "debug")):
		windowsSdl2Dll = Command(F"build/Windows/{buildType}/SDL2.dll", "library/Windows/lib/SDL2/SDL2.dll", Copy("$TARGET", "$SOURCE"))
		Depends(mainProgram, windowsSdl2Dll)

if not undiscoveredEmscripten: runScript(
	buildType="release",
	mainEnv=webMainEnv,
	libEnv=webLibEnv,
	platform="web",
)

Default(mainReleaseProgram)
