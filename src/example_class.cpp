#include "example_class.h"

void ExampleClass::_bind_methods() {
    godot::ClassDB::bind_method(D_METHOD("print_type", "variant"), &ExampleClass::print_type);
    godot::ClassDB::bind_method(D_METHOD("hello"), &ExampleClass::hello);
}

ExampleClass::ExampleClass() {
    tbl = std::make_unique<toml::table>();
}

void ExampleClass::print_type(const Variant &p_variant) const {
    print_line(vformat("Type: %d", p_variant.get_type()));
}

void ExampleClass::hello() const {
    print_line("Hello");
}