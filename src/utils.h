#pragma once

#include "godot_cpp/variant/utility_functions.hpp"
#include <cstdint>
#include <string_view>
#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "toml++/toml.hpp"
#include <memory>

using namespace godot;

namespace utils {

inline std::string to_str(const String& p_string) {
    return p_string.utf8().get_data();
}

inline String to_string(const std::optional<std::string>& p_optional) {
    return to_string(p_optional.value());
}

inline String to_string(const std::string& p_string) {
    return {p_string.c_str()};
}

inline String to_string(const std::optional<std::string_view>& p_optional) {
    return to_string(p_optional.value());
}

inline String to_string(const std::string_view& p_string_view) {
    return String::utf8(p_string_view.data(), static_cast<int64_t>(p_string_view.size()));
}

}