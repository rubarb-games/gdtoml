extends Node

func _ready() -> void:
	var reader: TomlReader = TomlReader.new()
	var code: int = Toml.parse_file(reader, ["res://sample.toml"])
	if code != OK:
		printerr(error_string(code))
		return
	
	exists(reader, "alone")
	exists(reader, "general")
	exists(reader, "general.width")
	exists(reader, "width")
	
	var general: Dictionary = reader.get_table("general")
	print(general)
	var audio: Dictionary = reader.get_table("general.audio")
	print(audio)
	
	var general_err: Dictionary = reader.get_table("general_not_found")
	print(general_err)
	
	var plugins_1: Array[String] = reader.get_string_arr("general.plugins")
	print(plugins_1)
	
	var plugins_2: Array = reader.get_array("general.plugins")
	print(plugins_2)
	
	var level: Dictionary = reader.get_table("level")
	print(level)
	
	var enemies: Array = reader.get_array("enemy")
	print(enemies)
	
	var my_name: Dictionary = reader.get_table("my_name")
	print(my_name)
	
	expect_arr(
		reader.get_array("general.random"),
		[1, 2.5, false, "string"],
		"general.random"
	)
	expect_arr(
		reader.get_int_arr("general.random"),
		[1],
		"general.random (int)"
	)
	expect_arr(
		reader.get_float_arr("general.random"),
		[2.5],
		"general.random (float)"
	)
	expect_arr(
		reader.get_bool_arr("general.random"),
		[false],
		"general.random (bool)"
	)
	expect_arr(
		reader.get_string_arr("general.random"),
		["string"],
		"general.random (string)"
	)

static func expect_arr(arr: Array, to_be: Array, msg: String) -> void:
	if arr.size() != to_be.size():
		printerr("%s have different size" % msg)
		return
	
	for i: int in range(0, arr.size()):
		if arr[i] != to_be[i]:
			printerr("%s are not equal" % msg)
	
	print("%s are equal" % msg)

static func exists(reader: TomlReader, key: String) -> void:
	print("Has '%s': %s" % [key, yesno(reader.exists(key))])

static func yesno(b: bool) -> String:
	return "Yes" if b else "No"
