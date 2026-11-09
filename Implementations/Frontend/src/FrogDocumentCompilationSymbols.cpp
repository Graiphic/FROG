#include "FrogDocumentConditionalDisable.hpp"
#include "FrogDocumentStructureSerializationHelpers.hpp"
#include "frog_native_engine/ConditionalDisable.hpp"
#include <charconv>
#include <sstream>

namespace frog::native_engine::document_internal {
std::string readCompilationSymbols(const JsonValue &root, FrogSourceProperties &properties) {
    const auto *configuration = objectField(root, "conditional_compilation");
    if (!configuration) return {};
    const auto *version=objectField(*configuration,"version");
    bool versionOne=version&&version->type==JsonValue::Type::Number;
    if(versionOne) {
        // Canonical adapters preserve numeric lexemes; their convenience
        // double projection is deliberately NaN. Read this schema number
        // fully, with the same locale-independent JSON numeric contract.
        double number=version->number;
        if(!version->string.empty()) {
            const auto parsed=std::from_chars(version->string.data(),version->string.data()+version->string.size(),number);
            versionOne=parsed.ec==std::errc{}&&parsed.ptr==version->string.data()+version->string.size();
        }
        versionOne=versionOne&&number==1.0;
    }
    if (configuration->type != JsonValue::Type::Object || !versionOne)
        return "conditional_compilation requires version 1.";
    for (const auto &[key, value] : configuration->object) {
        if (key == "version") continue;
        if ((key != "project_symbols" && key != "target_symbols") || value.type != JsonValue::Type::Object)
            return "Unknown or invalid conditional_compilation field: " + key;
        auto &symbols = key == "project_symbols" ? properties.compilationProjectSymbols : properties.compilationTargetSymbols;
        for (const auto &[name, symbol] : value.object) {
            if (name.empty() || isConditionalDisableBuiltInSymbol(name) || symbol.type != JsonValue::Type::String)
                return "Custom compilation symbols require non-reserved names and string values: " + name;
            symbols.emplace(name, symbol.string);
        }
    }
    return {};
}
std::string serializeCompilationSymbols(const FrogSourceProperties &properties) {
    if (properties.compilationProjectSymbols.empty() && properties.compilationTargetSymbols.empty()) return {};
    std::ostringstream output;
    output << "  \"conditional_compilation\": {\n    \"version\": 1,\n";
    const auto append = [&](std::string_view key, const auto &symbols) {
        output << "    \"" << key << "\": {";
        bool comma = false;
        for (const auto &[name, value] : symbols) {
            if (comma) output << ',';
            output << '\n' << "      "; appendJsonString(output, name);
            output << ": "; appendJsonString(output, value); comma = true;
        }
        output << "\n    }";
    };
    append("project_symbols", properties.compilationProjectSymbols); output << ",\n";
    append("target_symbols", properties.compilationTargetSymbols); output << "\n  },\n";
    return output.str();
}
}
