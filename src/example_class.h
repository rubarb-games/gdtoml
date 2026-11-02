#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "toml++/toml.hpp"
#include <memory>

using namespace godot;

class ExampleClass : public RefCounted {
    GDCLASS(ExampleClass, RefCounted)

private:
    std::unique_ptr<toml::table> tbl;

protected:
    static void _bind_methods();

public:
    ExampleClass();
    ~ExampleClass() override = default;

    bool try_parse(const String &p_content);
    String get_string(const String &p_key);
    // --
    void print_type(const Variant &p_variant) const;
    void hello() const;
};