#pragma once

#include <cstdint>
#include <filesystem>
#include <map>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace frog::json {

class ParseError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class Value {
public:
    using Array = std::vector<Value>;
    using Object = std::map<std::string, Value>;
    using Storage = std::variant<std::nullptr_t, bool, std::int64_t, double, std::string, Array, Object>;

    Value();
    Value(std::nullptr_t);
    Value(bool value);
    Value(std::int64_t value);
    Value(double value);
    Value(int value);
    Value(unsigned int value);
    Value(std::uint64_t value);
    Value(const char* value);
    Value(std::string value);
    Value(Array value);
    Value(Object value);

    bool is_null() const;
    bool is_bool() const;
    bool is_number() const;
    bool is_floating_number() const;
    bool is_string() const;
    bool is_array() const;
    bool is_object() const;

    bool as_bool() const;
    std::int64_t as_i64() const;
    double as_f64() const;
    const std::string& as_string() const;
    const Array& as_array() const;
    const Object& as_object() const;
    Array& as_array();
    Object& as_object();

    bool contains(const std::string& key) const;
    const Value& at(const std::string& key) const;
    Value& at(const std::string& key);
    const Value* find(const std::string& key) const;

private:
    Storage storage_;
};

using Array = Value::Array;
using Object = Value::Object;

Value parse(const std::string& text);
Value parse_file(const std::filesystem::path& path);
std::string stringify(const Value& value, bool pretty = true, int indent_size = 2);
std::string escape_string(const std::string& input);

} // namespace frog::json
