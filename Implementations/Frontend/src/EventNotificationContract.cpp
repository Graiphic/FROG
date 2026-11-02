#include "frog_native_engine/EventNotificationContract.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include <initializer_list>

namespace frog::native_engine {
namespace {
constexpr std::uint32_t scopeBit(EventSourceScope scope) noexcept {
    return 1U << static_cast<unsigned>(scope);
}
}
std::span<const EventNotificationSpec> eventNotificationCatalog() noexcept {
    using S = EventSourceScope;
    using C = EventControlCategory;
    constexpr auto application = scopeBit(S::Application);
    constexpr auto vi = scopeBit(S::Vi), pane = scopeBit(S::Pane), control = scopeBit(S::Control);
    constexpr auto interactive = vi | pane | control;
    constexpr std::string_view key="char virtual_key scan_code mods platform_mods focus_object";
    constexpr std::string_view keyResponse="discard char virtual_key mods platform_mods focus_object";
    constexpr std::string_view mouse="coords button mods platform_mods";
    constexpr std::string_view drag="coords button mods platform_mods available_data_names";
    constexpr std::string_view shortcut="menu_ref item_tag item_path coords sub_object";
    constexpr std::string_view cursor="cursor_index graph_cursor_ref plot_index point_index cursor_location";
    constexpr std::string_view unverified="No verified payload/response contract exists for this catalogue entry; common metadata is only an authoring subset.";
    static constexpr EventNotificationSpec specs[]{
        {"timeout", "Timeout", "Timing", application, C::Any, false, {}, {}, false, {}, "NI S05; FROG public common fields"},
        {"value_change", "Value Change", "Value", control, C::Any, false, "old_value new_value", {}, true, {}, "NI S06"},
        {"key_down", "Key Down", "Key", interactive, C::Any, false, key, {}, true, {}, "NI S08/S13"},
        {"key_down_filter", "Key Down?", "Key", interactive, C::Any, true, key, keyResponse, true, {}, "NI S09/S13"},
        {"key_repeat", "Key Repeat", "Key", interactive, C::Any, false, key, {}, true, {}, "NI S10"},
        {"key_repeat_filter", "Key Repeat?", "Key", interactive, C::Any, true, key, keyResponse, true, {}, "NI S11"},
        {"key_up", "Key Up", "Key", interactive, C::Any, false, "scan_code mods platform_mods focus_object", {}, true, {}, "NI S12"},
        {"mouse_down", "Mouse Down", "Mouse", interactive, C::Any, false, mouse, {}, true, {}, "NI S14"},
        {"mouse_down_filter", "Mouse Down?", "Mouse", interactive, C::Any, true, mouse, "discard button mods platform_mods", true, {}, "NI S15"},
        {"mouse_enter", "Mouse Enter", "Mouse", pane | control, C::Any, false, {}, {}, true, {}, "NI S17"},
        {"mouse_leave", "Mouse Leave", "Mouse", pane | control, C::Any, false, {}, {}, true, {}, "NI S18"},
        {"mouse_move", "Mouse Move", "Mouse", interactive, C::Any, false, "coords", {}, true, {}, "NI S19/S21"},
        {"mouse_up", "Mouse Up", "Mouse", interactive, C::Any, false, mouse, {}, true, {}, "NI S16"},
        {"mouse_wheel", "Mouse Wheel", "Mouse", interactive, C::Any, false, "coords delta orientation button_mods mods platform_mods", {}, true, {}, "NI S20"},
        {"focus_gained", "Focus Gained", "Focus", control, C::Any, false, {}, {}, false, unverified, "FROG catalogue; NI contract unverified"},
        {"focus_lost", "Focus Lost", "Focus", control, C::Any, false, {}, {}, false, unverified, "FROG catalogue; NI contract unverified"},
        {"drag_ended", "Drag Ended", "Drag", pane | control, C::Any, false, "drag_result", {}, true, {}, "NI S44"},
        {"drag_enter", "Drag Enter", "Drag", pane | control, C::Any, false, drag, "accepted", true, {}, "NI S39"},
        {"drag_leave", "Drag Leave", "Drag", pane | control, C::Any, false, "available_data_names", {}, true, {}, "NI S41"},
        {"drag_over", "Drag Over", "Drag", pane | control, C::Any, false, drag, "accepted", true, {}, "NI S40"},
        {"drag_source_update", "Drag Source Update", "Drag", control, C::Any, false, "drag_result", "drag_cursor", true, {}, "NI S43"},
        {"drop", "Drop", "Drag", pane | control, C::Any, false, drag, "accepted", true, {}, "NI S42"},
        {"shortcut_menu_activation_filter", "Shortcut Menu Activation?", "Shortcut Menu", pane | control, C::Any, true, "menu_ref coords sub_object", "discard", true, {}, "NI S33"},
        {"shortcut_menu_dismissed", "Shortcut Menu Dismissed", "Shortcut Menu", pane | control, C::Any, false, {}, {}, false, unverified, "FROG catalogue; NI contract unverified"},
        {"shortcut_menu_selection_app_filter", "Shortcut Menu Selection? (App)", "Shortcut Menu", pane | control, C::Any, true, shortcut, "discard", true, {}, "NI S35"},
        {"shortcut_menu_selection_app", "Shortcut Menu Selection (App)", "Shortcut Menu", pane | control, C::Any, false, shortcut, {}, true, {}, "NI S34"},
        {"shortcut_menu_selection_user", "Shortcut Menu Selection (User)", "Shortcut Menu", pane | control, C::Any, false, shortcut, {}, true, {}, "NI S36"},
        // NI documents this on Application, not on the activated VI. The
        // historical VI scope remains inspectable but cannot be newly bound.
        // Field labels/types are explicit FROG authoring mappings, not NI ABI.
        {"vi_activation", "FROG Activation", "Window", application | vi, C::Any, false,
            "program_name program_window program_application_ref", {}, true, {},
            "NI Application/notify/viactivation; FROG program_window.v1 mapping; runtime unavailable"},
        {"vi_deactivation", "FROG Deactivation", "Window", vi, C::Any, false, {}, {}, false, unverified, "FROG catalogue; NI contract unverified"},
        {"panel_close", "Panel Close", "Window", vi, C::Any, false, {}, {}, true, {}, "NI S25"},
        {"panel_close_filter", "Panel Close?", "Window", vi, C::Any, true, {}, "discard", true, {}, "NI S26"},
        {"panel_resize", "Panel Resize", "Window", vi, C::Any, false, "old_bounds new_bounds window_action previous_window_state", {}, true, {}, "NI S27"},
        {"pane_resize", "Pane Resize", "Pane", pane, C::Any, false, "old_bounds new_bounds", {}, true, {}, "NI S28"},
        {"pane_origin_change", "Pane Origin Change", "Pane", pane, C::Any, false, {}, {}, false, unverified, "FROG catalogue; NI contract unverified"},
        {"application_instance_close", "Application Instance Close", "Application", application, C::Any, false, {}, {}, true, {}, "NI S23"},
        {"application_instance_close_filter", "Application Instance Close?", "Application", application, C::Any, true, "silent_shutdown", "discard silent_shutdown", true, {}, "NI S24; FROG response policy v1 (preserve incoming when unwired)"},
        {"cursor_move", "Cursor Move", "Graph", control, C::WaveformGraph, false, cursor, {}, true, {}, "NI S56"},
        {"cursor_grab", "Cursor Grab", "Graph", control, C::WaveformGraph, false, cursor, {}, true, {}, "NI S54"},
        {"cursor_release", "Cursor Release", "Graph", control, C::WaveformGraph, false, cursor, {}, true, {}, "NI S57"},
        {"scale_range_change", "Scale Range Change", "Graph", control, C::WaveformGraph, false, "scale_kind scale_index scale_ref new_range", {}, true, {}, "NI S58"},
        {"plot_attribute_change", "Plot Attribute Change", "Graph", control, C::WaveformGraph, false, "plot_index plot_ref old_attributes new_attributes", {}, true, {}, "NI S60"},
        // GS XY notifications retain exact source coordinates/IDs in a typed
        // String payload; they do not reuse NI's f64 cursor/ref ABI or ValueChange.
        {"xy_cursor_move", "XY Cursor Changed", "Graph XY", control, C::WaveformGraph, false, "target_id graph_update", {}, true, {}, "GS XY exact JSON notification v1"},
        {"xy_view_change", "XY View Changed", "Graph XY", control, C::WaveformGraph, false, "target_id graph_update", {}, true, {}, "GS XY exact JSON notification v1"},
        {"xy_plot_attribute_change", "XY Plot Changed", "Graph XY", control, C::WaveformGraph, false, "target_id graph_update", {}, true, {}, "GS XY exact JSON notification v1"},
        {"xy_annotation_change", "XY Annotation Changed", "Graph XY", control, C::WaveformGraph, false, "target_id graph_update", {}, true, {}, "GS XY exact JSON notification v1"},
        {"user_event", "User Event", "Dynamic", scopeBit(S::Dynamic), C::Any, false, {}, {}, false, {}, "NI User Events; FROG typed payload"}};
    return specs;
}
const EventNotificationSpec *eventNotificationSpec(std::string_view kind) noexcept {
    for (const auto &spec : eventNotificationCatalog()) if (spec.kind == kind) return &spec;
    return nullptr;
}
std::string_view eventSourceScopeName(EventSourceScope scope) noexcept {
    switch (scope) {
    case EventSourceScope::Application: return "application";
    case EventSourceScope::Vi: return "vi";
    case EventSourceScope::Pane: return "pane";
    case EventSourceScope::Control: return "control";
    case EventSourceScope::Dynamic: return "dynamic";
    }
    return {};
}
EventSourceScope eventBindingSourceScope(const DiagramEventBinding &binding) noexcept {
    for (const auto scope : {EventSourceScope::Application, EventSourceScope::Vi,
            EventSourceScope::Pane, EventSourceScope::Control, EventSourceScope::Dynamic})
        if (binding.sourceScope == eventSourceScopeName(scope)) return scope;
    return binding.source.empty() ? EventSourceScope::Application : EventSourceScope::Control;
}
std::vector<DiagramEventBinding> eventCaseBindings(const DiagramEventCase &eventCase) {
    return eventCase.eventBindings.empty() ?
        std::vector<DiagramEventBinding>{{eventCase.eventKind, eventCase.eventSource}} : eventCase.eventBindings;
}
}
