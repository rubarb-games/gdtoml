#include "fmt/printf.h"

#include "toml_writer.h"
#include "godot_cpp/variant/utility_functions.hpp"
#include "toml++/impl/forward_declarations.hpp"
#include "toml++/impl/node_view.hpp"
#include <cstdint>
#include <string_view>

#include "utils.h"

void TomlWriter::_bind_methods() {
    // godot::ClassDB::bind_method(D_METHOD("try_parse", "string"), &TomlReader::try_parse);
}

TomlWriter::TomlWriter() {
    tbl = std::make_unique<toml::table>();
}

void TomlWriter::enable_log(const bool val) {
    log_enabled = val;
}

void TomlWriter::log(const std::string& p_msg) const {
    if (!log_enabled) {
        return;
    }
    std::string s = static_cast<std::string>(fmt::sprintf("[gdtoml.toml_reader] {}", p_msg));
    UtilityFunctions::print(utils::to_string(s));
}

String TomlWriter::serialize() {}

bool TomlWriter::set_string(const String& p_key, const String& p_value) {}

bool TomlWriter::set_int(const String& p_key, const int& p_value) {}

bool TomlWriter::set_float(const String& p_key, const float& p_value) {}

bool TomlWriter::set_bool(const String& p_key, const bool& p_value) {}

bool TomlWriter::set_array(const String& p_key, const Array& p_array) {}

bool TomlWriter::set_table(const String& p_key, const Dictionary& p_table) {}

bool TomlWriter::append_to_array(const String& p_key, const Variant& p_value) {}

bool TomlWriter::erase_from_array(const String& p_key, const Variant& p_value) {}

bool TomlWriter::remove(const String& p_key) {}

void TomlWriter::prune() {}