#include <cassert>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>

#include "contract.hpp"
#include "execute.hpp"
#include "json.hpp"

namespace {

std::filesystem::path repo_root() {
    return frog::runtime::find_repo_root(std::filesystem::path(FROG_RUNTIME_CPP_SOURCE_DIR));
}

frog::json::Value load_json(const std::filesystem::path& path) {
    return frog::json::parse_file(path);
}

std::filesystem::path resolve_repo_path(const std::string& relative_path) {
    return repo_root() / relative_path;
}

std::string canonical_json(const frog::json::Value& value) {
    return frog::json::stringify(value, true, 2);
}

void check_acceptance_file(const std::string& relative_path) {
    const auto acceptance = load_json(resolve_repo_path(relative_path));
    const auto& refs = acceptance.as_object().at("artifact_refs").as_object();
    const auto contract = load_json(resolve_repo_path(refs.at("contract_path").as_string()));
    const auto expected = load_json(resolve_repo_path(refs.at("snapshot_path").as_string()));

    std::optional<frog::json::Value> wfrog;
    if (const auto wfrog_path = refs.find("wfrog_path"); wfrog_path != refs.end() && wfrog_path->second.is_string()) {
        wfrog = load_json(resolve_repo_path(wfrog_path->second.as_string()));
    }

    const auto actual = frog::runtime::execute_reference_acceptance(
        acceptance,
        contract,
        wfrog.has_value() ? &*wfrog : nullptr);
    assert(canonical_json(actual) == canonical_json(expected));
}

void check_example06_cases() {
    const auto acceptance = load_json(resolve_repo_path("Implementations/Reference/Runtime/acceptance/example06_boolean_value_roundtrip.acceptance.json"));
    const auto& refs = acceptance.as_object().at("artifact_refs").as_object();
    const auto contract = load_json(resolve_repo_path(refs.at("contract_path").as_string()));
    const auto wfrog = load_json(resolve_repo_path(refs.at("wfrog_path").as_string()));
    for (const auto& item : acceptance.as_object().at("cases").as_array()) {
        const auto& current = item.as_object();
        const auto expected = load_json(resolve_repo_path(current.at("snapshot_path").as_string()));
        const auto actual = frog::runtime::execute_reference_contract_case(
            contract,
            frog::json::Value(frog::json::Object({{"input_value", current.at("input_value")}})),
            &wfrog);
        assert(canonical_json(actual) == canonical_json(expected));
    }
}

} // namespace

int main() {
    check_acceptance_file("Implementations/Reference/Runtime/acceptance/example01_pure_addition.acceptance.json");
    check_acceptance_file("Implementations/Reference/Runtime/acceptance/example02_ui_value_roundtrip.acceptance.json");
    check_acceptance_file("Implementations/Reference/Runtime/acceptance/example03_ui_property_write.acceptance.json");
    check_acceptance_file("Implementations/Reference/Runtime/acceptance/example04_stateful_feedback_delay.acceptance.json");
    check_acceptance_file("Implementations/Reference/Runtime/acceptance/example05_runtime_family.acceptance.json");
    check_acceptance_file("Implementations/Reference/Runtime/acceptance/example06_boolean_value_roundtrip.acceptance.json");
    check_example06_cases();
    std::cout << "reference examples runtime acceptance passed" << std::endl;
    return 0;
}
