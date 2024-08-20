
from typing import TypeAlias

EnvConfig: TypeAlias = dict[str, str|list[str]]

def make(*,
	CC            :      str  | None = None,
	CXX           :      str  | None = None,
	CFLAGS        : list[str] | None = None,
	CCFLAGS       : list[str] | None = None,
	CXXFLAGS      : list[str] | None = None,
	LINKFLAGS     : list[str] | None = None,
	LIBS          : list[str] | None = None,
	CPPDEFINES    : list[str] | None = None,
	AR            :      str  | None = None,
	RANLIB        :      str  | None = None,
	PROGSUFFIX    :      str  | None = None,
	LIBPREFIX     :      str  | None = None,
	LIBSUFFIX     :      str  | None = None,
	SHLIBPREFIX   :      str  | None = None,
	SHLIBSUFFIX   :      str  | None = None,
) -> EnvConfig:
	'''
	`CC`          : C compiler
	`CXX`         : C++ compiler
	`CFLAGS`      : C exclusive flags
	`CCFLAGS`     : C & C++ mutual flags
	`CXXFLAGS`    : C++ exclusive flags 
	`LINKFLAGS`   : linker flags
	`LIBS`        : libraries
	`CPPDEFINES`  : C preprocessor definitions
	`AR`          : archiver
	`RANLIB`      : indexer
	`PROGSUFFIX`  : program suffix
	`LIBPREFIX`   : static library prefix
	`LIBSUFFIX`   : static library suffix
	`SHLIBPREFIX` : shared library prefix
	`SHLIBSUFFIX` : shared library suffix
	'''
	return {key: value for key, value in locals().items() if value}

def parse(env, *configList: EnvConfig):
	for config in configList:
		for key, value in config.items():
			if isinstance(value, str): env[key] = value
			else: env.Append(**{key: value})
	return env

#region GCC

GCC_CORE = make(CXXFLAGS=["-std=c++17"])

GCC_WARNING = make(CXXFLAGS=[
	'-Wall', '-Wextra', '-Wpedantic', '-Wdeprecated',
	'-Wconversion', '-Wunused', '-Wformat=2', '-Wreorder', '-Wuninitialized',

	'-Wold-style-cast', '-Wcast-qual', '-Wcast-function-type',
	'-Wcast-align', '-Wunused-macros', '-Wzero-as-null-pointer-constant',

	'-Wreturn-local-addr',

	'-Wundef', # undefined identifier in `#if` directive
	'-Wnon-virtual-dtor', # virtual functions but non-virtual destructor
	'-Wdouble-promotion', # float implicitly promoted to double
	'-Walloca', # usage of `alloca`
	'-Wimplicit-fallthrough', # unannotated fall-through between switch labels

	'-Wno-error=deprecated-declarations', # deprecation warnings are not errors
])

GCC_RELEASE = make(
	CCFLAGS=['-O3'],
	CPPDEFINES=['NDEBUG'],
)

GCC_DEBUG = make(
	CCFLAGS=[
		"-O0", # zero optimization for faster compilation
		"-g", # add debug symbols
		"-Werror", # treat warnings as errors
	],
	CPPDEFINES=[
		"_GLIBCXX_DEBUG",
		"_GLIBCXX_DEBUG_PEDANTIC",
		"_LIBCPP_DEBUG=1", # for Clang's `libc++`
	]
)

#endregion

#region MSVC

MSVC_CORE = make(
	CXXFLAGS=[
		"/std:c++17",
		"/EHsc",
		"/permissive-", # stricter conformance to C++ standard
	],
	LINKFLAGS=["/SUBSYSTEM:CONSOLE"],
)

MSVC_RELEASE = make(
	CXXFLAGS=["/O2"],
	CPPDEFINES=["NDEBUG"],
)

MSVC_DEBUG = make(
	CXXFLAGS=[
		"/Od", # disable optimization for faster compilation
		"/WX", # treat warnings as errors
		"/RTC1", # runtime checks
	],
)

MSVC_WARNING = make(CXXFLAGS=[
	"/W4",
	"/w45262", # warn on implicit fall-through in switch statement after `case` label
	"/wd4996", # suppress deprecation warnings so they are not errors
])

MSVC_SDL2 = make(LIBS=["SDL2", "SDL2main", "shell32"])

#endregion
