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

class TomlWriter : public RefCounted {
    GDCLASS(TomlWriter, RefCounted)

private:
    std::unique_ptr<toml::table> tbl;

    bool log_enabled;
    void log(const std::string& p_msg) const;

protected:
    static void _bind_methods();

public:
    TomlWriter();
    ~TomlWriter() override = default;

    void enable_log(bool val);
    String serialize();

    bool set_string(const String& p_key, const String& p_value);
    // int, float and bool
    // return false if cannot set

    bool set_array(const String& p_key, const Array& p_array);
    bool set_table(const String& p_key, const Dictionary& p_table);

    bool append_to_array(const String& p_key, const Variant& p_value);
    bool erase_from_array(const String& p_key, const Variant& p_value);
    
    bool remove(const String& p_key);
};
