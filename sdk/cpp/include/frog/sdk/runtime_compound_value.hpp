#pragma once

#include "frog/sdk/runtime_contract.hpp"
#include <cmath>
#include <limits>

namespace frog::sdk::runtime {

// A reference provider is a capability backed by an actual owner registry.
// The SDK never fabricates one from a class name or persists its live handle.
class RuntimeReferenceAuthority {
public:
    virtual ~RuntimeReferenceAuthority() = default;
    virtual std::string_view canonicalType() const noexcept = 0;
    virtual bool validForOwner(const RuntimeValueOwner &) const noexcept = 0;
};

std::optional<RuntimeValue> makeRuntimeRecord(std::vector<RuntimeNamedField>);
std::optional<RuntimeValue> makeRuntimeReference(std::shared_ptr<const RuntimeReferenceAuthority>, const RuntimeValueOwner &);

namespace compound_detail {
class ImmutableRecord;
class BorrowedReference;
inline bool validate(const RuntimeValue &, const ValueType &, const RuntimeValueOwner *,
    unsigned, std::size_t &, std::size_t &) noexcept;

class ImmutableRecord final : public CompoundRuntimeValue {
private:
    friend std::optional<RuntimeValue> frog::sdk::runtime::makeRuntimeRecord(std::vector<RuntimeNamedField>);
    ImmutableRecord(std::string type, std::vector<RuntimeNamedField> fields)
        : type_(std::move(type)), fields_(std::move(fields)) {}
public:
    std::string_view canonicalType() const noexcept override { return type_; }
    bool isReference() const noexcept override { return false; }
    bool valid() const noexcept override {
        std::size_t nodes = 0, bytes = 0;
        for (const auto &field : fields_)
            if (!validate(field.value, field.type, nullptr, 1, nodes, bytes)) return false;
        return nodes < 4096;
    }
    bool validForOwner(const RuntimeValueOwner &owner) const noexcept override {
        if (!runtimeValueOwnerValid(owner)) return false;
        std::size_t nodes = 0, bytes = 0;
        for (const auto &field : fields_)
            if (!validate(field.value, field.type, &owner, 1, nodes, bytes)) return false;
        return nodes < 4096;
    }
    std::span<const RuntimeNamedField> fields() const noexcept override { return fields_; }
private:
    const std::string type_;
    const std::vector<RuntimeNamedField> fields_;
};

class BorrowedReference final : public CompoundRuntimeValue {
private:
    friend std::optional<RuntimeValue> frog::sdk::runtime::makeRuntimeReference(std::shared_ptr<const RuntimeReferenceAuthority>, const RuntimeValueOwner &);
    BorrowedReference(std::string type, std::shared_ptr<const RuntimeReferenceAuthority> authority,
        RuntimeValueOwner owner) : type_(std::move(type)), authority_(std::move(authority)), owner_(std::move(owner)) {}
public:
    std::string_view canonicalType() const noexcept override { return type_; }
    bool isReference() const noexcept override { return true; }
    bool valid() const noexcept override { return validForOwner(owner_); }
    bool validForOwner(const RuntimeValueOwner &owner) const noexcept override {
        return owner == owner_ && authority_ && authority_->canonicalType() == type_ &&
            authority_->validForOwner(owner);
    }
    std::span<const RuntimeNamedField> fields() const noexcept override { return {}; }
private:
    const std::string type_;
    const std::shared_ptr<const RuntimeReferenceAuthority> authority_;
    const RuntimeValueOwner owner_;
};

inline bool validate(const RuntimeValue &value, const ValueType &type, const RuntimeValueOwner *owner,
    unsigned depth, std::size_t &nodes, std::size_t &bytes) noexcept {
    if (depth > 32 || ++nodes > 4096 || std::holds_alternative<std::monostate>(value)) return false;
    try {
        if (type != valueTypeFromCanonicalName(type.canonicalName) || !type.known()) return false;
    } catch (...) { return false; }
    if (const auto *carrier = std::get_if<std::shared_ptr<const CompoundRuntimeValue>>(&value)) {
        if (!*carrier || (*carrier)->canonicalType() != type.canonicalName) return false;
        if (const auto *record = dynamic_cast<const ImmutableRecord *>(carrier->get())) {
            if (type.category != ValueCategory::Cluster) return false;
            for (const auto &field : record->fields())
                if (!validate(field.value, field.type, owner, depth + 1, nodes, bytes)) return false;
            return true;
        }
        const auto *reference = dynamic_cast<const BorrowedReference *>(carrier->get());
        return reference && type.category == ValueCategory::ObjectReference &&
            (owner ? reference->validForOwner(*owner) : reference->valid());
    }
    if (!runtimeValueMatches(value, type)) return false;
    if (const auto *number = std::get_if<std::int64_t>(&value)) {
        if (type.bitWidth == 64) return true;
        if (type.bitWidth != 8 && type.bitWidth != 16 && type.bitWidth != 32) return false;
        const auto limit = std::int64_t{1} << (type.bitWidth - 1);
        return *number >= -limit && *number < limit;
    }
    if (const auto *number = std::get_if<std::uint64_t>(&value)) {
        if (type.bitWidth == 64) return true;
        return (type.bitWidth == 8 || type.bitWidth == 16 || type.bitWidth == 32) &&
            *number < (std::uint64_t{1} << type.bitWidth);
    }
    if (const auto *number = std::get_if<double>(&value))
        return std::isfinite(*number) && (type.bitWidth == 64 ||
            (type.bitWidth == 32 && static_cast<double>(static_cast<float>(*number)) == *number));
    if (const auto *text = std::get_if<std::string>(&value)) {
        if (text->size() > 1'000'000 || text->size() > 16'000'000 - bytes) return false;
        bytes += text->size();
    }
    return true;
}
} // namespace compound_detail

[[nodiscard]] inline bool runtimeValueValidForOwner(const RuntimeValue &value, const ValueType &type,
    const RuntimeValueOwner &owner) noexcept {
    if (!runtimeValueOwnerValid(owner)) return false;
    std::size_t nodes = 0, bytes = 0;
    return compound_detail::validate(value, type, &owner, 0, nodes, bytes);
}
[[nodiscard]] inline bool runtimeValueRepresentable(const RuntimeValue &value, const ValueType &type) noexcept {
    std::size_t nodes = 0, bytes = 0;
    return compound_detail::validate(value, type, nullptr, 0, nodes, bytes);
}

[[nodiscard]] inline std::optional<RuntimeValue> makeRuntimeRecord(std::vector<RuntimeNamedField> fields) {
    if (fields.size() > 64) return {};
    std::unordered_set<std::string> ids;
    std::string type = "cluster<";
    std::size_t nodes = 1, bytes = 0;
    for (const auto &field : fields) {
        if (!structural_detail::identity(field.id) || !ids.insert(field.id).second ||
            !compound_detail::validate(field.value, field.type, nullptr, 1, nodes, bytes)) return {};
        if (type.size() > 8) type += ',';
        type += structural_detail::quoted(field.id) + ":" + field.type.canonicalName;
        if (type.size() >= 65536) return {};
    }
    type += '>';
    // Only these sealed immutable carriers are admitted by the recursive
    // validator. Caller-defined mutable CompoundRuntimeValue implementations
    // cannot enter a body by returning true from a virtual valid() method.
    return RuntimeValue{std::shared_ptr<const CompoundRuntimeValue>{new compound_detail::ImmutableRecord(std::move(type), std::move(fields))}};
}

[[nodiscard]] inline std::optional<RuntimeValue> makeRuntimeReference(
    std::shared_ptr<const RuntimeReferenceAuthority> authority, const RuntimeValueOwner &owner) {
    if (!authority || !runtimeValueOwnerValid(owner) || !authority->validForOwner(owner)) return {};
    const auto type = authority->canonicalType();
    if (!structural_detail::TypeSyntax(type).reference()) return {};
    return RuntimeValue{std::shared_ptr<const CompoundRuntimeValue>{new compound_detail::BorrowedReference(std::string(type), std::move(authority), owner)}};
}

// Content equality is explicit. The scalar variant keeps its legacy identity
// semantics; immutable records round-trip with independent allocations.
[[nodiscard]] inline bool runtimeValuesEqual(const RuntimeValue &left, const RuntimeValue &right,
    unsigned depth = 0) noexcept {
    if (depth > 32 || left.index() != right.index()) return false;
    if (left.index() != 6) return left == right;
    const auto &a = std::get<6>(left), &b = std::get<6>(right);
    if (!a || !b || a->canonicalType() != b->canonicalType() || !a->valid() || !b->valid()) return false;
    if (a->isReference() || b->isReference()) return a == b;
    const auto af = a->fields(), bf = b->fields();
    if (af.size() != bf.size()) return false;
    for (std::size_t index = 0; index < af.size(); ++index)
        if (af[index].id != bf[index].id || af[index].type != bf[index].type ||
            !runtimeValuesEqual(af[index].value, bf[index].value, depth + 1)) return false;
    return true;
}

} // namespace frog::sdk::runtime
