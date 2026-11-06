#pragma once

#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "fmt/printf.h"
#include "toml++/toml.hpp"

#include <cstdint>
#include <string_view>
#include <memory>
#include <optional>
#include <functional>

using namespace godot;

namespace utils {

// forward declarations
static bool append_variant(toml::array& output, const Variant& value);
static bool insert_variant(toml::table& output, const std::string& key, const Variant& value);

inline std::string to_str(const String& p_string) {
    auto utf8 = p_string.utf8();
    return std::string(utf8.get_data(), utf8.length());
}

inline String to_string(const std::optional<std::string>& p_optional) {
    if (!p_optional) {
        return {};
    }
    return {p_optional.value().c_str()};
}

inline String to_string(const std::string& p_string) {
    return {p_string.c_str()};
}

inline String to_string(const std::optional<std::string_view>& p_optional) {
    if (!p_optional) {
        return {};
    }
    std::string s(p_optional.value());
    return String::utf8(s.c_str());
}

inline String to_string(const std::string_view& p_string_view) {
    std::string s(p_string_view);
    return String::utf8(s.c_str());
}

// Creates a set of nested tables, starting at the provided root node.
static std::optional<std::reference_wrapper<toml::table>> create_path(
    toml::table& root, 
    const toml::path& path
    ) {
    
    if (!path || path.empty()) {
        return std::nullopt;
    }

    toml::table* current = &root;

    // Traverse all components except the leaf.
    for (size_t i = 0; i < path.size() - 1; ++i) {
        const auto& component = path[i];

        // A toml::path supports array indices, like [2] to get a specific
        // element from an array. Can't see a reason for supporting array
        // indices when creating a node hierarchy. This is for creating
        // a set of nested tables.
        if (component.type() != toml::path_component_type::key) {
            return std::nullopt;
        }

        const auto& key = component.key();
        auto it = current->find(key);

        // node->end() is an iterator pointing to the position after
        // the last key-value pair of the table.
        // When find() could not find a key, it will point to the position 
        // after the last key-value pair. 
        if (it == current->end()) {
            auto result = current->insert(key, toml::table{});
            // result.first is an iterator. It points to the position
            // where the newly created table was inserted
            it = result.first;
        }

        // at this point, it points to the key-value pair either
        // found by find() or created by insert(). it->first is the
        // key, and it->second is the value.
        current = it->second.as_table();

        if (!current) {
            // std::cerr << "Path component is not a table: "
            //           << key << '\n';
            return std::nullopt;
        }
    }

    return std::ref(*current);
}

// TODO: return an error code, to enable more informative logging on the calling side
template <typename T>
static bool inject_value(toml::table& doc, const toml::path& path, T&& value) {
    if (!path || path.empty()) {
        return false;
    }

    // utils::create_path can only create the path up to, but not
    // including, the leaf node. The leaf node might be an array 
    // index, which is not supported in this case.
    const auto& leaf = path[path.size() - 1];
    if (leaf.type() != toml::path_component_type::key) {
        return false;
    }

    // parent is an std::optional
    auto parent = utils::create_path(doc, path);
    if (!parent) {
        return false;
    }

    auto& table = parent->get();
    const auto& key = leaf.key();

    if (table.contains(key)) {
        return false;
    }

    // Emplace a new value at key, if it doesn't already exist
    auto result = table.emplace(key, value);
    // result is an iterator. result.first points to the position where
    // the value was inserted. result.second is a boolean that indicates
    // whether the emplacement was successful or not.
    return result.second;
}

static bool array_to_toml(const Array& array, toml::array& toml_array) {
    for (int64_t i = 0; i < array.size(); ++i) {
        // Halt on values that cannot be turned into toml nodes.
        // gdtoml will not make assumptions about your data types.
        if (!append_variant(toml_array, array[i])) {
            // UtilityFunctions::print(to_string(fmt::sprintf("cannot append variant at index {}", i)));
            // print error
            return false;
        }
    }

    return true;
}

static bool dictionary_to_toml(const Dictionary& dictionary, toml::table& toml_table) {
    Array keys = dictionary.keys();
    
    for (int64_t i = 0; i < keys.size(); ++i) {
        Variant key = keys[i];
        Variant value = dictionary[key];
        
        bool is_string_key = key.get_type() == Variant::STRING || key.get_type() == Variant::STRING_NAME;
        if (!is_string_key) {
            // TOML does not support non-string keys
            // Skip the key and continue to the next
            continue;
        }

        std::string key_str = to_str(key);
        switch (value.get_type()) {
            case Variant::BOOL:
                toml_table.insert(key_str, static_cast<bool>(value));

            case Variant::INT:
                toml_table.insert(key_str, static_cast<int64_t>(value));

            case Variant::FLOAT:
                toml_table.insert(key_str, static_cast<double>(value));

            case Variant::STRING:
            case Variant::STRING_NAME: {
                toml_table.insert(key_str, to_str(value));
            }

            case Variant::DICTIONARY: {
                toml::table nested;

                if (dictionary_to_toml(value, nested)) {
                    toml_table.insert(key_str, std::move(nested));
                }
            }

            case Variant::ARRAY: {
                Array nested_array = value;
                toml::array nested;
                
                for (int64_t i = 0; i < nested_array.size(); ++i) {
                    if (append_variant(nested, nested_array[i])) {
                        toml_table.insert(key_str, std::move(nested));
                    }
                }
            }

            default:
                continue;
        }
    }

    return true;
}

static bool append_variant(toml::array& output, const Variant& value) {
    switch (value.get_type()) {
        case Variant::BOOL:
            output.push_back(static_cast<bool>(value));
            return true;

        case Variant::INT:
            output.push_back(static_cast<int64_t>(value));
            return true;

        case Variant::FLOAT:
            output.push_back(static_cast<double>(value));
            return true;

        case Variant::STRING:
        case Variant::STRING_NAME:
        {
            output.push_back(to_str(value));
            return true;
        }

        case Variant::DICTIONARY:
        {
            toml::table nested;

            if (!dictionary_to_toml(value, nested))
                return false;

            output.push_back(std::move(nested));
            return true;
        }

        case Variant::ARRAY:
        {
            Array nested_array = value;
            toml::array nested;

            for (int64_t i = 0; i < nested_array.size(); ++i)
            {
                if (!append_variant(nested, nested_array[i]))
                    return false;
            }

            output.push_back(std::move(nested));
            return true;
        }

        default:
            return false;
    }
}
// Forward declarations
static Variant to_variant(const toml::node& node);

static Array to_array(const toml::array& toml_array) {
    Array result = {};

    for (auto&& element : toml_array) {
        result.append(to_variant(element));
    }

    return result;
}

static Dictionary to_dictionary(const toml::table& table) {
    Dictionary dict;

    for (auto&& [key, value] : table) {
        dict[utils::to_string(key.str())] = to_variant(value);
    }

    return dict;
}

static Variant to_variant(const toml::node& node) {
    switch (node.type()) {
        case toml::node_type::table:            { return to_dictionary(*node.as_table()); }
        case toml::node_type::array:            { return to_array(*node.as_array()); }
        case toml::node_type::string:           { return utils::to_string(*node.value<std::string>()); }
        case toml::node_type::integer:          { return node.value<int64_t>().value(); }
        case toml::node_type::floating_point:   { return node.value<double>().value(); }
        case toml::node_type::boolean:          { return node.value<bool>().value(); }
        case toml::node_type::date: {
            std::ostringstream stream;
            stream << node.value<toml::date>().value();
            return utils::to_string(stream.str());
        }
        case toml::node_type::time: {
            std::ostringstream stream;
            stream << node.value<toml::time>().value();
            return utils::to_string(stream.str());
        }
        case toml::node_type::date_time: {
            std::ostringstream stream;
            stream << node.value<toml::date_time>().value();
            return utils::to_string(stream.str());
        }
        default: {
            return {};
        }
    }
}

}