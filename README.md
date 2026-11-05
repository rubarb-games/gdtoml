# gdtoml

A Godot addon for reading and writing TOML files.

Supports TOML 1.1. Built for Godot 4.5 and later.

Currently supported platforms:

- Linux
- Windows
- macOS

# Examples

```toml
[settings]
fullscreen = true
volume_master = 100

```

## Reading from a TOML file

```gdscript
var reader: TomlReader = TomlReader.new()
if Toml.parse_file(reader, ["res://sample.toml"]) != OK:
    printerr("Cannot parse TOML file.")
    return

var settings: Dictionary = reader.get_table("settings")
print(settings)

var fullscreen: bool = reader.get_bool("settings.fullscreen")
print("Fullscreen: %s" % fullscreen)
```

Prints:
```
{ "fullscreen": true, "volume_master": 100 }
Fullscreen: true
```

## Writing to a TOML file

```gdscript
var writer: TomlWriter = TomlWriter.new()
writer.set_table("settings", {
    "fullscreen": true,
    "volume_master": 100,
    })

writer.set_bool("settings.vsync", true)

# Serialize to text
var toml_str: String = writer.serialize()
print(toml_str)
```

Prints:
```toml
[settings]
fullscreen = true
volume_master = 100
vsync = true
```

# Install

- Download the zip file from the releases. Extract to your project's 'addon' folder, so that gdtoml is in 'addons/gdtoml'.
- Open the Godot editor, open project settings, and enable the plugin.

# Documentation

In the works.
- [] Explain the API of Writer and Reader
- [] How to extend Toml autoload with helpers

# For development

- init godot-cpp submodule
`git submodule update --init --recursive`
- build compile_commands.json
`scons compiledb`
- if you are using sublime, enable LSP
