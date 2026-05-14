from Implementations.Reference.Runtime.python.cli import execute_example09_contract
from Implementations.Reference.Runtime.python.ui_runtime import (
    PathBrowserUiRuntime,
    PathRuntimeCore,
    build_runtime,
    default_example09_contract_path,
    default_example09_wfrog_path,
)


def test_python_example09_headless_matches_path_runtime_surface() -> None:
    artifact = execute_example09_contract("C:/FROG/from_python.txt")

    assert artifact["outputs"]["public"]["result_path"] == "C:/FROG/from_python.txt"
    assert artifact["outputs"]["ui"]["path_result"] == "C:/FROG/from_python.txt"


def test_python_example09_browser_ui_consumes_frog_instance_and_wfrog_realization() -> None:
    runtime = PathBrowserUiRuntime(
        contract_path=default_example09_contract_path(),
        wfrog_path=default_example09_wfrog_path(),
        open_browser=False,
    )
    runtime.runtime.execute_all({
        "input_path": "C:/FROG/top.txt",
        "input_path_no_icon": "C:/FROG/bottom.txt",
    })
    html = runtime.render_html()

    assert "Path Value Roundtrip" in html
    assert 'data-runtime-language="python"' in html
    assert 'data-compiler-backend="none"' in html
    assert 'data-execution-path="python_path_contract_executor"' in html
    assert "data-frog-visual-law='wfrog-realization-state-map'" in html
    assert "data-asset-route='/asset/path_rectangular_field_svg'" in html
    assert "class='path-skin'" in html
    assert "data-frog-part='browse_button'" in html
    assert "name='input_path' type='text'" in html
    assert "name='input_path_no_icon' type='text'" in html
    assert "data-frog-input-id='input_path'" in html
    assert "data-frog-input-id='input_path_no_icon'" in html
    assert "--frog-path-icon-display:none" in html
    assert "--frog-path-face-stroke-width:2px" in html
    assert "frogPathPicked" in html
    assert "path-card" not in html
    assert "fallback" not in html
    assert "Current runtime snapshot" not in html
    assert "<pre>" not in html
    assert "C:/FROG/top.txt" in html
    assert "C:/FROG/bottom.txt" in html


def test_python_example09_independent_preview_pair_does_not_mutate_primary_pair() -> None:
    core = PathRuntimeCore(
        contract_path=default_example09_contract_path(),
        wfrog_path=default_example09_wfrog_path(),
    )
    artifact = core.execute_all({
        "input_path": "C:/FROG/primary.txt",
        "input_path_no_icon": "C:/FROG/preview.txt",
    })

    assert artifact["outputs"]["public"]["result_path"] == "C:/FROG/primary.txt"
    assert artifact["outputs"]["ui"]["path_result"] == "C:/FROG/primary.txt"
    assert artifact["outputs"]["ui"]["path_result_no_icon"] == "C:/FROG/preview.txt"


def test_python_runtime_builder_dispatches_example09() -> None:
    runtime = build_runtime(example="09", open_browser=False)

    assert isinstance(runtime, PathBrowserUiRuntime)
