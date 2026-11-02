#include "example_class.h"
#include "toml++/impl/node_view.hpp"

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

// get_string_arr(path)
void ExampleClass::hello() const {
    static constexpr auto source = R"(
        str = "hello world"

        numbers = [ 1, 2, 3, "four", 5.0 ]
        vegetables = [ "tomato", "onion", "mushroom", "lettuce" ]
        minerals = [ "quartz", "iron", "copper", "diamond" ]

        [animals]
        cats = [ "tiger", true, "lion", "puma" ]
        birds = [ "macaw", "pigeon", "canary" ]
        fish = [ "salmon", "trout", "carp" ]
    )";

    toml::parse_result result = toml::parse(source);

    if (!result.succeeded()) {
        print_line("Cannot parse");
        return;
    }

    auto cats = result.table().at_path("animals.cats").as_array();
    for (auto&& node : *cats) {
        std::optional<std::string> s = node.value<std::string>();
        if (!s.has_value()) {
            continue;
        }
        print_line(s->c_str());
    }

    auto a = result.table().at_path("animals");
    if (a.is_table()) {
        for (auto&& [key, value] : *a.as_table()) {
            print_line(key.data());
        }
    }

    // toml::table* animals = result.table().at_path("animals").as_table();
    // animals->prune(); // removes empty nodes

    bool contains = result.table().contains("asdf");
    if (!contains) {
        print_line("doc has no key 'asdf'");
    }


    // auto i = result.table().get_as<int>("str");
    
}