#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace frog::native_engine {

// Source-owned declarations for the private frog.document.draft format.
// This is preservation and authoring, not public semantic validation or FIR.
// Copies share immutable bytes, including unknown metadata and exact numbers.
class FrogDraftInterface {
public:
    FrogDraftInterface();
    [[nodiscard]] static std::optional<FrogDraftInterface> read(
        std::string_view json, std::string *error = nullptr);
    // Only the document reader calls this after accepting the private envelope
    // and its legacy byte-string policy. Public readers remain strict UTF-8.
    [[nodiscard]] static FrogDraftInterface retainParsedDraftDeclaration(std::string json,
        std::optional<std::string> legacyRootMapUtf8 = std::nullopt);
    // Historical private root interface_map is retained independently from the
    // declaration. Only accepted private Read can attach these source bytes.
    [[nodiscard]] std::optional<std::string_view> retainedLegacyRootInterfaceMapUtf8() const noexcept;
    [[nodiscard]] std::string_view declarationUtf8() const noexcept;
    [[nodiscard]] std::size_t retainedBytes() const noexcept;
    [[nodiscard]] bool containsPort(std::string_view id) const;

    // An explicit authoring command can declare a port. An existing identity
    // must have the same direction and type; binding never changes its meaning.
    [[nodiscard]] bool declarePort(bool input, std::string_view id,
        std::string_view type, std::string_view connection = {});
    // Explicit private authoring only. Refusal preserves all retained bytes,
    // including the independent historical root-map companion.
    [[nodiscard]] bool editPortForAuthoring(std::string_view id,
        bool expectedInput, std::string_view expectedTypeExpression,
        bool nextInput, std::string_view nextTypeExpression,
        std::optional<std::string_view> nextInputConnection, std::string *diagnostic = nullptr);
    [[nodiscard]] bool setInputConnection(std::string_view id, std::string_view connection);
    [[nodiscard]] bool setMapSettings(std::string_view profile, std::string_view pattern,
        const std::vector<std::string> &transforms);
    // Presentation mapping is written from authored map state at the boundary;
    // it does not reconstruct or remove the independently declared ports.
    [[nodiscard]] std::string withMap(std::string_view mapJson) const;

private:
    explicit FrogDraftInterface(std::string json);
    std::shared_ptr<const std::string> bytes_;
    std::shared_ptr<const std::string> legacyRootMapBytes_;
};

} // namespace frog::native_engine
