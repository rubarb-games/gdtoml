#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/variant.hpp"

#include <memory>
#include <cstdint>
#include <utility>

#include "fmt/format.h"

#define TOML_EXCEPTIONS 0
#include "toml++/toml.hpp"

#include "utils.h"

using namespace godot;

class TomlReader : public RefCounted {
    GDCLASS(TomlReader, RefCounted)

private:
    std::unique_ptr<toml::table> doc;

    bool log_enabled;
    bool doc_ok() const;
    bool _exists(const std::string& p_path) const;

    template<typename... Args>
    void log(fmt::format_string<Args...> format, Args&&... args) const {
        std::string msg = fmt::format(format, std::forward<Args>(args)...);
        std::string s = fmt::format("[gdtoml.toml_reader] {}", msg);
        UtilityFunctions::print(utils::to_string(s));
    }

    template<typename T>
    std::optional<T> get_value(const std::string& path, const T& default_val) const {
        toml::node_view<toml::node> node = doc->at_path(path);

        if (node.type() == toml::node_type::none) {
            log("Path '{}' was not found", path);
            return default_val;
        }

        std::optional<T> val = node.value<T>();
        if (!val.has_value()) {
            std::string value_type = "[unknown type]";

            if constexpr (std::is_same_v<T, std::string>) {
                value_type = "std::string";
            }
            else if constexpr (std::is_same_v<T, int>) {
                value_type = "int";
            }
            else if constexpr (std::is_same_v<T, double>) {
                value_type = "double";
            }
            else if constexpr (std::is_same_v<T, bool>) {
                value_type = "bool";
            }

            log("Value at path '{}' is not a {}", path, value_type);
            val = default_val;
        }

        return val;
    }

protected:
    static void _bind_methods();

public:
    TomlReader();
    ~TomlReader() override = default;

    bool try_parse(const String& p_content);
    void enable_log(bool val);

    bool exists(const String& p_path) const;

    String get_string(const String& p_path, const String& p_default = "") const;
    TypedArray<String> get_string_arr(const String& p_path) const;

    int64_t get_int(const String& p_path, const int64_t& p_default = 0) const;
    TypedArray<int64_t> get_int_arr(const String& p_path) const;

    float get_float(const String& p_path, const float& p_default = 0.0) const;
    TypedArray<float> get_float_arr(const String& p_path) const;

    bool get_bool(const String& p_path, const bool& p_default = false) const;
    TypedArray<bool> get_bool_arr(const String& p_path) const;

    Array get_array(const String& p_path) const;

    Dictionary get_table(const String& p_path) const;
    TypedArray<String> get_table_keys(const String& p_path) const;
};
