#include "toml_reader.h"
#include "toml++/impl/forward_declarations.hpp"
#include "toml++/impl/node_view.hpp"
#include <cstdint>

#include "utils.h"

void TomlReader::_bind_methods() {
    godot::ClassDB::bind_method(D_METHOD("try_parse", "content"), &TomlReader::try_parse);
    godot::ClassDB::bind_method(D_METHOD("enable_log", "value"), &TomlReader::enable_log);
    godot::ClassDB::bind_method(D_METHOD("exists", "path"), &TomlReader::exists);

    godot::ClassDB::bind_method(D_METHOD("get_string", "path", "default"), &TomlReader::get_string, DEFVAL(""));
    godot::ClassDB::bind_method(D_METHOD("get_string_arr", "path", "default"), &TomlReader::get_string_arr, DEFVAL(TypedArray<String>{}));

    godot::ClassDB::bind_method(D_METHOD("get_int", "path", "default"), &TomlReader::get_int, DEFVAL(0));
    godot::ClassDB::bind_method(D_METHOD("get_int_arr", "path", "default"), &TomlReader::get_int_arr, DEFVAL(TypedArray<int64_t>{}));

    godot::ClassDB::bind_method(D_METHOD("get_float", "path", "default"), &TomlReader::get_float, DEFVAL(0.0));
    godot::ClassDB::bind_method(D_METHOD("get_float_arr", "path", "default"), &TomlReader::get_float_arr, DEFVAL(TypedArray<float>{}));

    godot::ClassDB::bind_method(D_METHOD("get_bool", "path", "default"), &TomlReader::get_bool, DEFVAL(false));
    godot::ClassDB::bind_method(D_METHOD("get_bool_arr", "path", "default"), &TomlReader::get_bool_arr, DEFVAL(TypedArray<bool>{}));

    godot::ClassDB::bind_method(D_METHOD("get_array", "path", "default"), &TomlReader::get_array, DEFVAL(Array{}));

    godot::ClassDB::bind_method(D_METHOD("get_table", "path", "default"), &TomlReader::get_table, DEFVAL(Dictionary{}));
    godot::ClassDB::bind_method(D_METHOD("get_table_keys", "path", "default"), &TomlReader::get_table_keys, DEFVAL(TypedArray<String>{}));
}

TomlReader::TomlReader() {
    log_enabled = false;
}

void TomlReader::enable_log(const bool val) {
    log_enabled = val;
    if (log_enabled) {
        log("Enable logging");
    }
}

bool TomlReader::try_parse(const String& p_content) {
    std::string content_str = utils::to_str(p_content);
    toml::parse_result result = toml::parse(content_str);

    if (!result) {
        std::string s(result.error().description());
        log("Could not parse content: {}", s);
        return false;
    }

    doc = std::make_unique<toml::table>(std::move(result).table());
    return true;
}

bool TomlReader::doc_ok() const {
    if (!doc) {
        log("No valid TOML document. Run try_parse first.");
        return false;
    }
    return true;
}

bool TomlReader::exists(const String& p_path) const {
    if (!doc_ok()) {
        return false;
    }

    const std::string path = utils::to_str(p_path);
    // if (doc->at_path(p_path).node() == nullptr) {
    if (doc->at_path(path).type() == toml::node_type::none) {
        log("TOML document does not contain path '{}'.", path);
        return false;
    }

    return true;
}

// String

String TomlReader::get_string(const String& p_path, const String& p_default) const {
    if (!doc_ok()) {
        return p_default;
    }

    const std::string& path = utils::to_str(p_path);
    const std::string& default_val = utils::to_str(p_default);
    return utils::to_string(get_value(path, default_val));
}

TypedArray<String> TomlReader::get_string_arr(const String& p_path, const TypedArray<String>& p_default) const {
    if (!doc_ok()) {
        return p_default;
    }

    const std::string& path = utils::to_str(p_path);
    toml::node_view<toml::node> node = doc->at_path(path);

    if (node.type() != toml::node_type::array) {
        log("Path '{}' was found, but it's not an array.", path);
        return p_default;
    }
    
    TypedArray<String> array = {};
    for (auto&& node : *node.as_array()) {
        // Check specifically for string. Might not be necessary, but just in case.
        if (!node.is_string()) {
            log("Value at node is '{}'. Expected a string. Value is ignored.", utils::to_str(utils::to_variant(node)));
            continue;
        }
        std::optional<std::string> v = node.value<std::string>();
        if (!v.has_value()) {
            log("Value at node is '{}'. Expected a string. Value is ignored.", utils::to_str(utils::to_variant(node)));
            continue;
        }
        array.append(v->c_str());
    }

    return array;
}

// int

int64_t TomlReader::get_int(const String& p_path, const int64_t& p_default) const {
    if (!doc_ok()) {
        return p_default;
    }

    const std::string& path = utils::to_str(p_path);
    return get_value(path, p_default).value();
}

TypedArray<int64_t> TomlReader::get_int_arr(const String& p_path, const TypedArray<int64_t>& p_default) const {
    if (!doc_ok()) {
        return p_default;
    }

    const std::string& path = utils::to_str(p_path);
    toml::node_view<toml::node> node = doc->at_path(path);

    if (node.type() != toml::node_type::array) {
        log("Path '{}' was found, but it's not an array.", path);
        return p_default;
    }
    
    TypedArray<int64_t> array = {};
    for (auto&& node : *node.as_array()) {
        // Check specifically for integer. A boolean can also count as an integer.
        if (!node.is_integer()) {
            log("Value at node is '{}'. Expected an int. Value is ignored.", utils::to_str(utils::to_variant(node)));
            continue;
        }
        std::optional<int64_t> v = node.value<int64_t>();
        if (!v.has_value()) {
            continue;
        }
        array.append(v.value());
    }

    return array;
}

// float

float TomlReader::get_float(const String& p_path, const float& p_default) const {
    if (!doc_ok()) {
        return p_default;
    }

    const std::string& path = utils::to_str(p_path);
    return get_value(path, p_default).value();
}

TypedArray<float> TomlReader::get_float_arr(const String& p_path, const TypedArray<float>& p_default) const {
    if (!doc_ok()) {
        return p_default;
    }

    const std::string& path = utils::to_str(p_path);
    toml::node_view<toml::node> node = doc->at_path(path);

    if (node.type() != toml::node_type::array) {
        log("Path '{}' was found, but it's not an array.", path);
        return p_default;
    }
    
    TypedArray<float> array = {};
    for (auto&& node : *node.as_array()) {
        // Check specifically for float. Ints can also be parsed as float.
        if (!node.is_floating_point()) {
            log("Value at node is '{}'. Expected a float. Value is ignored.", utils::to_str(utils::to_variant(node)));
            continue;
        }
        std::optional<float> v = node.value<float>();
        if (!v.has_value()) {
            continue;
        }
        array.append(v.value());
    }

    return array;
}

// bool

bool TomlReader::get_bool(const String& p_path, const bool& p_default) const {
    if (!doc_ok()) {
        return p_default;
    }

    const std::string& path = utils::to_str(p_path);
    return get_value(path, p_default).value();
}

TypedArray<bool> TomlReader::get_bool_arr(const String& p_path, const TypedArray<bool>& p_default) const {
    if (!doc_ok()) {
        return p_default;
    }

    const std::string& path = utils::to_str(p_path);
    toml::node_view<toml::node> node = doc->at_path(path);

    if (node.type() != toml::node_type::array) {
        log("Path '{}' was found, but it's not an array.", path);
        return p_default;
    }
    
    TypedArray<bool> array = {};
    for (auto&& node : *node.as_array()) {
        // Check specifically for bool. Ints can also be parsed as float.
        if (!node.is_boolean()) {
            log("Value at node is '{}'. Expected a bool. Value is ignored.", utils::to_str(utils::to_variant(node)));
            continue;
        }
        std::optional<bool> v = node.value<bool>();
        if (!v.has_value()) {
            continue;
        }
        array.append(v.value());
    }

    return array;
}

// array

Array TomlReader::get_array(const String& p_path, const Array& p_default) const {
    if (!doc_ok()) {
        return p_default;
    }

    const std::string& path = utils::to_str(p_path);
    toml::node_view<toml::node> node = doc->at_path(path);

    if (node.type() != toml::node_type::array) {
        log("Path '{}' was found, but it's not an array.", path);
        return p_default;
    }
    
    Array array = {};
    for (auto&& node : *node.as_array()) {
        array.append(utils::to_variant(node));
    }

    return array;
}

// dictionary

Dictionary TomlReader::get_table(const String& p_path, const Dictionary& p_default) const {
    if (!doc_ok()) {
        return p_default;
    }

    const std::string& path = utils::to_str(p_path);
    toml::node_view<toml::node> node = doc->at_path(path);

    if (node.type() != toml::node_type::table) {
        log("Path '{}' was found, but it's not a table.", path);
        return p_default;
    }

    return utils::to_dictionary(*node.as_table());
}

TypedArray<String> TomlReader::get_table_keys(const String& p_path, const TypedArray<String>& p_default) const {
    if (!doc_ok()) {
        return p_default;
    }

    const std::string& path = utils::to_str(p_path);
    toml::node_view<toml::node> node = doc->at_path(path);

    if (node.type() != toml::node_type::table) {
        log("Path '{}' was found, but it's not a table.", path);
        return p_default;
    }

    TypedArray<String> keys = {};
    for (auto&& [key, value] : *node.as_table()) {
        keys.append(utils::to_string(key.str()));
    }

    return keys;
}
