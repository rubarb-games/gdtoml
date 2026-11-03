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