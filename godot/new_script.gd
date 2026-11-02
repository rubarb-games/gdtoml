extends Node

func _ready() -> void:
	var e: ExampleClass = ExampleClass.new()
	e.hello()
	#var toml: String = "[section]\n" +\
		#"hello = \"world\""
	#if e.try_parse(toml):
		#print("parsed")
		#var s: String = e.get_string("section.hello")
		#print(s)
	#else:
		#print("nope")
