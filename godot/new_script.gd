extends Node

func _ready() -> void:
	var e: ExampleClass = ExampleClass.new()
	var t = 1.0
	e.print_type(t)
	e.hello()
