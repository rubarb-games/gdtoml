# gdtoml

A Godot addon for reading and writing TOML files. Uses the [TOML++](https://marzer.github.io/tomlplusplus/) library for all interactions with the TOML document.

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

## Project settings

In the project settings, you will find a section called "gdbuild". There is only one option at the moment.
Options:

- **logging**: Turn on to enable logging. The setting itself doesn't actually do anything. You have to pass the setting value to TomlReader and TomlWriter. When the setting is located in project settings, it becomes easier to globally turn logging on or off, regardless of where in the code any of the TomlReader or TomlWriter classes are used. In a CI/CD system, you can this way easily toggle logging for different builds. This is how you use it:

```gdscript
var reader: TomlReader = TomlReader.new()
reader.enable_log(Toml.enable_logging())
```


## Paths

The path you pass to any of the `get_` or `set_` functions on TomlReader and TomlWriter supports dot notation and array accessors.

```toml
version = "1.0"

[settings]
fullscreen = true
vsync = true
options = ["left", "right", "down"] # sorry, I couldn't think of any better example

[settings.audio]
volume_master = 100
volume_sfx = 50
```

The version string is orphan, so it can be fetched by calling `get_string("version")`. The fullscreen boolean belongs to a section called 'settings', and has to be fetched by `get_bool("settings.fullscreen")`. The volume settings are one level deeper, and can be fetched by calling `get_int("settings.audio.volume_master")`.

To get the options list, you can use `get_string_arr("settings.options")`, which will return an array of type `Array[String]`. Or you can fetch one of the values directly by using `get_string("settings.options[1]")` to get the `right` value.

## Get Array functions

TomlReader has several functions for getting arrays of specific types. When you use any of the functions that are specific to a type, such as `get_string_arr`, only values of type string will be returned, even if the array contains mixed values.

```toml
mixed_list = ["left", "right", 999, "down"]
```

```gdscript
print(reader.get_string_arr("mixed_list))
```

Will print: `["left", "right", "down"]`

## TomlReader API

### Constructor

The constructor has no parameters. It creates an instance of the TomlReader. TomlReader inherits from RefCounted, so Godot will manage the memory for you.

```gdscript
var reader: TomlReader = TomlReader.new()
```

### enable_log(enable: bool)

Set to true to turn on logging for this instance of TomlReader. Logging will print all errors. Use the "gdbuild/logging" project setting to turn on and off logging globally. The Toml autoload has a function for getting the settings value. Use it like this:

```gdscript
var reader: TomlReader = TomlReader.new()
reader.enable_log(Toml.enable_logging())
```

### try_parse(content: String) -> bool

Pass a string of the TOML file that you want to parse. If it succeeds, the function will return true.
The Toml autoload has a function for loading a file and passing the contents to the TomlReader. Use it like this:

```gdscript
var reader: TomlReader = TomlReader.new()
if Toml.parse_file(reader, ["res://sample.toml"]) != OK:
    printerr("Could not load the toml file.")
    return
```

The `Toml.parse_file()` function accepts an array of file paths. It will iterate through the array, and return the first file that successfully loads and parses. This is useful when a file can be in several places, and you want to prioritize one over the other.

### exists(path: String) -> bool

Returns true when the Toml document has a node at the given path, and false when it doesn't.

### get_string(path: String, default_value: String = "") -> String

Get a string value from the Toml document, using a path like described in the section about paths. If the value cannot be found, the default value will be returned. Defaults to an empty string.

### get_string_arr(path: String) -> Array[String]

Get an array of strings from the Toml document, using a path like described in the section about paths. If the value cannot be found, an empty Array[String] will be returned. Will only return string values, as explained in the section about Array functions.

### get_int(path: String, default_value: int = 0) -> int

Get an int value from the Toml document, using a path like described in the section about paths. If the value cannot be found, the default value will be returned. Defaults to 0.

### get_int_arr(path: String) -> Array[int]

Get an array of int from the Toml document, using a path like described in the section about paths. If the value cannot be found, an empty Array[int] will be returned. Will only return int values, as explained in the section about Array functions.

### get_float(path: String, default_value: float = 0.0) -> float

Get a float value from the Toml document, using a path like described in the section about paths. If the value cannot be found, the default value will be returned. Defaults to 0.0.

### get_float_arr(path: String) -> Array[float]

Get an array of float from the Toml document, using a path like described in the section about paths. If the value cannot be found, an empty Array[float] will be returned. Will only return float values, as explained in the section about Array functions.

### get_bool(path: String, default_value: bool = false) -> bool

Get a bool value from the Toml document, using a path like described in the section about paths. If the value cannot be found, the default value will be returned. Defaults to false.

### get_bool_arr(path: String) -> Array[bool]

Get an array of bool from the Toml document, using a path like described in the section about paths. If the value cannot be found, an empty Array[bool] will be returned. Will only return bool values, as explained in the section about Array functions.

### get_array(path: String) -> Array

Get an array of all values from the Toml document, using a path like described in the section about paths. If the value cannot be found, an empty Array will be returned. When the array contains other arrays or tables, all of the content will be returned.

```toml
[[plugin]]
name = "Forward"

[[plugin]]
name = "Backward"
```

```gdscript
var plugins: Array = reader.get_array("plugin")
```

The plugins variable will be an array of Dictionaries.

### get_table(path: String) -> Dictionary

Returns a Dictionary of all values, including nested Dictionaries.

### get_table_keys(path: String) -> Array[String]

Returns an array of strings containing all the keys of a TOML table.

```toml
[plugins]
forward = true
backward = false
```

```gdscript
var plugins: Array[String] = reader.get_table_keys("plugins")
```

The plugins array will look like: `["forward", "backward"]`.

## TomlWriter API

## Constructor

The constructor has no parameters. It creates an instance of the TomlWriter. TomlWriter inherits from RefCounted, so Godot will manage the memory for you.

## enable_log(enable: bool)

Set to true to turn on logging for this instance of TomlWriter. Logging will print all errors. Use the "gdbuild/logging" project setting to turn on and off logging globally. The Toml autoload has a function for getting the settings value. Use it like this:

```gdscript
var writer: TomlWriter = TomlWriter.new()
writer.enable_log(Toml.enable_logging())
```

## serialize(prune: bool) -> String

Serializes all the values added to the document via the `set_` functions, to a Godot String. The String can be written to a file using Godot's FileAccess API. When `prune` is true, any empty arrays and tables will be discarded. Useful for outputting a clean and tidy TOML.

## set_string(path: String, value: String) -> bool

Set a String value at a given path. Any missing parts of the path will be created before adding the key.

Returns true when the value was successfully set. In a later version, an error code will be returned, to inform you of why the value wasn't successfully set. Possible ways this function can fail:

- The path ends with an array accessor. Paths such as `grandchild.parent[2]` is not supported (yet).
- Could not create the path. Either because the path contains an array accessor, or one of the path elements is not a table.
- The value already exists.

```gdscript
var writer: TomlWriter = TomlWriter.new()
writer.set_string("grandparent.parent.child.name", "Bernard")
```

Will produce the following TOML:

```toml
[grandparent.parent.child]
name = "Bernard"
```

If you later call `writer.set_string("grandparent.name", "Laverne")`, the TOML will look like this:

```toml
[grandparent]
name = "Laverne"

[grandparent.parent.child]
name = "Bernard"
```

## set_int(path: String, value: int) -> bool

Set an int value at a given path. See `set_string` for a more detailed description.

## set_float(path: String, value: float) -> bool

Set a float value at a given path. See `set_string` for a more detailed description.

## set_bool(path: String, value: bool) -> bool

Set a bool value at a given path. See `set_string` for a more detailed description.

## set_array(path: String, value: Array) -> bool

Insert an array at a given path. See `set_string` for a more detailed description.

## set_table(path: String, value: Dictionary) -> bool

Insert a table at a given path. See `set_string` for a more detailed description.

## Toml

The Toml autoload registers with your project when you enable the gdtoml addon. It's purpose to give you some utility functions for making it easier to work with TomlWriter and TomlReader. It already has functions for:

- enable_logging() -> bool: Returns the value from "gdtoml/logging" in Project Settings.
- parse_file(reader: TomlReader, paths: Array[String]) -> int: Loads and parses a TOML file.

There are also a couple of example functions for working with Vectors. TOML does not carry any information about what data type each of the values represent in your project. A data type such as a Vector can be represented in several different ways. gdtoml does not make any decisions for you for how to interpret your data. Feel free to extend the Toml class with any functions you might need for reading and writing data types.

# For development

The godot-cpp project is needed for letting gdtoml know about the Godot API. It's included as a git sub module, and must be initialized like this: `git submodule update --init --recursive`

Depending on your choice of IDE, you might need to build the `compile_commands.json` file. Editors such as Sublime Text requires this file to give you code completion hints. Build the file like this: `scons compiledb`
