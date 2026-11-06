#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/variant.hpp"

#define TOML_EXCEPTIONS 0
#include "toml++/toml.hpp"

#include <memory>

using namespace godot;

class TomlWriter : public RefCounted {
    GDCLASS(TomlWriter, RefCounted)

private:
    std::unique_ptr<toml::table> tbl;
    bool log_enabled;

    template<typename... Args>
    void log(fmt::format_string<Args...> format, Args&&... args) const {
        if (!log_enabled) { return; }
        std::string msg = fmt::format(format, std::forward<Args>(args)...);
        std::string s = fmt::format("[gdtoml.toml_writer] {}", msg);
        UtilityFunctions::print(utils::to_string(s));
    }

protected:
    static void _bind_methods();

public:
    TomlWriter();
    ~TomlWriter() override = default;

    void enable_log(bool val);
    String serialize(bool prune);

    bool set_string(const String& p_path, const String& p_value);
    bool set_int(const String& p_path, const int& p_value);
    bool set_float(const String& p_path, const float& p_value);
    bool set_bool(const String& p_path, const bool& p_value);

    bool set_array(const String& p_path, const Array& p_array);
    bool set_table(const String& p_path, const Dictionary& p_table);
};
