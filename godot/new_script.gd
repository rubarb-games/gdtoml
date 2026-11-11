extends Node

func _ready() -> void:
	test_reader()
	test_writer()

func test_writer() -> void:
	var writer: TomlWriter = TomlWriter.new()
	
	writer.set_string("grandparent.parent.child.name", "Bernard")
	writer.set_string("grandparent.name", "Laverne")
	
	writer.set_string("settings.path", "file://filename.txt")
	writer.set_bool("settings.enabled", true)
	writer.set_int("settings.seed", 1704)
	writer.set_float("settings.volume", 0.75)
	
	writer.set_bool("settings.graphics.vsync_enabled", false)
	writer.set_bool("settings.list[1]", false)
	
	writer.set_array("letters", ["a", "b", "c"])
	writer.set_array("letters", ["a", "b", "c", "d"]) # not allowed
	writer.set_string("letters[3]", "d") # not supported
	
	writer.set_table("table", {
		"a": 1,
		"b": 2,
	})
	writer.set_string("table.c", "hallo")
	
	writer.set_array("numbers", []) # won't serialize when pruning
	writer.set_table("empty", {}) # won't serialize when pruning
	
	print(writer.serialize())

func test_reader() -> void:
	var reader: TomlReader = TomlReader.new()
	reader.enable_log(Toml.enable_logging())
	
	var code: int = Toml.parse_file(reader, ["res://sample.toml"])
	if code != OK:
		printerr(error_string(code))
		return
	
	exists(reader, "alone")
	print("Alone: %s" % reader.get_bool("alone"))
	exists(reader, "intvalue")
	print("Intvalue: %s" % reader.get_int("intvalue"))
	exists(reader, "something")
	print("Something: %s" % reader.get_string("something", "ERR"))
	#return
	
	exists(reader, "general")
	exists(reader, "general.width")
	exists(reader, "width")
	
	print_table(reader, "general")
	print_table(reader, "general.audio")
	print_table(reader, "xxx") # should print nothing
	
	expect_arr(
		reader.get_string_arr("general.plugins"),
		["this", "that"],
		"general.plugins"
	)
	
	print_table(reader, "level")
	
	print_array(reader, "enemy")
	expect_arr(
		reader.get_array("enemy"),
		[{"name":"Peasant"}, {"name":"Accountant"}],
		"enemies"
	)
	
	print_table(reader, "my_name")
	expect_table(
		reader.get_table("my_name"),
		{"first":"Thomas", "last":"Viktil"},
		"my_name"
	)
	
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
	
	var keys: Array[String] = reader.get_table_keys("general")
	print(keys)

static func print_array(r: TomlReader, path: String) -> void:
	var a: Array = r.get_array(path)
	print("%s:\n%s" % [path, JSON.stringify(a, "  ")])

static func print_table(r: TomlReader, path: String) -> void:
	var d: Dictionary = r.get_table(path)
	print("%s:\n%s" % [path, JSON.stringify(d, "  ")])

static func expect_table(tbl: Dictionary, to_be: Dictionary, msg: String) -> void:
	if tbl.keys().size() != to_be.keys().size():
		printerr("%s are not equal" % msg)
		return
	
	for key: Variant in tbl:
		if not to_be.has(key):
			printerr("%s are not equal" % msg)
			return
		
		if tbl[key] != to_be[key]:
			printerr("%s are not equal" % msg)
			return
	
	print("%s are equal" % msg)

static func expect_arr(arr: Array, to_be: Array, msg: String) -> void:
	if arr.size() != to_be.size():
		printerr("%s have different size" % msg)
		return
	
	for i: int in range(0, arr.size()):
		if arr[i] != to_be[i]:
			printerr("%s are not equal" % msg)
			return
	
	print("%s are equal" % msg)

static func exists(reader: TomlReader, key: String) -> void:
	print("Has '%s': %s" % [key, yesno(reader.exists(key))])

static func yesno(b: bool) -> String:
	return "Yes" if b else "No"
