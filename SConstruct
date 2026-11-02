#!/usr/bin/env python
import os
import sys

# from methods import print_error

target = ARGUMENTS.get("target", "template_debug")

libname = "asdf"
projectdir = "godot"

localEnv = Environment(tools=["default"], PLATFORM="")

localEnv["target"] = target

# Load this BEFORE invoking godot-cpp's SConstruct.
# This is essential for capturing both projects.
localEnv.Tool("compilation_db")
# Use absolute paths in compile_commands.json.
localEnv["COMPILATIONDB_USE_ABSPATH"] = True

# Build profiles can be used to decrease compile times.
# You can either specify "disabled_classes", OR
# explicitly specify "enabled_classes" which disables all other classes.
# Modify the example file as needed and uncomment the line below or
# manually specify the build_profile parameter when running SCons.

# localEnv["build_profile"] = "build_profile.json"

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

copy = env.Install("{}/addons/{}".format(projectdir, libname), library)

# COMPILE DB
# =======================

compdb = env.CompilationDatabase(
    "compile_commands.json",
)

Alias("compiledb", compdb)


default_args = [library, copy]
Default(*default_args)