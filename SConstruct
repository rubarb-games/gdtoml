#!/usr/bin/env python
import os
import sys

# from methods import print_error

target = ARGUMENTS.get("target", "template_debug")
platform = ARGUMENTS.get("platform", "linux")

libname = "gdtoml"
projectdir = "godot"

if platform == "windows":
    localEnv = Environment(tools=["msvc"], PLATFORM="", ENV={'PATH': os.environ['PATH']})
else:
    localEnv = Environment(tools=["default"], PLATFORM="", ENV={'PATH': os.environ['PATH']})

localEnv["target"] = target

# Load this BEFORE invoking godot-cpp's SConstruct.
# This is essential for capturing both projects.
localEnv.Tool("compilation_db")
# Use absolute paths in compile_commands.json.
localEnv["COMPILATIONDB_USE_ABSPATH"] = True


localEnv.Append(
    # C++ standard
    CXXFLAGS = [
        "-std=c++20",
    ],
)

if localEnv["target"] == "template_debug":
    localEnv.Append(
        CCFLAGS = [
            "-O0", # no optimization
            "-g3", # max debug symbols
            "-fno-omit-frame-pointer",
            # "-Wpedantic",
            # "-Wshadow",
            # "-Wsign-conversion",
            # "-fsanitize=address,undefined",
        ],

        # LINKFLAGS = [
        #     "-fsanitize=address,undefined",
        # ],

        # SHLINKFLAGS = [
        #     "-fsanitize=address,undefined",
        # ],
    )

elif localEnv["target"] == "template_release":
    localEnv.Append(
        CCFLAGS = [
            "-O1", # optimize low hanging fruits
            "-s", # strip symbol table
            "-g0", # no debug symbols
            "-DNDEBUG",
            #"-flto", # (Link-Time Optimization) — Enables cross-file optimization to remove dead code and inline small functions.
            "-w",
            "-fno-exceptions",
            # "-fno-rtti"  # Disables C++ exception handling and Run-Time Type Information, which drastically cuts down runtime overhead (note: ensure your code doesn't actually throw exceptions)
        ],
        LINKFLAGS = [
            "-flto",
        ],
        SHLINKFLAGS = [
            "-flto",
        ],
    )


# Build profiles can be used to decrease compile times.
# You can either specify "disabled_classes", OR
# explicitly specify "enabled_classes" which disables all other classes.
# Modify the example file as needed and uncomment the line below or
# manually specify the build_profile parameter when running SCons.

localEnv["build_profile"] = "build_profile.json"

# customs = ["custom.py"]
# customs = [os.path.abspath(path) for path in customs]

# opts = Variables(customs, ARGUMENTS)
# opts.Update(localEnv)

# Help(opts.GenerateHelpText(localEnv))

env = localEnv.Clone()

if not (os.path.isdir("godot-cpp") and os.listdir("godot-cpp")):
    print_error("""godot-cpp is not available within this folder, as Git submodules haven't been initialized.
Run the following command to download godot-cpp:

    git submodule update --init --recursive""")
    sys.exit(1)

env = SConscript("godot-cpp/SConstruct", {"api_version": 4.5, "env": env})

env.Append(
    CPPDEFINES=["FMT_HEADER_ONLY"],
    CPPPATH = [
        "#/include",
        "#/src",
    ],
)

sources = []

for root, dirs, files in os.walk("src"):
    for filename in files:
        if filename.endswith((".cpp", ".cc", ".cxx")):
            sources.append(os.path.join(root, filename))

print("Source files:")
for source in sources:
    print("  ", source)

# .dev doesn't inhibit compatibility, so we don't need to key it.
# .universal just means "compatible with all relevant arches" so we don't need to key it.
suffix = env['suffix'].replace(".dev", "").replace(".universal", "")

# remove the lib prefix
env['SHLIBPREFIX'] = ''

lib_filename = "{}{}{}".format(libname, suffix, env.subst('$SHLIBSUFFIX'))
library = env.SharedLibrary(
    "bin/{}/{}".format(env['platform'], lib_filename),
    source=sources,
)


# INSTALL TO ADDONS FOLDER
# =======================

copy = env.Install("{}/addons/{}".format(projectdir, libname), library)
Alias("install", copy)


# COMPILE DB
# =======================

compdb = env.CompilationDatabase(
    "compile_commands.json",
)
Alias("compiledb", compdb)


# DEFAULT COMMAND
# =======================

# default_args = [library, copy]
Default(library)