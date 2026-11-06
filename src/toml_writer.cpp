#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

#include "fmt/printf.h"
#include "toml++/impl/forward_declarations.hpp"

#include <cstdint>

#include "toml_writer.h"
#include "utils.h"

void TomlWriter::_bind_methods() {
    godot::ClassDB::bind_method(D_METHOD("enable_log", "value"), &TomlWriter::enable_log);
    godot::ClassDB::bind_method(D_METHOD("serialize", "prune"), &TomlWriter::serialize, DEFVAL(true));
    
    godot::ClassDB::bind_method(D_METHOD("set_string", "path", "value"), &TomlWriter::set_string);
    godot::ClassDB::bind_method(D_METHOD("set_int", "path", "value"), &TomlWriter::set_int);
    godot::ClassDB::bind_method(D_METHOD("set_float", "path", "value"), &TomlWriter::set_float);
    godot::ClassDB::bind_method(D_METHOD("set_bool", "path", "value"), &TomlWriter::set_bool);
    
    godot::ClassDB::bind_method(D_METHOD("set_array", "path", "array"), &TomlWriter::set_array);
    godot::ClassDB::bind_method(D_METHOD("set_table", "path", "dictionary"), &TomlWriter::set_table);
}

TomlWriter::TomlWriter() {
    tbl = std::make_unique<toml::table>();
}

void TomlWriter::enable_log(const bool val) {
    log_enabled = val;
}

String TomlWriter::serialize(bool prune = true) {
    if (prune) {
        tbl->prune();
    }

    toml::format_flags flags = 
        toml::format_flags::allow_unicode_strings |
        toml::format_flags::allow_multi_line_strings;

    std::ostringstream stream;
    stream << toml::toml_formatter{ *tbl, flags };
    return utils::to_string(stream.str());
}

// Basic data types

bool TomlWriter::set_string(const String& p_path, const String& p_value) {
    return utils::inject_value(*tbl, toml::path{ utils::to_str(p_path) }, utils::to_str(p_value));
}

bool TomlWriter::set_int(const String& p_path, const int& p_value) {
    return utils::inject_value(*tbl, toml::path{ utils::to_str(p_path) }, static_cast<int64_t>(p_value));
}

bool TomlWriter::set_float(const String& p_path, const float& p_value) {
    return utils::inject_value(*tbl, toml::path{ utils::to_str(p_path) }, static_cast<double>(p_value));
}

bool TomlWriter::set_bool(const String& p_path, const bool& p_value) {
    return utils::inject_value(*tbl, toml::path{ utils::to_str(p_path) }, static_cast<bool>(p_value));
}

// Array

bool TomlWriter::set_array(const String& p_path, const Array& p_array) {
    toml::array array;
    if (!utils::array_to_toml(p_array, array)) {
        return false;
    }

    return utils::inject_value(*tbl, toml::path{ utils::to_str(p_path) }, array);
}

// Table

bool TomlWriter::set_table(const String& p_path, const Dictionary& p_dictionary) {
    toml::table table;
    if (!utils::dictionary_to_toml(p_dictionary, table)) {
        return false;
    }

    return utils::inject_value(*tbl, toml::path{ utils::to_str(p_path) }, table);
}
