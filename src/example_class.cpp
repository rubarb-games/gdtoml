#include "example_class.h"

void ExampleClass::_bind_methods() {
    godot::ClassDB::bind_method(D_METHOD("print_type", "variant"), &ExampleClass::print_type);
    godot::ClassDB::bind_method(D_METHOD("hello"), &ExampleClass::hello);
    godot::ClassDB::bind_method(D_METHOD("try_parse", "string"), &ExampleClass::try_parse);
    godot::ClassDB::bind_method(D_METHOD("get_string", "string"), &ExampleClass::get_string);
}

ExampleClass::ExampleClass() {
    // tbl = std::make_unique<toml::table>();
}

inline std::string to_str(const String &p_string) {
    return p_string.utf8().get_data();
}

// What happens if you call try_parse twice on the same instance?
bool ExampleClass::try_parse(const String &p_content) {
    std::string content_str = to_str(p_content);
    toml::parse_result result = toml::parse(content_str);

    if (!result) {
        return false;
    }

    tbl = std::make_unique<toml::table>(std::move(result).table());
    return true;
}

String ExampleClass::get_string(const String &p_key) {
    // validate tbl?

    // const auto &doc = *tbl;
    // auto v = doc[to_str(p_key)].value<std::string>();

    std::optional<std::string> val = tbl->at_path(to_str(p_key)).value<std::string>();
    if (!val.has_value()) {
        return {};
    }
    return {val.value().c_str()};
}

void ExampleClass::print_type(const Variant &p_variant) const {
    print_line(vformat("Type: %d", p_variant.get_type()));
}

void ExampleClass::hello() const {
    print_line("Hello");
}