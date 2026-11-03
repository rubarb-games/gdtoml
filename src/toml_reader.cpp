#include "fmt/printf.h"

#include "toml_reader.h"
#include "godot_cpp/variant/utility_functions.hpp"
#include "toml++/impl/forward_declarations.hpp"
#include "toml++/impl/node_view.hpp"
#include <cstdint>
#include <string_view>

#include "utils.h"

// Forward declarations
Variant to_variant(const toml::node& node);

Array TomlReader::to_array(const toml::array& toml_array) {
    Array result;

    for (auto&& element : toml_array) {
        result.append(to_variant(element));
    }

    return result;
}

Dictionary TomlReader::to_dictionary(const toml::table& table) {
    Dictionary dict;

    for (auto&& [key, value] : table) {
        dict[utils::to_string(key.str())] = to_variant(value);
    }

    return dict;
}

Variant TomlReader::to_variant(const toml::node& node) {
    switch (node.type()) {
        case toml::node_type::table:            { return to_dictionary(*node.as_table()); }
        case toml::node_type::array:            { return to_array(*node.as_array()); }
        case toml::node_type::string:           { return utils::to_string(node.value<std::string_view>()); }
        case toml::node_type::integer:          { return node.value<int64_t>().value(); }
        case toml::node_type::floating_point:   { return node.value<double>().value(); }
        case toml::node_type::boolean:          { return node.value<bool>().value(); }
        case toml::node_type::date: {
            std::ostringstream stream;
            stream << node.value<toml::date>().value();
            return utils::to_string(stream.str());
        }
        case toml::node_type::time: {
            std::ostringstream stream;
            stream << node.value<toml::time>().value();
            return utils::to_string(stream.str());
        }
        case toml::node_type::date_time: {
            std::ostringstream stream;
            stream << node.value<toml::date_time>().value();
            return utils::to_string(stream.str());
        }
        default: {
            return {};
        }
    }
}

void TomlReader::_bind_methods() {
    godot::ClassDB::bind_method(D_METHOD("try_parse", "string"), &TomlReader::try_parse);
    godot::ClassDB::bind_method(D_METHOD("enable_log", "value"), &TomlReader::enable_log);
    godot::ClassDB::bind_method(D_METHOD("exists", "key"), &TomlReader::exists);

    godot::ClassDB::bind_method(D_METHOD("get_string", "key"), &TomlReader::get_string);
    godot::ClassDB::bind_method(D_METHOD("get_string_or", "key", "default"), &TomlReader::get_string_or);
    godot::ClassDB::bind_method(D_METHOD("get_string_arr", "key"), &TomlReader::get_string_arr);

    godot::ClassDB::bind_method(D_METHOD("get_int", "key"), &TomlReader::get_int);
    godot::ClassDB::bind_method(D_METHOD("get_int_or", "key", "default"), &TomlReader::get_int_or);
    godot::ClassDB::bind_method(D_METHOD("get_int_arr", "key"), &TomlReader::get_int_arr);

    godot::ClassDB::bind_method(D_METHOD("get_float", "key"), &TomlReader::get_float);
    godot::ClassDB::bind_method(D_METHOD("get_float_or", "key", "default"), &TomlReader::get_float_or);
    godot::ClassDB::bind_method(D_METHOD("get_float_arr", "key"), &TomlReader::get_float_arr);

    godot::ClassDB::bind_method(D_METHOD("get_bool", "key"), &TomlReader::get_bool);
    godot::ClassDB::bind_method(D_METHOD("get_bool_or", "key", "default"), &TomlReader::get_bool_or);
    godot::ClassDB::bind_method(D_METHOD("get_bool_arr", "key"), &TomlReader::get_bool_arr);

    godot::ClassDB::bind_method(D_METHOD("get_array", "key"), &TomlReader::get_array);

    godot::ClassDB::bind_method(D_METHOD("get_table", "key"), &TomlReader::get_table);
    godot::ClassDB::bind_method(D_METHOD("get_table_keys", "key"), &TomlReader::get_table_keys);
}

TomlReader::TomlReader() {
    log_enabled = false;
}

void TomlReader::enable_log(const bool val) {
    log_enabled = val;
}

void TomlReader::log(const std::string& p_msg) const {
    if (!log_enabled) {
        return;
    }
    std::string s = static_cast<std::string>(fmt::sprintf("[gdtoml.toml_reader] {}", p_msg));
    UtilityFunctions::print(utils::to_string(s));
}

bool TomlReader::exists(const String& p_key) const {
    if (!tbl) {
        log(fmt::sprintf("No TOML document parsed. Cannot find key '{}'. Run try_parse first.", utils::to_str(p_key)));
        return false;
    }

    if (tbl->at_path(utils::to_str(p_key)).node() == nullptr) {
        log(fmt::sprintf("TOML document does not contain key '{}'.", utils::to_str(p_key)));
        return false;
    }

    return true;
}

bool TomlReader::try_parse(const String& p_content) {
    std::string content_str = utils::to_str(p_content);
    toml::parse_result result = toml::parse(content_str);

    if (!result) {
        log("Could not parse content");
        return false;
    }

    tbl = std::make_unique<toml::table>(std::move(result).table());
    return true;
}

// String

String TomlReader::get_string(const String& p_key) const {
    return get_string_or(p_key, "");
}

String TomlReader::get_string_or(const String& p_key, const String& p_default) const {
    if (!exists(p_key)) {
        return p_default;
    }

    std::optional<std::string> val = tbl->at_path(utils::to_str(p_key)).value<std::string>();
    if (!val.has_value()) {
        log(fmt::sprintf("Key '{}' is not a string", utils::to_str(p_key)));
        return p_default;
    }

    return utils::to_string(val);
}

TypedArray<String> TomlReader::get_string_arr(const String& p_key) const {
    if (!exists(p_key)) {
        return {};
    }

    toml::node_view<toml::node> node = tbl->at_path(utils::to_str(p_key));

    if (!node.is_array()) {
        log(fmt::sprintf("Key '{}' is not an array.", utils::to_str(p_key)));
        return {};
    }
    
    TypedArray<String> array = {};
    for (auto&& node : *node.as_array()) {
        if (!node.is_string()) {
            continue;
        }
        std::optional<std::string> v = node.value<std::string>();
        if (!v.has_value()) {
            continue;
        }
        array.append(v->c_str());
    }

    return array;
}

// int

int64_t TomlReader::get_int(const String& p_key) const {
    return get_int_or(p_key, -1);
}

int64_t TomlReader::get_int_or(const String& p_key, const int64_t& p_default) const {
    if (!exists(p_key)) {
        return p_default;
    }

    std::optional<int64_t> val = tbl->at_path(utils::to_str(p_key)).value<int64_t>();
    if (!val.has_value()) {
        log(fmt::sprintf("Key '{}' is not an int", utils::to_str(p_key)));
        return p_default;
    }

    return val.value();
}

TypedArray<int64_t> TomlReader::get_int_arr(const String& p_key) const {
    if (!exists(p_key)) {
        return {};
    }

    toml::node_view<toml::node> node = tbl->at_path(utils::to_str(p_key));
    
    if (!node.is_array()) {
        log(fmt::sprintf("Key '{}' is not an array.", utils::to_str(p_key)));
        return {};
    }
    
    TypedArray<int64_t> array = {};
    for (auto&& node : *node.as_array()) {
        if (!node.is_integer()) {
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

float TomlReader::get_float(const String& p_key) const {
    return get_float_or(p_key, -1);
}

float TomlReader::get_float_or(const String& p_key, const float& p_default) const {
    if (!exists(p_key)) {
        return p_default;
    }

    std::optional<float> val = tbl->at_path(utils::to_str(p_key)).value<float>();
    if (!val.has_value()) {
        log(fmt::sprintf("Key '{}' is not a float", utils::to_str(p_key)));
        return p_default;
    }

    return val.value();
}

TypedArray<float> TomlReader::get_float_arr(const String& p_key) const {
    if (!exists(p_key)) {
        return {};
    }

    toml::node_view<toml::node> node = tbl->at_path(utils::to_str(p_key));
    
    if (!node.is_array()) {
        log(fmt::sprintf("Key '{}' is not an array.", utils::to_str(p_key)));
        return {};
    }
    
    TypedArray<float> array = {};
    for (auto&& node : *node.as_array()) {
        if (!node.is_floating_point()) {
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

bool TomlReader::get_bool(const String& p_key) const {
    return get_bool_or(p_key, -1);
}

bool TomlReader::get_bool_or(const String& p_key, const bool& p_default) const {
    if (!exists(p_key)) {
        return p_default;
    }

    std::optional<bool> val = tbl->at_path(utils::to_str(p_key)).value<bool>();
    if (!val.has_value()) {
        log(fmt::sprintf("Key '{}' is not a bool", utils::to_str(p_key)));
        return p_default;
    }

    return val.value();
}

TypedArray<bool> TomlReader::get_bool_arr(const String& p_key) const {
    if (!exists(p_key)) {
        return {};
    }

    toml::node_view<toml::node> node = tbl->at_path(utils::to_str(p_key));
    
    if (!node.is_array()) {
        log(fmt::sprintf("Key '{}' is not an array.", utils::to_str(p_key)));
        return {};
    }
    
    TypedArray<bool> array = {};
    for (auto&& node : *node.as_array()) {
        if (!node.is_boolean()) {
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

Array TomlReader::get_array(const String& p_key) const {
    if (!exists(p_key)) {
        return {};
    }

    toml::node_view<toml::node> node = tbl->at_path(utils::to_str(p_key));
    
    if (!node.is_array()) {
        log(fmt::sprintf("Key '{}' is not an array.", utils::to_str(p_key)));
        return {};
    }
    
    Array array = {};
    for (auto&& node : *node.as_array()) {
        array.append(to_variant(node));
    }

    return array;
}

// dictionary

Dictionary TomlReader::get_table(const String& p_key) const {
    if (!exists(p_key)) {
        return {};
    }

    toml::node_view<toml::node> node = tbl->at_path(utils::to_str(p_key));
    
    if (!node.is_table()) {
        log(fmt::sprintf("Key '{}' is not a table.", utils::to_str(p_key)));
        return {};
    }

    return to_dictionary(*node.as_table());
}

TypedArray<String> TomlReader::get_table_keys(const String& p_key) const {
    if (!exists(p_key)) {
        return {};
    }

    toml::node_view<toml::node> node = tbl->at_path(utils::to_str(p_key));
    
    if (!node.is_table()) {
        log(fmt::sprintf("Key '{}' is not a table.", utils::to_str(p_key)));
        return {};
    }

    TypedArray<String> keys = {};
    for (auto&& [key, value] : *node.as_table()) {
        keys.append(utils::to_string(key.str()));
    }

    return keys;
}
