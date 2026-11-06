extends Node

func enable_logging() -> bool:
	return ProjectSettings.get_setting("gdtoml/logging", false)

func parse_file(p_reader: TomlReader, p_paths: Array[String]) -> int:
	for path: String in p_paths:
		var file = FileAccess.open(path, FileAccess.READ)
		if file == null:
			continue
		var content = file.get_as_text()
		if p_reader.try_parse(content):
			return OK
	return ERR_CANT_OPEN

func to_vec2(arr: Array[float]) -> Vector2:
	if arr.size() != 2:
		Vector2.ZERO
	return Vector2(arr[0], arr[1])

func from_vec2(vec: Vector2) -> Array:
	return [vec.x, vec.y]

func to_vec2i(arr: Array[int]) -> Vector2i:
	if arr.size() != 2:
		Vector2i.ZERO
	return Vector2i(arr[0], arr[1])

func from_vec2i(vec: Vector2i) -> Array:
	return [vec.x, vec.y]

func as_vec3(arr: Array[float]) -> Vector3:
	if arr.size() != 3:
		Vector3.ZERO
	return Vector3(arr[0], arr[1], arr[2])

func from_vec3(vec: Vector3) -> Array:
	return [vec.x, vec.y, vec.z]

func as_vec3i(arr: Array[int]) -> Vector3i:
	if arr.size() != 3:
		Vector3i.ZERO
	return Vector3i(arr[0], arr[1], arr[2])

func from_vec3i(vec: Vector3i) -> Array:
	return [vec.x, vec.y, vec.z]
