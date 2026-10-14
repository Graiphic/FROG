#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

namespace frog::sdk::runtime::structural_detail {

inline bool identity(std::string_view value) noexcept {
    if (value.empty() || value.size() > 1024) return false;
    for (const unsigned char ch : value) if (ch < 32) return false;
    return true;
}
inline std::string quoted(std::string_view value) {
    std::string result = "\"";
    for (const char ch : value) {
        if (ch == '\\' || ch == '"') result += '\\';
        result += ch;
    }
    return result + '"';
}

// Canonical syntax is a structural contract. Classes and borrowed handles are
// still checked by their actual owner registry, never by this parser.
class TypeSyntax final {
public:
    explicit TypeSyntax(std::string_view value) : value_(value) {}
    bool reference() noexcept {
        try { return reset() && take("ref<") && ref(0) && position_ == value_.size(); }
        catch (...) { return false; }
    }
    bool record() noexcept {
        try { return reset() && take("cluster<") && cluster(0, true) && position_ == value_.size(); }
        catch (...) { return false; }
    }
    bool transport() noexcept {
        try { return reset() && type(0, true) && position_ == value_.size(); }
        catch (...) { return false; }
    }
    std::optional<std::vector<std::pair<std::string, std::string>>> recordFields() {
        fields_.clear();
        if (!record()) return {};
        return fields_;
    }
private:
    bool reset() noexcept {
        position_ = nodes_ = 0;
        return !value_.empty() && value_.size() <= 65536;
    }
    bool take(std::string_view token) noexcept {
        if (!value_.substr(position_).starts_with(token)) return false;
        position_ += token.size();
        return true;
    }
    std::optional<std::string> name() {
        if (!take("\"")) return {};
        std::string result;
        while (position_ < value_.size()) {
            auto ch = value_[position_++];
            if (ch == '"') return identity(result) ? std::optional{result} : std::nullopt;
            if (static_cast<unsigned char>(ch) < 32 || result.size() >= 1024) return {};
            if (ch == '\\') {
                if (position_ >= value_.size() || (value_[position_] != '\\' && value_[position_] != '"')) return {};
                ch = value_[position_++];
            }
            result += ch;
        }
        return {};
    }
    bool positive() noexcept {
        if (position_ >= value_.size() || value_[position_] < '1' || value_[position_] > '9') return false;
        unsigned n = 0;
        while (position_ < value_.size() && value_[position_] >= '0' && value_[position_] <= '9') {
            if (n > 1'000'000) return false;
            n = n * 10 + static_cast<unsigned>(value_[position_++] - '0');
        }
        return n <= 1'000'000;
    }
    bool ref(unsigned depth) {
        return name().has_value() && (take(">") || (take(",") && type(depth + 1, false) && take(">")));
    }
    bool cluster(unsigned depth, bool root, bool transportOnly = false) {
        std::unordered_set<std::string> ids;
        if (take(">")) return true;
        do {
            auto id = name();
            if (!id || ids.size() >= 64 || !ids.insert(*id).second || !take(":")) return false;
            const auto begin = position_;
            if (!type(depth + 1, transportOnly)) return false;
            if (root) fields_.emplace_back(std::move(*id), value_.substr(begin, position_ - begin));
        } while (take(","));
        return take(">");
    }
    bool type(unsigned depth, bool transportOnly) {
        if (depth > 32 || ++nodes_ > 4096) return false;
        if (take("ref<")) return ref(depth);
        if (take("cluster<")) return cluster(depth, false, transportOnly);
        if (!transportOnly && take("array<")) {
            if (!type(depth + 1, false)) return false;
            if (take(",")) { static_cast<void>(take("rank=")); if (!positive()) return false; }
            return take(">");
        }
        if (!transportOnly && take("enum<")) return name().has_value() && take(",") && type(depth + 1, false) && take(">");
        const auto begin = position_;
        while (position_ < value_.size() && value_[position_] != ',' && value_[position_] != '>') ++position_;
        const auto scalar = value_.substr(begin, position_ - begin);
        for (const auto name : {"bool", "i8", "i16", "i32", "i64", "u8", "u16", "u32", "u64", "f32", "f64", "string", "path"})
            if (scalar == name) return true;
        if (!transportOnly) for (const auto name : {"c32", "c64", "fxp", "cfx"}) if (scalar == name) return true;
        return false;
    }
    std::string_view value_;
    std::size_t position_ = 0, nodes_ = 0;
    std::vector<std::pair<std::string, std::string>> fields_;
};

} // namespace frog::sdk::runtime::structural_detail
