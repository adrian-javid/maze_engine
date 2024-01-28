from platform import system as GetPlatform

PLATFORM = GetPlatform()

match PLATFORM:
    case 'Windows': cxxFlags = ["/std:c++17", "/EHsc"]
    case 'Linux': cxxFlags = ["-std=c++17"]
    case _: cxxFlags = []

env = Environment(
    CXXFLAGS=cxxFlags,
    CPPPATH=[Dir("source")],
    COMPILATIONDB_USE_ABSPATH=False,
    COMPILATIONDB_PATH_FILTER=F"build/native/*"
)

env.Tool('compilation_db')

SConscript(
    "source/SConscript",
    exports={"env": env, "PLATFORM": PLATFORM},
    variant_dir=F"build/native",
    must_exist=True,
    duplicate=0,
)
