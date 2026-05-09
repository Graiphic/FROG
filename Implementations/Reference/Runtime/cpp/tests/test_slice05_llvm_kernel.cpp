#include <cassert>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

#include "json.hpp"
#include "runtime.hpp"

extern "C" void frog_example05_run(std::uint16_t input_value, frog::runtime::FrogRunResult* out_result);

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

std::filesystem::path native_manifest_path() {
    return repo_root() / "Implementations" / "Reference" / "LLVM" / "examples" /
           "05_bounded_ui_accumulator" / "native_kernel_manifest.json";
}

std::string canonical_json(const frog::json::Value& value) {
    return frog::json::stringify(value, true, 2);
}

const frog::json::Object& acceptance_root() {
    static const frog::json::Value acceptance = load_json(
        repo_root() / "Implementations" / "Reference" / "Runtime" / "acceptance" / "example05_runtime_family.acceptance.json");
    return acceptance.as_object();
}

void test_direct_llvm_kernel_bridge_call() {
    const auto& root = acceptance_root();
    const auto& overflow = root.at("overflow").as_object();

    const auto bridge = frog::runtime::make_linked_native_kernel_bridge(native_manifest_path(), &frog_example05_run);
    assert(bridge.manifest().entry_symbol == "frog_example05_run");
    assert(bridge.manifest().abi == "frog_u16_to_result_status_outptr");
    assert(bridge.manifest().source_artifact == "Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/kernel.ll");

    const auto ok = bridge.run(3);
    assert(ok.ok);
    assert(ok.result == 15);
    assert(ok.error_code == 0);

    const auto rejected = bridge.run(static_cast<std::uint16_t>(overflow.at("input_value").as_i64()));
    assert(!rejected.ok);
    assert(rejected.error_code == 1);
    assert(rejected.diagnostic == overflow.at("expected_error").as_string());
}

void test_runtime_uses_llvm_kernel_bridge() {
    const auto& root = acceptance_root();
    const auto& refs = root.at("artifact_refs").as_object();
    const auto& headless = root.at("headless").as_object();
    const auto& overflow = root.at("overflow").as_object();

    const auto contract_path = resolve_repo_path(refs.at("contract_path").as_string());
    const auto wfrog_path = resolve_repo_path(refs.at("wfrog_path").as_string());
    const auto snapshot_path = resolve_repo_path(refs.at("snapshot_path").as_string());
    const auto expected = load_json(snapshot_path);

    const auto bridge = frog::runtime::make_linked_native_kernel_bridge(native_manifest_path(), &frog_example05_run);

    frog::runtime::Slice05RuntimeCore runtime(contract_path, wfrog_path);
    const auto actual = runtime.execute_with_native_kernel_bridge(
        bridge,
        static_cast<std::uint16_t>(headless.at("input_value").as_i64()));
    assert(canonical_json(actual) == canonical_json(expected));

    bool rejected = false;
    try {
        static_cast<void>(runtime.execute_with_native_kernel_bridge(
            bridge,
            static_cast<std::uint16_t>(overflow.at("input_value").as_i64())));
    } catch (const std::exception& error) {
        rejected = true;
        assert(std::string(error.what()) == overflow.at("expected_error").as_string());
    }
    assert(rejected);
}

} // namespace

int main() {
    test_direct_llvm_kernel_bridge_call();
    test_runtime_uses_llvm_kernel_bridge();
    std::cout << "slice05 LLVM-produced native kernel bridge passed" << std::endl;
    return 0;
}
