#include "json.hpp"

#include <cmath>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace frog::json {

namespace {

class Parser {
public:
    explicit Parser(const std::string& text) : text_(text) {}

    Value parse_value() {
        skip_ws();
        if (eof()) {
            throw ParseError("Unexpected end of JSON input.");
        }
        const char ch = peek();
        switch (ch) {
        case '{':
            return parse_object();
        case '[':
            return parse_array();
        case '"':
            return Value(parse_string());
        case 't':
            parse_literal("true");
            return Value(true);
        case 'f':
            parse_literal("false");
            return Value(false);
        case 'n':
            parse_literal("null");
            return Value(nullptr);
        default:
            if (ch == '-' || std::isdigit(static_cast<unsigned char>(ch))) {
                return parse_number();
            }
            throw ParseError(std::string("Unexpected character in JSON: ") + ch);
        }
    }

    void ensure_finished() {
        skip_ws();
        if (!eof()) {
            throw ParseError("Unexpected trailing content in JSON input.");
        }
    }

private:
    const std::string& text_;
    std::size_t index_ = 0;

    bool eof() const { return index_ >= text_.size(); }
    char peek() const { return text_[index_]; }
    char get() { return text_[index_++]; }

    void skip_ws() {
        while (!eof() && std::isspace(static_cast<unsigned char>(peek()))) {
            ++index_;
        }
    }

    void expect(char expected) {
        if (eof() || get() != expected) {
            throw ParseError(std::string("Expected JSON character: ") + expected);
        }
    }

    void parse_literal(const char* literal) {
        while (*literal) {
            if (eof() || get() != *literal) {
                throw ParseError("Malformed JSON literal.");
            }
            ++literal;
        }
    }

    std::string parse_string() {
        expect('"');
        std::string out;
        while (!eof()) {
            const char ch = get();
            if (ch == '"') {
                return out;
            }
            if (ch == '\\') {
                if (eof()) {
                    throw ParseError("Malformed JSON escape sequence.");
                }
                const char esc = get();
                switch (esc) {
                case '"': out.push_back('"'); break;
                case '\\': out.push_back('\\'); break;
                case '/': out.push_back('/'); break;
                case 'b': out.push_back('\b'); break;
                case 'f': out.push_back('\f'); break;
                case 'n': out.push_back('\n'); break;
                case 'r': out.push_back('\r'); break;
                case 't': out.push_back('\t'); break;
                case 'u': {
                    if (index_ + 4 > text_.size()) {
                        throw ParseError("Malformed unicode escape in JSON string.");
                    }
                    const std::string hex = text_.substr(index_, 4);
                    index_ += 4;
                    const auto value = static_cast<unsigned int>(std::stoul(hex, nullptr, 16));
                    if (value <= 0x7F) {
                        out.push_back(static_cast<char>(value));
                    } else if (value <= 0x7FF) {
                        out.push_back(static_cast<char>(0xC0 | ((value >> 6) & 0x1F)));
                        out.push_back(static_cast<char>(0x80 | (value & 0x3F)));
                    } else {
                        out.push_back(static_cast<char>(0xE0 | ((value >> 12) & 0x0F)));
                        out.push_back(static_cast<char>(0x80 | ((value >> 6) & 0x3F)));
                        out.push_back(static_cast<char>(0x80 | (value & 0x3F)));
                    }
                    break;
                }
                default:
                    throw ParseError("Unsupported JSON escape sequence.");
                }
            } else {
                out.push_back(ch);
            }
        }
        throw ParseError("Unterminated JSON string.");
    }

    Value parse_number() {
        const std::size_t start = index_;
        if (peek() == '-') {
            ++index_;
        }
        if (eof() || !std::isdigit(static_cast<unsigned char>(peek()))) {
            throw ParseError("Malformed JSON number.");
        }
        while (!eof() && std::isdigit(static_cast<unsigned char>(peek()))) {
            ++index_;
        }
        bool floating = false;
        if (!eof() && peek() == '.') {
            floating = true;
            ++index_;
            if (eof() || !std::isdigit(static_cast<unsigned char>(peek()))) {
                throw ParseError("Malformed JSON number.");
            }
            while (!eof() && std::isdigit(static_cast<unsigned char>(peek()))) {
                ++index_;
            }
        }
        if (!eof() && (peek() == 'e' || peek() == 'E')) {
            floating = true;
            ++index_;
            if (!eof() && (peek() == '+' || peek() == '-')) {
                ++index_;
            }
            if (eof() || !std::isdigit(static_cast<unsigned char>(peek()))) {
                throw ParseError("Malformed JSON number.");
            }
            while (!eof() && std::isdigit(static_cast<unsigned char>(peek()))) {
                ++index_;
            }
        }
        const auto token = text_.substr(start, index_ - start);
        if (floating) {
            return Value(std::stod(token));
        }
        return Value(static_cast<std::int64_t>(std::stoll(token)));
    }

    Value parse_array() {
        expect('[');
        Array values;
        skip_ws();
        if (!eof() && peek() == ']') {
            get();
            return Value(values);
        }
        while (true) {
            values.push_back(parse_value());
            skip_ws();
            if (!eof() && peek() == ',') {
                get();
                continue;
            }
            if (!eof() && peek() == ']') {
                get();
                return Value(values);
            }
            throw ParseError("Malformed JSON array.");
        }
    }

    Value parse_object() {
        expect('{');
        Object object;
        skip_ws();
        if (!eof() && peek() == '}') {
            get();
            return Value(object);
        }
        while (true) {
            skip_ws();
            if (eof() || peek() != '"') {
                throw ParseError("Expected a JSON object key.");
            }
            const std::string key = parse_string();
            skip_ws();
            expect(':');
            object.emplace(key, parse_value());
            skip_ws();
            if (!eof() && peek() == ',') {
                get();
                continue;
            }
            if (!eof() && peek() == '}') {
                get();
                return Value(object);
            }
            throw ParseError("Malformed JSON object.");
        }
    }
};

void append_string(std::ostringstream& out, const Value& value, bool pretty, int indent_size, int level);
void append_array(std::ostringstream& out, const Array& array, bool pretty, int indent_size, int level);
void append_object(std::ostringstream& out, const Object& object, bool pretty, int indent_size, int level);

std::string indent(int level, int indent_size) {
    return std::string(static_cast<std::size_t>(level * indent_size), ' ');
}

std::string format_double(double value) {
    std::ostringstream number;
    number << std::setprecision(15) << value;
    std::string text = number.str();
    if (text.find('.') == std::string::npos && text.find('e') == std::string::npos && text.find('E') == std::string::npos) {
        text += ".0";
    }
    return text;
}

void append_string(std::ostringstream& out, const Value& value, bool pretty, int indent_size, int level) {
    (void)pretty;
    (void)indent_size;
    (void)level;
    if (value.is_null()) {
        out << "null";
    } else if (value.is_bool()) {
        out << (value.as_bool() ? "true" : "false");
    } else if (value.is_number()) {
        out << (value.is_floating_number() ? format_double(value.as_f64()) : std::to_string(value.as_i64()));
    } else if (value.is_string()) {
        out << '"' << escape_string(value.as_string()) << '"';
    } else if (value.is_array()) {
        append_array(out, value.as_array(), pretty, indent_size, level);
    } else {
        append_object(out, value.as_object(), pretty, indent_size, level);
    }
}

void append_array(std::ostringstream& out, const Array& array, bool pretty, int indent_size, int level) {
    out << '[';
    if (array.empty()) {
        out << ']';
        return;
    }
    if (pretty) {
        out << '\n';
    }
    for (std::size_t index = 0; index < array.size(); ++index) {
        if (pretty) {
            out << indent(level + 1, indent_size);
        }
        append_string(out, array[index], pretty, indent_size, level + 1);
        if (index + 1 < array.size()) {
            out << ',';
        }
        if (pretty) {
            out << '\n';
        }
    }
    if (pretty) {
        out << indent(level, indent_size);
    }
    out << ']';
}

void append_object(std::ostringstream& out, const Object& object, bool pretty, int indent_size, int level) {
    out << '{';
    if (object.empty()) {
        out << '}';
        return;
    }
    if (pretty) {
        out << '\n';
    }
    std::size_t index = 0;
    for (const auto& entry : object) {
        if (pretty) {
            out << indent(level + 1, indent_size);
        }
        out << '"' << escape_string(entry.first) << '"' << (pretty ? ": " : ":");
        append_string(out, entry.second, pretty, indent_size, level + 1);
        if (++index < object.size()) {
            out << ',';
        }
        if (pretty) {
            out << '\n';
        }
    }
    if (pretty) {
        out << indent(level, indent_size);
    }
    out << '}';
}

} // namespace

Value::Value() : storage_(nullptr) {}
Value::Value(std::nullptr_t) : storage_(nullptr) {}
Value::Value(bool value) : storage_(value) {}
Value::Value(std::int64_t value) : storage_(value) {}
Value::Value(double value) : storage_(value) {}
Value::Value(int value) : storage_(static_cast<std::int64_t>(value)) {}
Value::Value(unsigned int value) : storage_(static_cast<std::int64_t>(value)) {}
Value::Value(std::uint64_t value) : storage_(static_cast<std::int64_t>(value)) {}
Value::Value(const char* value) : storage_(std::string(value)) {}
Value::Value(std::string value) : storage_(std::move(value)) {}
Value::Value(Array value) : storage_(std::move(value)) {}
Value::Value(Object value) : storage_(std::move(value)) {}

bool Value::is_null() const { return std::holds_alternative<std::nullptr_t>(storage_); }
bool Value::is_bool() const { return std::holds_alternative<bool>(storage_); }
bool Value::is_number() const { return std::holds_alternative<std::int64_t>(storage_) || std::holds_alternative<double>(storage_); }
bool Value::is_floating_number() const { return std::holds_alternative<double>(storage_); }
bool Value::is_string() const { return std::holds_alternative<std::string>(storage_); }
bool Value::is_array() const { return std::holds_alternative<Array>(storage_); }
bool Value::is_object() const { return std::holds_alternative<Object>(storage_); }

bool Value::as_bool() const { return std::get<bool>(storage_); }
std::int64_t Value::as_i64() const { return std::holds_alternative<double>(storage_) ? static_cast<std::int64_t>(std::get<double>(storage_)) : std::get<std::int64_t>(storage_); }
double Value::as_f64() const { return std::holds_alternative<double>(storage_) ? std::get<double>(storage_) : static_cast<double>(std::get<std::int64_t>(storage_)); }
const std::string& Value::as_string() const { return std::get<std::string>(storage_); }
const Array& Value::as_array() const { return std::get<Array>(storage_); }
const Object& Value::as_object() const { return std::get<Object>(storage_); }
Array& Value::as_array() { return std::get<Array>(storage_); }
Object& Value::as_object() { return std::get<Object>(storage_); }

bool Value::contains(const std::string& key) const {
    if (!is_object()) {
        return false;
    }
    return as_object().find(key) != as_object().end();
}

const Value& Value::at(const std::string& key) const {
    return as_object().at(key);
}

Value& Value::at(const std::string& key) {
    return as_object().at(key);
}

const Value* Value::find(const std::string& key) const {
    if (!is_object()) {
        return nullptr;
    }
    const auto it = as_object().find(key);
    if (it == as_object().end()) {
        return nullptr;
    }
    return &it->second;
}

Value parse(const std::string& text) {
    Parser parser(text);
    Value value = parser.parse_value();
    parser.ensure_finished();
    return value;
}

Value parse_file(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        throw std::runtime_error("Unable to open JSON file: " + path.string());
    }
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return parse(buffer.str());
}

std::string stringify(const Value& value, bool pretty, int indent_size) {
    std::ostringstream out;
    append_string(out, value, pretty, indent_size, 0);
    return out.str();
}

std::string escape_string(const std::string& input) {
    std::ostringstream out;
    for (const unsigned char ch : input) {
        switch (ch) {
        case '\\': out << "\\\\"; break;
        case '"': out << "\\\""; break;
        case '\b': out << "\\b"; break;
        case '\f': out << "\\f"; break;
        case '\n': out << "\\n"; break;
        case '\r': out << "\\r"; break;
        case '\t': out << "\\t"; break;
        default:
            if (ch < 0x20) {
                out << "\\u";
                const char* digits = "0123456789ABCDEF";
                out << '0' << '0' << digits[(ch >> 4) & 0x0F] << digits[ch & 0x0F];
            } else {
                out << static_cast<char>(ch);
            }
        }
    }
    return out.str();
}

} // namespace frog::json
