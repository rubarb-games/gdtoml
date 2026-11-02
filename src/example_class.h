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

    void print_type(const Variant &p_variant) const;
    void hello() const;
};