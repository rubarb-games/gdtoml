#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "toml++/toml.hpp"
#include <memory>

using namespace godot;

class TomlReader : public RefCounted {
    GDCLASS(TomlReader, RefCounted)

private:
    std::unique_ptr<toml::table> tbl;

    bool log_enabled;
    void log(const std::string& p_msg) const;

    static std::string to_str(const String& p_string);
    static String to_string(const std::optional<std::string>& p_optional);
    static String to_string(const std::string& p_string);
    static String to_string(const std::optional<std::string_view>& p_optional);
    static String to_string(const std::string_view& p_string_view);

    static Variant to_variant(const toml::node& node);
    static Array to_array(const toml::array& toml_array);
    static Dictionary to_dictionary(const toml::table& table);

protected:
    static void _bind_methods();

public:
    TomlReader();
    ~TomlReader() override = default;

    bool try_parse(const String& p_content);
    void enable_log(bool val);

    bool exists(const String& p_key) const;

    String get_string(const String& p_key) const;
    String get_string_or(const String& p_key, const String& p_default) const;
    TypedArray<String> get_string_arr(const String& p_key) const;

    int64_t get_int(const String& p_key) const;
    int64_t get_int_or(const String& p_key, const int64_t& p_default) const;
    TypedArray<int64_t> get_int_arr(const String& p_key) const;

    float get_float(const String& p_key) const;
    float get_float_or(const String& p_key, const float& p_default) const;
    TypedArray<float> get_float_arr(const String& p_key) const;

    bool get_bool(const String& p_key) const;
    bool get_bool_or(const String& p_key, const bool& p_default) const;
    TypedArray<bool> get_bool_arr(const String& p_key) const;

    Array get_array(const String& p_key) const;

    Dictionary get_table(const String& p_key) const;
    TypedArray<String> get_table_keys(const String& p_key) const;
};