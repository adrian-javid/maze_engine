
AddOption(
	'--emscripten-only',
	dest='emscripten_only',
	action='store_true',
	default=False,
	help='Only build Emscripten targets and ignore requirements for other targets.',
)

emscriptenOnly: bool = GetOption("emscripten_only")

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
	CCFLAGS=['-fexceptions'],
	LINKFLAGS=['-fexceptions'],
	PROGSUFFIX='.js',
)

undiscoveredEmscripten = [
	F"Could not find `{toolName}`."
	for toolType, toolName in emscriptenRequirementTools.items()
	if toolType not in EMSCRIPTEN
]

if undiscoveredEmscripten:
	errorString: str = F"Cannot build web target. {' '.join(undiscoveredEmscripten)}"
	if emscriptenOnly: Exit(errorString)
	else: warn(errorString)

baseEnv = Environment(
	CPPPATH=[Dir("source"), Dir(F"library/{NATIVE_PLATFORM}/include/")],
	LIBPATH=[*Glob(F"library/{NATIVE_PLATFORM}/lib/*")],
	COMPILATIONDB_USE_ABSPATH=False,
)
baseEnv.Tool('compilation_db')

linuxLibEnv = C.parse(baseEnv.Clone(), C.GCC_CORE, C.GCC_RELEASE)

def setCompilationDatabasePathFilter(env, buildType: Literal["release", "debug"], platform: str=NATIVE_PLATFORM):
	# This should be after `Tool('compilation_db')`
	env["COMPILATIONDB_PATH_FILTER"] = F"build/{platform}/{buildType}/*"
	return env

webLibEnv = C.parse(linuxLibEnv.Clone(), EMSCRIPTEN)
webMainEnv = C.parse(webLibEnv.Clone(), C.GCC_WARNING)
setCompilationDatabasePathFilter(webMainEnv, platform='web', buildType="release")
webMainEnv.Append(CXXFLAGS=['--use-port=sdl2', '--use-port=sdl2_mixer'])
webMainEnv.Append(LINKFLAGS=['--use-port=sdl2', '--use-port=sdl2_mixer', '-lembind'])

if not emscriptenOnly:
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
			LINUX_PKG_CONFIG: str = "pkg-config --cflags --libs sdl2 SDL2_mixer"
			setCompilationDatabasePathFilter(
				nativeReleaseMainEnv := C.parse(nativeLibEnv.Clone(), C.GCC_WARNING),
				buildType="release"
			).ParseConfig(LINUX_PKG_CONFIG)
			setCompilationDatabasePathFilter(
				nativeDebugMainEnv := C.parse(baseEnv.Clone(), C.GCC_CORE, C.GCC_DEBUG, C.GCC_WARNING),
				buildType="debug"
			).ParseConfig(LINUX_PKG_CONFIG)

def runScript(mainEnv, libEnv, platform: str, buildType: Literal["release", "debug"]):
	return SConscript(
		"source/SConscript",
		exports={"mainEnv": mainEnv, "libEnv": libEnv, "PLATFORM": platform},
		variant_dir=F"build/{platform}/{buildType}",
		must_exist=True,
		duplicate=0,
	)

if not emscriptenOnly:
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
			windowsSdl2Dll = Command(F"build/Windows/{buildType}/run/SDL2.dll", "library/Windows/lib/SDL2/SDL2.dll", Copy("$TARGET", "$SOURCE"))
			Depends(mainProgram, windowsSdl2Dll)

if not undiscoveredEmscripten:
	webBuildType: str = "release"
	webProgram, webTestProgram, webCompilationDatabase = runScript(
		buildType=webBuildType,
		mainEnv=webMainEnv,
		libEnv=webLibEnv,
		platform="web",
	)
	javaScript = Command(F"website/generated/maze_engine.js", F"build/web/{webBuildType}/run/maze_engine.js", Copy("$TARGET", "$SOURCE"))
	Depends(javaScript, webProgram)
	webAssembly = Command(F"website/generated/maze_engine.wasm", F"build/web/{webBuildType}/run/maze_engine.wasm", Copy("$TARGET", "$SOURCE"))
	Depends(webAssembly, webProgram)

if not emscriptenOnly: Default(mainReleaseProgram)
