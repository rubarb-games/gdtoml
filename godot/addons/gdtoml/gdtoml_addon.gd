@tool
extends EditorPlugin

func _init():
	add_custom_project_setting("gdtoml/logging", false, TYPE_BOOL)
	var error: int = ProjectSettings.save()
	if error: push_error("Encountered error %d when saving project settings." % error)

func _get_plugin_name():
	return "gdtoml"

func _enter_tree():
	add_autoload_singleton("Toml", "res://addons/gdtoml/toml.gd")

func _exit_tree():
	remove_autoload_singleton("Toml")

func add_custom_project_setting(name: String, default_value, type: int, hint: int = PROPERTY_HINT_NONE, hint_string: String = "") -> void:
	if ProjectSettings.has_setting(name): return
	
	var setting_info: Dictionary = {
		"name": name,
		"type": type,
		"hint": hint,
		"hint_string": hint_string
	}
	
	ProjectSettings.set_setting(name, default_value)
	ProjectSettings.add_property_info(setting_info)
	ProjectSettings.set_initial_value(name, default_value)
