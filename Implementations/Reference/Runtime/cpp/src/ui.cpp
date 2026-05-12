#include "ui.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "json.hpp"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <shellapi.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace frog::runtime {

namespace {

using frog::json::Object;
using frog::json::Value;

#ifdef _WIN32
using socket_t = SOCKET;
constexpr socket_t invalid_socket = INVALID_SOCKET;
#else
using socket_t = int;
constexpr socket_t invalid_socket = -1;
#endif

struct NetworkBootstrap {
    NetworkBootstrap() {
#ifdef _WIN32
        WSADATA data;
        if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
            throw std::runtime_error("WSAStartup failed.");
        }
#endif
    }
    ~NetworkBootstrap() {
#ifdef _WIN32
        WSACleanup();
#endif
    }
};

void close_socket(socket_t socket) {
    if (socket == invalid_socket) {
        return;
    }
#ifdef _WIN32
    closesocket(socket);
#else
    close(socket);
#endif
}

std::string read_text_file(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        throw std::runtime_error("Unable to open file: " + path.string());
    }
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

std::string html_escape(const std::string& input) {
    std::string out;
    out.reserve(input.size());
    for (const char ch : input) {
        switch (ch) {
        case '&': out += "&amp;"; break;
        case '<': out += "&lt;"; break;
        case '>': out += "&gt;"; break;
        case '"': out += "&quot;"; break;
        case '\'': out += "&#39;"; break;
        default: out.push_back(ch); break;
        }
    }
    return out;
}

std::string property_string(const Object& properties, const std::string& key, const std::string& fallback = "") {
    const auto it = properties.find(key);
    if (it == properties.end() || !it->second.is_string()) {
        return fallback;
    }
    return it->second.as_string();
}

bool property_bool(const Object& properties, const std::string& key, bool fallback = false) {
    const auto it = properties.find(key);
    if (it == properties.end() || !it->second.is_bool()) {
        return fallback;
    }
    return it->second.as_bool();
}

std::uint16_t property_u16(const Object& properties, const std::string& key, std::uint16_t fallback = 0) {
    const auto it = properties.find(key);
    if (it == properties.end() || !it->second.is_number()) {
        return fallback;
    }
    const auto value = it->second.as_i64();
    if (value < 0 || value > 65535) {
        throw std::runtime_error("Widget value must remain in the u16 domain.");
    }
    return static_cast<std::uint16_t>(value);
}

std::optional<double> property_number(const Object& properties, const std::string& key) {
    const auto it = properties.find(key);
    if (it == properties.end() || !it->second.is_number()) {
        return std::nullopt;
    }
    return it->second.as_f64();
}

std::int64_t layout_i64(const Value& layout, const std::string& key, std::int64_t fallback = 0) {
    if (!layout.is_object()) {
        return fallback;
    }
    const auto& object = layout.as_object();
    const auto it = object.find(key);
    if (it == object.end() || !it->second.is_number()) {
        return fallback;
    }
    return it->second.as_i64();
}

std::string css_px(std::int64_t value) {
    return std::to_string(value) + "px";
}

std::string css_percent(double value) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << value << "%";
    return out.str();
}

bool is_safe_hex_color(const std::string& value) {
    if (!(value.size() == 4 || value.size() == 7) || value.front() != '#') {
        return false;
    }
    return std::all_of(value.begin() + 1, value.end(), [](unsigned char ch) {
        return std::isxdigit(ch) != 0;
    });
}

std::string safe_css_color(const std::string& value, const std::string& fallback) {
    if (value == "transparent") {
        return value;
    }
    return is_safe_hex_color(value) ? value : fallback;
}

std::string safe_css_length(const std::string& value, const std::string& fallback) {
    if (value.size() <= 2 || value.substr(value.size() - 2) != "px") {
        return fallback;
    }
    const auto number = value.substr(0, value.size() - 2);
    bool saw_digit = false;
    bool saw_dot = false;
    for (const auto ch : number) {
        if (std::isdigit(static_cast<unsigned char>(ch)) != 0) {
            saw_digit = true;
            continue;
        }
        if (ch == '.' && !saw_dot) {
            saw_dot = true;
            continue;
        }
        return fallback;
    }
    return saw_digit ? value : fallback;
}

std::string safe_css_font_weight(const std::string& value, const std::string& fallback) {
    if (value == "normal" || value == "bold" || value == "lighter" || value == "bolder") {
        return value;
    }
    if (value.size() != 3 || !std::all_of(value.begin(), value.end(), [](unsigned char ch) {
            return std::isdigit(ch) != 0;
        })) {
        return fallback;
    }
    const auto weight = std::stoi(value);
    return weight >= 100 && weight <= 900 && weight % 100 == 0 ? value : fallback;
}

struct SvgGeometry {
    double view_width = 380.0;
    double view_height = 150.0;
    double caption_x = 16.0;
    double caption_y = 46.0;
    double value_face_x = 22.0;
    double value_face_y = 82.0;
    double value_face_width = 214.0;
    double value_face_height = 28.0;
    double increment_up_x = 246.0;
    double increment_up_y = 82.0;
    double increment_up_width = 30.0;
    double increment_up_height = 13.0;
    double increment_down_x = 246.0;
    double increment_down_y = 97.0;
    double increment_down_width = 30.0;
    double increment_down_height = 13.0;
};

std::optional<std::size_t> find_svg_element_with_id(const std::string& svg, const std::string& id) {
    const std::string double_quoted = "id=\"" + id + "\"";
    const auto id_pos = svg.find(double_quoted);
    if (id_pos == std::string::npos) {
        return std::nullopt;
    }
    const auto tag_start = svg.rfind('<', id_pos);
    if (tag_start == std::string::npos) {
        return std::nullopt;
    }
    return tag_start;
}

std::optional<std::string> svg_attribute(const std::string& svg, const std::string& element_id, const std::string& attribute) {
    const auto tag_start = find_svg_element_with_id(svg, element_id);
    if (!tag_start.has_value()) {
        return std::nullopt;
    }
    const auto tag_end = svg.find('>', *tag_start);
    if (tag_end == std::string::npos) {
        return std::nullopt;
    }
    const auto attr_pos = svg.find(attribute + "=\"", *tag_start);
    if (attr_pos == std::string::npos || attr_pos > tag_end) {
        return std::nullopt;
    }
    const auto value_start = attr_pos + attribute.size() + 2;
    const auto value_end = svg.find('"', value_start);
    if (value_end == std::string::npos || value_end > tag_end) {
        return std::nullopt;
    }
    return svg.substr(value_start, value_end - value_start);
}

double svg_attribute_double(const std::string& svg, const std::string& element_id, const std::string& attribute, double fallback) {
    const auto value = svg_attribute(svg, element_id, attribute);
    if (!value.has_value()) {
        return fallback;
    }
    try {
        return std::stod(*value);
    } catch (const std::exception&) {
        return fallback;
    }
}

std::optional<std::string> svg_child_rect_attribute(const std::string& svg, const std::string& group_id, const std::string& attribute) {
    const auto tag_start = find_svg_element_with_id(svg, group_id);
    if (!tag_start.has_value()) {
        return std::nullopt;
    }
    const auto group_end = svg.find("</g>", *tag_start);
    const auto rect_start = svg.find("<rect", *tag_start);
    if (group_end == std::string::npos || rect_start == std::string::npos || rect_start > group_end) {
        return std::nullopt;
    }
    const auto rect_end = svg.find('>', rect_start);
    if (rect_end == std::string::npos || rect_end > group_end) {
        return std::nullopt;
    }
    const auto attr_pos = svg.find(attribute + "=\"", rect_start);
    if (attr_pos == std::string::npos || attr_pos > rect_end) {
        return std::nullopt;
    }
    const auto value_start = attr_pos + attribute.size() + 2;
    const auto value_end = svg.find('"', value_start);
    if (value_end == std::string::npos || value_end > rect_end) {
        return std::nullopt;
    }
    return svg.substr(value_start, value_end - value_start);
}

double svg_child_rect_attribute_double(const std::string& svg, const std::string& group_id, const std::string& attribute, double fallback) {
    const auto value = svg_child_rect_attribute(svg, group_id, attribute);
    if (!value.has_value()) {
        return fallback;
    }
    try {
        return std::stod(*value);
    } catch (const std::exception&) {
        return fallback;
    }
}

void parse_viewbox(const std::string& svg, SvgGeometry& geometry) {
    const std::string marker = "viewBox=\"";
    const auto start = svg.find(marker);
    if (start == std::string::npos) {
        return;
    }
    const auto value_start = start + marker.size();
    const auto value_end = svg.find('"', value_start);
    if (value_end == std::string::npos) {
        return;
    }
    std::istringstream input(svg.substr(value_start, value_end - value_start));
    double min_x = 0.0;
    double min_y = 0.0;
    double width = geometry.view_width;
    double height = geometry.view_height;
    if (input >> min_x >> min_y >> width >> height) {
        (void)min_x;
        (void)min_y;
        if (width > 0.0 && height > 0.0) {
            geometry.view_width = width;
            geometry.view_height = height;
        }
    }
}

SvgGeometry load_svg_geometry(const WidgetState& widget) {
    SvgGeometry geometry;
    if (widget.asset_path.empty() || !std::filesystem::exists(widget.asset_path)) {
        return geometry;
    }
    const auto svg = read_text_file(widget.asset_path);
    parse_viewbox(svg, geometry);
    geometry.caption_x = svg_attribute_double(svg, "caption_text", "x", geometry.caption_x);
    geometry.caption_y = svg_attribute_double(svg, "caption_text", "y", geometry.caption_y);
    geometry.value_face_x = svg_attribute_double(svg, "value_face", "x", geometry.value_face_x);
    geometry.value_face_y = svg_attribute_double(svg, "value_face", "y", geometry.value_face_y);
    geometry.value_face_width = svg_attribute_double(svg, "value_face", "width", geometry.value_face_width);
    geometry.value_face_height = svg_attribute_double(svg, "value_face", "height", geometry.value_face_height);
    geometry.increment_up_x = svg_child_rect_attribute_double(svg, "increment_up", "x", geometry.increment_up_x);
    geometry.increment_up_y = svg_child_rect_attribute_double(svg, "increment_up", "y", geometry.increment_up_y);
    geometry.increment_up_width = svg_child_rect_attribute_double(svg, "increment_up", "width", geometry.increment_up_width);
    geometry.increment_up_height = svg_child_rect_attribute_double(svg, "increment_up", "height", geometry.increment_up_height);
    geometry.increment_down_x = svg_child_rect_attribute_double(svg, "increment_down", "x", geometry.increment_down_x);
    geometry.increment_down_y = svg_child_rect_attribute_double(svg, "increment_down", "y", geometry.increment_down_y);
    geometry.increment_down_width = svg_child_rect_attribute_double(svg, "increment_down", "width", geometry.increment_down_width);
    geometry.increment_down_height = svg_child_rect_attribute_double(svg, "increment_down", "height", geometry.increment_down_height);
    return geometry;
}

double pct(double value, double total) {
    if (total <= 0.0) {
        return 0.0;
    }
    return (value / total) * 100.0;
}

std::string svg_anchor_style(double x, double y, const SvgGeometry& geometry) {
    std::ostringstream style;
    style << "left:" << css_percent(pct(x, geometry.view_width)) << ";";
    style << "top:" << css_percent(pct(y, geometry.view_height)) << ";";
    return style.str();
}

std::string caption_transform_for_align(const std::string& align) {
    if (align == "center") {
        return "translate(-50%,-50%)";
    }
    if (align == "right" || align == "end") {
        return "translate(-100%,-50%)";
    }
    return "translateY(-50%)";
}

std::string caption_text_align(const std::string& align) {
    if (align == "center") {
        return "center";
    }
    if (align == "right" || align == "end") {
        return "right";
    }
    return "left";
}

std::string caption_anchor_style(const Object& properties, const SvgGeometry& geometry) {
    const auto x = property_number(properties, "caption.anchor.x").value_or(geometry.caption_x);
    const auto y = property_number(properties, "caption.anchor.y").value_or(geometry.caption_y);
    const auto align = property_string(properties, "caption.align.horizontal", "left");
    std::ostringstream style;
    style << svg_anchor_style(x, y, geometry);
    style << "transform:" << caption_transform_for_align(align) << ";";
    style << "text-align:" << caption_text_align(align) << ";";
    if (!property_bool(properties, "caption.visible", true)) {
        style << "display:none;";
    }
    return style.str();
}

std::string svg_box_style(double x, double y, double width, double height, const SvgGeometry& geometry) {
    std::ostringstream style;
    style << "left:" << css_percent(pct(x, geometry.view_width)) << ";";
    style << "top:" << css_percent(pct(y, geometry.view_height)) << ";";
    style << "width:" << css_percent(pct(width, geometry.view_width)) << ";";
    style << "height:" << css_percent(pct(height, geometry.view_height)) << ";";
    return style.str();
}

std::string asset_route(const WidgetState& widget) {
    return widget.asset_id.has_value() ? "/asset/" + *widget.asset_id : std::string();
}

std::string render_numeric_skin(const WidgetState& widget, bool is_control, const std::string& color) {
    if (widget.asset_path.empty() || !std::filesystem::exists(widget.asset_path)) {
        return "<div class='numeric-skin missing-skin'></div>";
    }
    const auto frame_fill = safe_css_color(property_string(widget.properties, "style.frame.fill_color", "#ffffff"), "#ffffff");
    const auto frame_stroke = safe_css_color(property_string(widget.properties, "style.frame.border_color", "#000000"), "#000000");
    const auto frame_stroke_width = safe_css_length(property_string(widget.properties, "style.frame.border_width", "2px"), "2px");
    const auto value_face_fill = safe_css_color(property_string(widget.properties, "style.value_face.fill_color", color), color);
    const auto value_face_stroke = safe_css_color(property_string(widget.properties, "style.value_face.border_color", "transparent"), "transparent");
    const auto value_face_stroke_width = safe_css_length(property_string(widget.properties, "style.value_face.border_width", "0px"), "0px");
    const auto step_fill = safe_css_color(property_string(widget.properties, "style.increment_button.fill_color.normal", color), color);
    const auto step_symbol = safe_css_color(property_string(widget.properties, "style.increment_button.symbol_color.normal", "#ffffff"), "#ffffff");
    std::ostringstream style;
    style << "--frog-numeric-caption-display:none;";
    style << "--frog-numeric-text-display:none;";
    style << "--frog-numeric-frame-fill:" << html_escape(frame_fill) << ";";
    style << "--frog-numeric-frame-stroke:" << html_escape(frame_stroke) << ";";
    style << "--frog-numeric-frame-stroke-width:" << html_escape(frame_stroke_width) << ";";
    style << "--frog-numeric-unit-display:" << (property_bool(widget.properties, "unit_label.visible", false) ? "inline" : "none") << ";";
    style << "--frog-numeric-radix-display:" << (property_bool(widget.properties, "display.radix_visible", false) ? "inline" : "none") << ";";
    style << "--frog-numeric-spinner-display:" << (is_control && property_bool(widget.properties, "display.increment_buttons_visible", true) ? "inline" : "none") << ";";
    style << "--frog-numeric-value-face-fill:" << html_escape(value_face_fill) << ";";
    style << "--frog-numeric-value-face-stroke:" << html_escape(value_face_stroke) << ";";
    style << "--frog-numeric-value-face-stroke-width:" << html_escape(value_face_stroke_width) << ";";
    style << "--frog-numeric-spinner-fill:" << html_escape(step_fill) << ";";
    style << "--frog-numeric-spinner-stroke:" << html_escape(step_symbol) << ";";

    std::ostringstream html;
    html << "<div class='numeric-skin' aria-hidden='true' style='" << style.str() << "'>";
    html << read_text_file(widget.asset_path);
    html << "</div>";
    return html.str();
}

std::uint16_t property_step(const Object& properties, const std::string& key, std::uint16_t fallback) {
    const auto value = property_u16(properties, key, fallback);
    return value == 0 ? fallback : value;
}

std::string numeric_step_button_state_style(const WidgetState& widget) {
    const auto normal_fill = safe_css_color(property_string(widget.properties, "style.increment_button.fill_color.normal", "#5B9BD5"), "#5B9BD5");
    const auto pressed_fill = safe_css_color(property_string(widget.properties, "style.increment_button.fill_color.pressed", "#2B4F7B"), "#2B4F7B");
    const auto normal_border = safe_css_color(property_string(widget.properties, "style.increment_button.border_color.normal", "transparent"), "transparent");
    const auto pressed_border = safe_css_color(property_string(widget.properties, "style.increment_button.border_color.pressed", normal_border), normal_border);
    const auto normal_symbol = safe_css_color(property_string(widget.properties, "style.increment_button.symbol_color.normal", "#ffffff"), "#ffffff");
    const auto pressed_symbol = safe_css_color(property_string(widget.properties, "style.increment_button.symbol_color.pressed", normal_symbol), normal_symbol);
    std::ostringstream style;
    style << "--frog-numeric-step-fill:" << html_escape(normal_fill) << ";";
    style << "--frog-numeric-step-fill-pressed:" << html_escape(pressed_fill) << ";";
    style << "--frog-numeric-step-border:" << html_escape(normal_border) << ";";
    style << "--frog-numeric-step-border-pressed:" << html_escape(pressed_border) << ";";
    style << "--frog-numeric-step-symbol:" << html_escape(normal_symbol) << ";";
    style << "--frog-numeric-step-symbol-pressed:" << html_escape(pressed_symbol) << ";";
    return style.str();
}

std::string render_numeric_widget(const WidgetState& widget) {
    const bool is_control = widget.role == "control";
    const auto geometry = load_svg_geometry(widget);
    const auto x = layout_i64(widget.layout, "x", 0);
    const auto y = layout_i64(widget.layout, "y", 0);
    const auto width = layout_i64(widget.layout, "width", 160);
    const auto height = layout_i64(widget.layout, "height", 48);
    const auto value = property_u16(widget.properties, "value", 0);
    const auto label = property_string(widget.properties, "caption.text", property_string(widget.properties, "label", widget.widget_id));
    const auto color = safe_css_color(property_string(widget.properties, "foreground_color", "#ffffff"), "#ffffff");
    const auto label_color = safe_css_color(property_string(widget.properties, "label_color", "#111827"), "#111827");
    const auto label_weight = safe_css_font_weight(property_string(widget.properties, "style.caption.font_weight", "400"), "400");
    const auto route = asset_route(widget);
    const auto minimum = property_u16(widget.properties, "data_entry.minimum", 0);
    const auto maximum = property_u16(widget.properties, "data_entry.maximum", 65535);
    const auto step = property_step(widget.properties, "data_entry.increment_step", 1);

    std::ostringstream html;
    html << "<section class='frog-widget numeric-widget " << (is_control ? "numeric-control" : "numeric-indicator") << "'";
    html << " data-widget-id='" << html_escape(widget.widget_id) << "'";
    html << " data-class-ref='" << html_escape(widget.class_ref) << "'";
    html << " data-role='" << html_escape(widget.role) << "'";
    html << " data-frog-visual-law='wfrog-realization-state-map'";
    if (!route.empty()) {
        html << " data-asset-route='" << html_escape(route) << "'";
    }
    html << " style='position:absolute;left:" << css_px(x) << ";top:" << css_px(y) << ";width:" << css_px(width)
         << ";height:" << css_px(height) << ";";
    if (!property_bool(widget.properties, "visible", true)) {
        html << "display:none;";
    }
    html << "'>";

    html << render_numeric_skin(widget, is_control, color);

    html << "<span class='numeric-label-overlay' data-frog-part='caption' data-svg-anchor='caption.anchor' style='"
          << caption_anchor_style(widget.properties, geometry)
          << "color:" << html_escape(label_color) << ";font-weight:" << html_escape(label_weight) << ";'>" << html_escape(label) << "</span>";

    const auto value_style = svg_box_style(
        geometry.value_face_x,
        geometry.value_face_y,
        geometry.value_face_width,
        geometry.value_face_height,
        geometry);

    if (is_control) {
        html << "<input id='" << html_escape(widget.widget_id) << "_value' name='input_value' type='number' min='" << minimum << "' max='" << maximum << "' step='" << step << "'";
        html << " class='numeric-value-overlay numeric-control-editor' data-frog-part='text_value' data-svg-anchor='text_value.center'";
        html << " style='" << value_style << "color:#111827;'";
        html << " value='" << value << "'";
        if (!property_bool(widget.properties, "enabled", true)) {
            html << " disabled";
        }
        html << " />";
        if (property_bool(widget.properties, "display.increment_buttons_visible", true)) {
            const auto step_state_style = numeric_step_button_state_style(widget);
            html << "<button type='button' class='numeric-step-overlay numeric-increment' data-target='" << html_escape(widget.widget_id) << "_value' data-step='" << step << "' data-frog-part='increment_up' data-frog-method='increment' data-frog-button-state-law='normal-pressed' aria-label='Increment " << html_escape(label) << "' style='"
                 << svg_box_style(geometry.increment_up_x, geometry.increment_up_y, geometry.increment_up_width, geometry.increment_up_height, geometry)
                 << step_state_style
                 << "'></button>";
            html << "<button type='button' class='numeric-step-overlay numeric-decrement' data-target='" << html_escape(widget.widget_id) << "_value' data-step='-" << step << "' data-frog-part='increment_down' data-frog-method='decrement' data-frog-button-state-law='normal-pressed' aria-label='Decrement " << html_escape(label) << "' style='"
                 << svg_box_style(geometry.increment_down_x, geometry.increment_down_y, geometry.increment_down_width, geometry.increment_down_height, geometry)
                 << step_state_style
                 << "'></button>";
        }
    } else {
        html << "<output class='numeric-value-overlay numeric-indicator-value' data-frog-part='text_value' data-svg-anchor='text_value.center'";
        html << " style='" << value_style << "color:#111827;'>" << value << "</output>";
    }

    html << "</section>";
    return html.str();
}

SvgGeometry load_string_svg_geometry(const WidgetState& widget) {
    SvgGeometry geometry;
    geometry.view_width = 420.0;
    geometry.view_height = 190.0;
    geometry.caption_x = 16.0;
    geometry.caption_y = 46.0;
    geometry.value_face_x = 28.0;
    geometry.value_face_y = 88.0;
    geometry.value_face_width = 364.0;
    geometry.value_face_height = 56.0;
    if (widget.asset_path.empty() || !std::filesystem::exists(widget.asset_path)) {
        return geometry;
    }
    const auto svg = read_text_file(widget.asset_path);
    parse_viewbox(svg, geometry);
    geometry.caption_x = svg_attribute_double(svg, "caption_text", "x", geometry.caption_x);
    geometry.caption_y = svg_attribute_double(svg, "caption_text", "y", geometry.caption_y);
    geometry.value_face_x = svg_attribute_double(svg, "text_region", "x", geometry.value_face_x);
    geometry.value_face_y = svg_attribute_double(svg, "text_region", "y", geometry.value_face_y);
    geometry.value_face_width = svg_attribute_double(svg, "text_region", "width", geometry.value_face_width);
    geometry.value_face_height = svg_attribute_double(svg, "text_region", "height", geometry.value_face_height);
    return geometry;
}

std::string render_string_skin(const WidgetState& widget) {
    if (widget.asset_path.empty() || !std::filesystem::exists(widget.asset_path)) {
        return "<div class='string-skin missing-skin'></div>";
    }
    const auto frame_fill = safe_css_color(property_string(widget.properties, "style.frame.fill_color", "transparent"), "transparent");
    const auto frame_stroke = safe_css_color(property_string(widget.properties, "style.frame.border_color", "transparent"), "transparent");
    const auto frame_stroke_width = safe_css_length(property_string(widget.properties, "style.frame.border_width", "0px"), "0px");
    const auto region_fill = safe_css_color(property_string(widget.properties, "style.text_region.fill_color", "#ffffff"), "#ffffff");
    const auto region_stroke = safe_css_color(property_string(widget.properties, "style.text_region.border_color", "#64748b"), "#64748b");
    const auto region_stroke_width = safe_css_length(property_string(widget.properties, "style.text_region.border_width", "2px"), "2px");
    const auto region_hover_fill = safe_css_color(property_string(widget.properties, "style.text_region.fill_color.hover", region_fill), region_fill);
    const auto region_hover_stroke = safe_css_color(property_string(widget.properties, "style.text_region.border_color.hover", region_stroke), region_stroke);
    const auto region_hover_stroke_width =
        safe_css_length(property_string(widget.properties, "style.text_region.border_width.hover", region_stroke_width), region_stroke_width);
    const auto text_fill = safe_css_color(property_string(widget.properties, "style.text.color", "#111827"), "#111827");
    const auto text_size = safe_css_length(property_string(widget.properties, "style.text.font_size", "16px"), "16px");
    const auto text_weight = safe_css_font_weight(property_string(widget.properties, "style.text.font_weight", "400"), "400");
    std::ostringstream style;
    style << "--frog-string-label-display:none;";
    style << "--frog-string-caption-display:none;";
    style << "--frog-string-placeholder-display:none;";
    style << "--frog-string-frame-fill:" << html_escape(frame_fill) << ";";
    style << "--frog-string-frame-stroke:" << html_escape(frame_stroke) << ";";
    style << "--frog-string-frame-stroke-width:" << html_escape(frame_stroke_width) << ";";
    style << "--frog-string-text-region-fill:" << html_escape(region_fill) << ";";
    style << "--frog-string-text-region-stroke:" << html_escape(region_stroke) << ";";
    style << "--frog-string-text-region-stroke-width:" << html_escape(region_stroke_width) << ";";
    style << "--frog-string-text-region-fill-hover:" << html_escape(region_hover_fill) << ";";
    style << "--frog-string-text-region-stroke-hover:" << html_escape(region_hover_stroke) << ";";
    style << "--frog-string-text-region-stroke-width-hover:" << html_escape(region_hover_stroke_width) << ";";
    style << "--frog-string-text-fill:" << html_escape(text_fill) << ";";
    style << "--frog-string-text-font-size:" << html_escape(text_size) << ";";
    style << "--frog-string-text-font-weight:" << html_escape(text_weight) << ";";

    std::ostringstream html;
    html << "<div class='string-skin' aria-hidden='true' style='" << style.str() << "'>";
    html << read_text_file(widget.asset_path);
    html << "</div>";
    return html.str();
}

std::string render_string_widget(const WidgetState& widget) {
    const bool is_control = widget.role == "control";
    const auto geometry = load_string_svg_geometry(widget);
    const auto x = layout_i64(widget.layout, "x", 0);
    const auto y = layout_i64(widget.layout, "y", 0);
    const auto width = layout_i64(widget.layout, "width", 240);
    const auto height = layout_i64(widget.layout, "height", 110);
    const auto value = property_string(widget.properties, "value");
    const auto label = property_string(widget.properties, "caption.text", widget.widget_id);
    const auto label_color = safe_css_color(property_string(widget.properties, "caption.style.text_color", "#111827"), "#111827");
    const auto text_color = safe_css_color(property_string(widget.properties, "style.text.color", "#111827"), "#111827");
    const auto text_size = safe_css_length(property_string(widget.properties, "style.text.font_size", "16px"), "16px");
    const auto text_weight = safe_css_font_weight(property_string(widget.properties, "style.text.font_weight", "400"), "400");
    const auto route = asset_route(widget);
    const auto value_style = svg_box_style(
        geometry.value_face_x,
        geometry.value_face_y,
        geometry.value_face_width,
        geometry.value_face_height,
        geometry);

    std::ostringstream html;
    html << "<section class='frog-widget string-widget " << (is_control ? "string-control" : "string-indicator") << "'";
    html << " data-widget-id='" << html_escape(widget.widget_id) << "'";
    html << " data-class-ref='" << html_escape(widget.class_ref) << "'";
    html << " data-role='" << html_escape(widget.role) << "'";
    html << " data-frog-visual-law='wfrog-realization-state-map'";
    if (!route.empty()) {
        html << " data-asset-route='" << html_escape(route) << "'";
    }
    html << " style='position:absolute;left:" << css_px(x) << ";top:" << css_px(y) << ";width:" << css_px(width)
         << ";height:" << css_px(height) << ";";
    if (!property_bool(widget.properties, "visible", true)) {
        html << "display:none;";
    }
    html << "'>";

    html << render_string_skin(widget);
    html << "<span class='string-caption-overlay' data-frog-part='caption' data-svg-anchor='caption.anchor' style='"
          << caption_anchor_style(widget.properties, geometry)
          << "color:" << html_escape(label_color) << ";'>" << html_escape(label) << "</span>";

    if (is_control) {
        html << "<input id='" << html_escape(widget.widget_id) << "_value' name='input_text' type='text'";
        html << " class='string-value-overlay string-control-editor' data-frog-part='text_value' data-svg-anchor='text_region.left_center'";
        html << " style='" << value_style << "color:" << html_escape(text_color) << ";font-size:" << html_escape(text_size)
             << ";font-weight:" << html_escape(text_weight) << ";'";
        html << " value='" << html_escape(value) << "'";
        if (!property_bool(widget.properties, "interaction.enabled", true)) {
            html << " disabled";
        }
        html << " />";
    } else {
        html << "<output class='string-value-overlay string-indicator-value' data-frog-part='text_value' data-svg-anchor='text_region.left_center'";
        html << " style='" << value_style << "color:" << html_escape(text_color) << ";font-size:" << html_escape(text_size)
             << ";font-weight:" << html_escape(text_weight) << ";'>" << html_escape(value) << "</output>";
    }

    html << "</section>";
    return html.str();
}

std::uint16_t parse_u16_form_value(const std::string& value) {
    const auto parsed = std::stoul(value);
    if (parsed > 65535ul) {
        throw std::runtime_error("input_value must remain in the u16 domain.");
    }
    return static_cast<std::uint16_t>(parsed);
}

bool parse_bool_form_value(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    if (value == "true" || value == "1" || value == "on") {
        return true;
    }
    if (value == "false" || value == "0" || value.empty()) {
        return false;
    }
    throw std::runtime_error("input_value must be a Boolean value.");
}

std::string boolean_text(bool value, const Object& properties) {
    return property_string(properties, value ? "state_text.true_text" : "state_text.false_text", value ? "TRUE" : "FALSE");
}

std::string state_property(const Object& properties, const std::string& base, const std::string& state, const std::string& fallback) {
    return property_string(properties, base + "." + state, fallback);
}

std::string render_boolean_widget(const WidgetState& widget) {
    const bool is_control = widget.role == "control";
    const auto geometry = load_svg_geometry(widget);
    const auto x = layout_i64(widget.layout, "x", 0);
    const auto y = layout_i64(widget.layout, "y", 0);
    const auto width = layout_i64(widget.layout, "width", 160);
    const auto height = layout_i64(widget.layout, "height", 80);
    const bool value = property_bool(widget.properties, "value", false);
    const std::string visual_state = value ? "true" : "false";
    const std::string hover_state = value ? "hover_true" : "hover_false";
    const std::string pressed_state = value ? "pressed_true" : "pressed_false";
    const std::string transition_state = value ? "transition_true_to_false" : "transition_false_to_true";
    const std::string variant = property_string(widget.properties, "realization.variant", widget.class_ref.find("indicator") != std::string::npos ? "circular" : "rectangular");
    const std::string state_text = boolean_text(value, widget.properties);
    const std::string caption = property_string(widget.properties, "caption.text", widget.widget_id);
    const std::string route = asset_route(widget);
    const std::string next_value = value ? "false" : "true";
    const std::string state_fill = state_property(widget.properties, "style.inner.fill_color", visual_state, value ? "#8bd86f" : "#ffffff");
    const std::string hover_fill = state_property(widget.properties, "style.inner.fill_color", hover_state, value ? "#9be884" : "#eef6ff");
    const std::string pressed_fill = state_property(widget.properties, "style.inner.fill_color", pressed_state, value ? "#6fc657" : "#dbeafe");
    const std::string state_border = state_property(widget.properties, "style.outer.border_color", visual_state, value ? "#184a24" : "#111827");
    const std::string hover_border = state_property(widget.properties, "style.outer.border_color", hover_state, value ? "#166534" : "#2563eb");
    const std::string pressed_border = state_property(widget.properties, "style.outer.border_color", pressed_state, value ? "#14532d" : "#1d4ed8");
    const std::string state_inner_border = state_property(widget.properties, "style.inner.border_color", visual_state, state_border);
    const std::string hover_inner_border = state_property(widget.properties, "style.inner.border_color", hover_state, hover_border);
    const std::string pressed_inner_border = state_property(widget.properties, "style.inner.border_color", pressed_state, pressed_border);
    const std::string text_color = state_property(widget.properties, "state_text.style.text_color", visual_state, value ? "#0b3d19" : "#111827");
    const std::string transition_ms = property_string(widget.properties, "style.transition.duration_ms", "120");
    const std::string transition_timing = property_string(widget.properties, "style.transition.timing", "ease-out");
    const std::string pressed_inset = property_string(widget.properties, "style.pressed.inset", "1px");
    const bool state_text_visible = property_bool(widget.properties, "state_text.visible", true);
    const bool frame_visible = property_bool(widget.properties, "style.frame.visible", true);
    const bool focus_visible = property_bool(widget.properties, "style.focus_ring.visible", false);
    const std::string inner_left = property_string(widget.properties, "style.inner.left", variant == "circular" ? "52px" : "18px");
    const std::string inner_top = property_string(widget.properties, "style.inner.top", variant == "circular" ? "23px" : "31px");
    const std::string inner_width = property_string(widget.properties, "style.inner.width", variant == "circular" ? "56px" : "124px");
    const std::string inner_height = property_string(widget.properties, "style.inner.height", variant == "circular" ? "56px" : "34px");
    const std::string focus_color = safe_css_color(property_string(widget.properties, "style.focus_ring.color", "#2563eb"), "#2563eb");
    const std::string focus_width = focus_visible
        ? safe_css_length(property_string(widget.properties, "style.focus_ring.width", "3px"), "3px")
        : "0px";

    std::ostringstream html;
    if (is_control) {
        html << "<button class='frog-widget boolean-widget boolean-control' type='submit'";
        html << " name='input_value' value='" << next_value << "' data-toggle-target='" << next_value << "'";
        html << " aria-pressed='" << (value ? "true" : "false") << "'";
    } else {
        html << "<section class='frog-widget boolean-widget boolean-indicator' aria-readonly='true'";
    }
    html << " data-widget-id='" << html_escape(widget.widget_id) << "'";
    html << " data-class-ref='" << html_escape(widget.class_ref) << "'";
    html << " data-role='" << html_escape(widget.role) << "'";
    if (widget.asset_id.has_value()) {
        html << " data-asset-ref='asset:" << html_escape(*widget.asset_id) << "'";
    }
    if (!route.empty()) {
        html << " data-asset-route='" << html_escape(route) << "'";
    }
    html << " data-current-value='" << (value ? "true" : "false") << "'";
    html << " data-realization-variant='" << html_escape(variant) << "'";
    html << " data-frog-visual-law='wfrog-realization-state-map'";
    html << " data-frog-visual-state='" << html_escape(visual_state) << "'";
    html << " data-frog-hover-state='" << html_escape(hover_state) << "'";
    html << " data-frog-pressed-state='" << html_escape(pressed_state) << "'";
    html << " data-frog-transition-state='" << html_escape(transition_state) << "'";
    html << " data-frog-state-text-visible='" << (state_text_visible ? "true" : "false") << "'";
    html << " data-frog-frame-visible='" << (frame_visible ? "true" : "false") << "'";
    html << " style='position:absolute;left:" << css_px(x) << ";top:" << css_px(y)
         << ";width:" << css_px(width) << ";height:" << css_px(height) << ";"
         << "--boolean-fill:" << html_escape(state_fill) << ";"
         << "--boolean-hover-fill:" << html_escape(hover_fill) << ";"
         << "--boolean-pressed-fill:" << html_escape(pressed_fill) << ";"
         << "--boolean-border:" << html_escape(state_border) << ";"
         << "--boolean-hover-border:" << html_escape(hover_border) << ";"
         << "--boolean-pressed-border:" << html_escape(pressed_border) << ";"
         << "--boolean-inner-border:" << html_escape(state_inner_border) << ";"
         << "--boolean-hover-inner-border:" << html_escape(hover_inner_border) << ";"
         << "--boolean-pressed-inner-border:" << html_escape(pressed_inner_border) << ";"
         << "--boolean-inner-left:" << html_escape(inner_left) << ";"
         << "--boolean-inner-top:" << html_escape(inner_top) << ";"
         << "--boolean-inner-width:" << html_escape(inner_width) << ";"
         << "--boolean-inner-height:" << html_escape(inner_height) << ";"
         << "--boolean-text:" << html_escape(text_color) << ";"
         << "--boolean-focus-color:" << html_escape(focus_color) << ";"
         << "--boolean-focus-width:" << html_escape(focus_width) << ";"
         << "--boolean-transition:" << html_escape(transition_ms) << "ms " << html_escape(transition_timing) << ";"
         << "--boolean-pressed-inset:" << html_escape(pressed_inset) << ";";
    if (!property_bool(widget.properties, "visible", true)) {
        html << "display:none;";
    }
    html << "'>";

    html << "<span class='boolean-state-face' data-frog-part='inner_face' aria-hidden='true'></span>";
    if (!route.empty()) {
        html << "<img class='boolean-skin' src='" << html_escape(route) << "' alt='' aria-hidden='true' />";
    } else {
        html << "<div class='boolean-skin missing-skin'></div>";
    }
    html << "<span class='boolean-caption-overlay' data-frog-part='caption' data-svg-anchor='caption.anchor' style='"
         << caption_anchor_style(widget.properties, geometry) << "'>" << html_escape(caption) << "</span>";
    if (state_text_visible) {
        html << "<span class='boolean-state-overlay' data-frog-part='state_text'>" << html_escape(state_text) << "</span>";
    }
    html << (is_control ? "</button>" : "</section>");
    return html.str();
}

struct Request {
    std::string method;
    std::string path;
    std::string body;
};

void send_all(socket_t client, const std::string& payload) {
    std::size_t sent = 0;
    while (sent < payload.size()) {
#ifdef _WIN32
        const int count = send(client, payload.data() + static_cast<int>(sent), static_cast<int>(payload.size() - sent), 0);
#else
        const ssize_t count = send(client, payload.data() + sent, payload.size() - sent, 0);
#endif
        if (count <= 0) {
            throw std::runtime_error("Failed to write to socket.");
        }
        sent += static_cast<std::size_t>(count);
    }
}

std::string receive_request(socket_t client) {
    std::string buffer;
    char chunk[4096];
    while (true) {
#ifdef _WIN32
        const int count = recv(client, chunk, sizeof(chunk), 0);
#else
        const ssize_t count = recv(client, chunk, sizeof(chunk), 0);
#endif
        if (count <= 0) {
            break;
        }
        buffer.append(chunk, chunk + count);
        if (buffer.find("\r\n\r\n") != std::string::npos) {
            const auto header_end = buffer.find("\r\n\r\n");
            const auto content_length_pos = buffer.find("Content-Length:");
            std::size_t content_length = 0;
            if (content_length_pos != std::string::npos) {
                const auto line_end = buffer.find("\r\n", content_length_pos);
                const auto value_begin = content_length_pos + std::string("Content-Length:").size();
                const auto raw_value = buffer.substr(value_begin, line_end - value_begin);
                content_length = static_cast<std::size_t>(std::stoul(raw_value));
            }
            if (buffer.size() >= header_end + 4 + content_length) {
                break;
            }
        }
    }
    return buffer;
}

Request parse_request(const std::string& raw) {
    const auto header_end = raw.find("\r\n\r\n");
    if (header_end == std::string::npos) {
        throw std::runtime_error("Malformed HTTP request.");
    }
    std::istringstream stream(raw.substr(0, header_end));
    Request request;
    stream >> request.method >> request.path;
    request.body = raw.substr(header_end + 4);
    return request;
}

void send_response(
    socket_t client,
    const std::string& status,
    const std::string& content_type,
    const std::string& body,
    const std::optional<std::pair<std::string, std::string>>& extra_header = std::nullopt) {
    std::ostringstream headers;
    headers << "HTTP/1.1 " << status << "\r\n";
    headers << "Content-Type: " << content_type << "\r\n";
    headers << "Content-Length: " << body.size() << "\r\n";
    headers << "Connection: close\r\n";
    if (extra_header.has_value()) {
        headers << extra_header->first << ": " << extra_header->second << "\r\n";
    }
    headers << "\r\n" << body;
    send_all(client, headers.str());
}

std::string url_decode(const std::string& input) {
    std::string output;
    output.reserve(input.size());
    for (std::size_t index = 0; index < input.size(); ++index) {
        const char ch = input[index];
        if (ch == '+') {
            output.push_back(' ');
        } else if (ch == '%' && index + 2 < input.size()) {
            const std::string hex = input.substr(index + 1, 2);
            output.push_back(static_cast<char>(std::stoi(hex, nullptr, 16)));
            index += 2;
        } else {
            output.push_back(ch);
        }
    }
    return output;
}

std::optional<std::string> parse_form_value(const std::string& body, const std::string& key) {
    std::size_t start = 0;
    while (start <= body.size()) {
        const auto end = body.find('&', start);
        const auto pair = body.substr(start, end == std::string::npos ? std::string::npos : end - start);
        const auto separator = pair.find('=');
        const std::string current_key = pair.substr(0, separator);
        const std::string current_value = separator == std::string::npos ? "" : pair.substr(separator + 1);
        if (current_key == key) {
            return url_decode(current_value);
        }
        if (end == std::string::npos) {
            break;
        }
        start = end + 1;
    }
    return std::nullopt;
}

void open_in_browser(const std::string& url) {
#ifdef _WIN32
    ShellExecuteA(nullptr, "open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#elif __APPLE__
    std::string command = "open \"" + url + "\" >/dev/null 2>&1 &";
    std::system(command.c_str());
#else
    std::string command = "xdg-open \"" + url + "\" >/dev/null 2>&1 &";
    std::system(command.c_str());
#endif
}

} // namespace

BrowserUiRuntime::BrowserUiRuntime(
    std::optional<std::filesystem::path> contract_path,
    std::optional<std::filesystem::path> wfrog_path,
    std::shared_ptr<const NativeKernelBridge> native_kernel_bridge_)
    : core(contract_path.value_or(default_contract_path()), wfrog_path.value_or(default_wfrog_path())),
      native_kernel_bridge(std::move(native_kernel_bridge_)) {}

frog::json::Value BrowserUiRuntime::run_once(std::uint16_t input_value) {
    try {
        frog::json::Value artifact = native_kernel_bridge == nullptr
            ? core.execute(input_value)
            : core.execute_with_native_kernel_bridge(*native_kernel_bridge, input_value);
        last_error.reset();
        return artifact;
    } catch (const std::exception& error) {
        last_error = error.what();
        throw;
    }
}

std::string BrowserUiRuntime::render_html() const {
    const auto& ctrl = core.widgets.at("ctrl_input");
    const auto& ind = core.widgets.at("ind_result");
    const auto snapshot = frog::json::stringify(core.execution_artifact(), true, 2);
    const auto panel_width = layout_i64(core.panel.layout, "width", 460);
    const auto panel_height = layout_i64(core.panel.layout, "height", 170);
    const bool uses_native_kernel = native_kernel_bridge != nullptr;

    std::string diagnostics;
    if (last_error.has_value()) {
        diagnostics = "<div class='diagnostic error'>" + html_escape(*last_error) + "</div>";
    }

    std::ostringstream html;
    html << "<!doctype html><html lang='en'><head><meta charset='utf-8'><title>" << html_escape(core.panel.title) << "</title>";
    html << "<style>"
            "body{font-family:Segoe UI,Arial,sans-serif;margin:24px;background:#f3f6f8;color:#1f2933;}"
            "h1{margin:0 0 12px 0;font-size:24px;}"
            "p.meta{margin:0 0 20px 0;color:#52606d;}"
            ".runtime-facts{display:flex;flex-wrap:wrap;gap:8px;margin:-8px 0 18px 0;}"
            ".runtime-facts div{display:flex;gap:6px;align-items:baseline;padding:6px 8px;border:1px solid #d9e2ec;border-radius:6px;background:#ffffff;}"
            ".runtime-facts dt{margin:0;color:#52606d;font-size:11px;font-weight:700;text-transform:uppercase;}"
            ".runtime-facts dd{margin:0;color:#1f2933;font-size:12px;font-weight:600;}"
            ".front-panel{position:relative;background:#ffffff;border-radius:10px;box-shadow:0 4px 14px rgba(15,23,42,0.08);overflow:hidden;}"
            ".frog-widget{position:absolute;box-sizing:border-box;}"
            ".numeric-widget{font-family:Segoe UI,Arial,sans-serif;}"
            ".numeric-skin{position:absolute;inset:0;width:100%;height:100%;display:block;}"
            ".numeric-skin svg{width:100%;height:100%;display:block;}"
            ".missing-skin{background:#e5e7eb;border:1px solid #9ca3af;border-radius:6px;}"
            ".numeric-label-overlay{position:absolute;transform:translateY(-50%);font-size:12px;line-height:1;white-space:nowrap;pointer-events:none;}"
            ".numeric-value-overlay{position:absolute;box-sizing:border-box;font-family:Consolas,Segoe UI Mono,monospace;font-size:11px;font-weight:700;line-height:1;border:0;background:transparent;}"
            ".numeric-control-editor{padding:0 4px;border-radius:0;outline:0;background:transparent;appearance:textfield;-moz-appearance:textfield;}"
            ".numeric-control-editor::-webkit-outer-spin-button,.numeric-control-editor::-webkit-inner-spin-button{appearance:none;margin:0;}"
            ".numeric-control-editor:focus{outline:0;background:transparent;}"
            ".numeric-indicator-value{display:flex;align-items:center;padding:0 4px;pointer-events:none;}"
            ".numeric-step-overlay{position:absolute;box-sizing:border-box;padding:0;border:1px solid var(--frog-numeric-step-border);border-radius:0;background:var(--frog-numeric-step-fill);color:transparent;cursor:pointer;display:flex;align-items:center;justify-content:center;}"
            ".numeric-step-overlay:focus{outline:0;}"
            ".numeric-step-overlay:active{background:var(--frog-numeric-step-fill-pressed);border-color:var(--frog-numeric-step-border-pressed);}"
            ".numeric-step-overlay::before{content:'';display:block;width:0;height:0;}"
            ".numeric-increment::before{border-left:5px solid transparent;border-right:5px solid transparent;border-bottom:6px solid var(--frog-numeric-step-symbol);}"
            ".numeric-decrement::before{border-left:5px solid transparent;border-right:5px solid transparent;border-top:6px solid var(--frog-numeric-step-symbol);}"
            ".numeric-increment:active::before{border-bottom-color:var(--frog-numeric-step-symbol-pressed);}"
            ".numeric-decrement:active::before{border-top-color:var(--frog-numeric-step-symbol-pressed);}"
            ".actions{margin-top:16px;display:flex;gap:12px;align-items:center;}"
            "button{padding:8px 14px;border:0;border-radius:6px;cursor:pointer;background:#0f62fe;color:#ffffff;font-weight:600;}"
            ".numeric-step-overlay{padding:0;border-radius:0;color:transparent;font-weight:400;}"
            ".diagnostic{margin:12px 0;padding:10px 12px;border-radius:6px;}"
            ".diagnostic.error{background:#fff1f2;color:#9f1239;border:1px solid #fecdd3;}"
            "summary{cursor:pointer;margin-top:16px;font-weight:600;}"
            "pre{white-space:pre-wrap;word-break:break-word;background:#0b1020;color:#dbeafe;padding:12px;border-radius:8px;font-size:12px;}"
            "</style><script>"
            "document.addEventListener('click',function(event){"
            "const button=event.target.closest('.numeric-step-overlay');"
            "if(!button)return;"
            "const input=document.getElementById(button.dataset.target);"
            "if(!input||input.disabled)return;"
            "const step=Number(button.dataset.step||'1');"
            "const min=Number(input.min||'0');"
            "const max=Number(input.max||'65535');"
            "const next=Math.min(max,Math.max(min,Number(input.value||'0')+step));"
            "input.value=String(next);"
            "input.dispatchEvent(new Event('input',{bubbles:true}));"
            "});"
            "</script></head><body>";
    html << "<h1>" << html_escape(core.panel.title) << "</h1>";
    html << "<p class='meta'>Example 05 - .wfrog front panel + C++ runtime</p>";
    html << "<dl class='runtime-facts' aria-label='Runtime facts'>";
    html << "<div><dt>Runtime</dt><dd>C++ reference runtime</dd></div>";
    html << "<div><dt>Execution</dt><dd>" << (uses_native_kernel ? "native kernel bridge" : "contract executor") << "</dd></div>";
    html << "<div><dt>Compiler backend</dt><dd>" << (uses_native_kernel ? "LLVM native kernel artifact" : "none in runtime path") << "</dd></div>";
    html << "</dl>";
    html << diagnostics;
    html << "<form method='post' action='/run'>";
    html << "<div class='front-panel' data-panel-id='" << html_escape(core.panel.panel_id) << "' data-coordinate-space='panel_pixels'";
    html << " data-runtime-language='cpp'";
    html << " data-compiler-backend='" << (uses_native_kernel ? "llvm" : "none") << "'";
    html << " data-execution-path='" << (uses_native_kernel ? "native_kernel_bridge" : "contract_executor") << "'";
    html << " style='width:" << css_px(panel_width) << ";height:" << css_px(panel_height) << ";'>";
    html << render_numeric_widget(ctrl);
    html << render_numeric_widget(ind);
    html << "</div>";
    html << "<div class='actions'><button type='submit'>Run Example 05</button><a href='/state.json'>state.json</a></div>";
    html << "</form><details><summary>Current runtime snapshot</summary><pre>" << html_escape(snapshot) << "</pre></details>";
    html << "</body></html>";
    return html.str();
}

void BrowserUiRuntime::serve(const std::string& host, std::uint16_t port, bool should_open_browser) {
    NetworkBootstrap network_bootstrap;
    (void)network_bootstrap;

    socket_t server = ::socket(AF_INET, SOCK_STREAM, 0);
    if (server == invalid_socket) {
        throw std::runtime_error("Unable to create server socket.");
    }

#ifndef _WIN32
    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    if (inet_pton(AF_INET, host.c_str(), &address.sin_addr) != 1) {
        close_socket(server);
        throw std::runtime_error("Only numeric IPv4 host values are supported by this minimal runtime.");
    }

    if (bind(server, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0) {
        close_socket(server);
        throw std::runtime_error("Unable to bind server socket.");
    }
    if (listen(server, 16) != 0) {
        close_socket(server);
        throw std::runtime_error("Unable to listen on server socket.");
    }

    sockaddr_in bound_address{};
#ifdef _WIN32
    int bound_length = sizeof(bound_address);
#else
    socklen_t bound_length = sizeof(bound_address);
#endif
    if (getsockname(server, reinterpret_cast<sockaddr*>(&bound_address), &bound_length) != 0) {
        close_socket(server);
        throw std::runtime_error("Unable to inspect bound server socket.");
    }

    const std::string url = "http://" + host + ":" + std::to_string(ntohs(bound_address.sin_port)) + "/";
    std::cout << url << std::endl;
    if (should_open_browser) {
        open_in_browser(url);
    }

    while (true) {
        socket_t client = accept(server, nullptr, nullptr);
        if (client == invalid_socket) {
            continue;
        }
        try {
            const auto raw = receive_request(client);
            const auto request = parse_request(raw);
            if (request.method == "GET" && request.path == "/") {
                send_response(client, "200 OK", "text/html; charset=utf-8", render_html());
            } else if (request.method == "GET" && request.path == "/state.json") {
                send_response(client, "200 OK", "application/json; charset=utf-8", frog::json::stringify(core.execution_artifact(), true, 2));
            } else if (request.method == "GET" && request.path.rfind("/asset/", 0) == 0) {
                const std::string asset_id = request.path.substr(std::string("/asset/").size());
                const auto asset_it = core.asset_map.find(asset_id);
                if (asset_it == core.asset_map.end() || !std::filesystem::exists(asset_it->second)) {
                    send_response(client, "404 Not Found", "text/plain; charset=utf-8", "missing asset");
                } else {
                    send_response(client, "200 OK", "image/svg+xml", read_text_file(asset_it->second));
                }
            } else if (request.method == "POST" && request.path == "/run") {
                try {
                    const auto form_value = parse_form_value(request.body, "input_value").value_or("0");
                    run_once(parse_u16_form_value(form_value));
                } catch (const std::exception& error) {
                    last_error = error.what();
                }
                send_response(client, "303 See Other", "text/plain; charset=utf-8", "", std::make_pair(std::string("Location"), std::string("/")));
            } else {
                send_response(client, "404 Not Found", "text/plain; charset=utf-8", "not found");
            }
        } catch (const std::exception& error) {
            try {
                send_response(client, "500 Internal Server Error", "text/plain; charset=utf-8", error.what());
            } catch (...) {
            }
        }
        close_socket(client);
    }
}

BooleanBrowserUiRuntime::BooleanBrowserUiRuntime(
    std::filesystem::path contract_path,
    std::filesystem::path wfrog_path,
    std::shared_ptr<const NativeBoolKernelBridge> native_kernel_bridge_)
    : core(std::move(contract_path), std::move(wfrog_path)),
      native_kernel_bridge(std::move(native_kernel_bridge_)) {}

frog::json::Value BooleanBrowserUiRuntime::run_once(bool input_value) {
    try {
        frog::json::Value artifact = native_kernel_bridge == nullptr
            ? core.execute(input_value)
            : core.execute_with_native_kernel_bridge(*native_kernel_bridge, input_value);
        last_error.reset();
        return artifact;
    } catch (const std::exception& error) {
        last_error = error.what();
        throw;
    }
}

std::string BooleanBrowserUiRuntime::render_html() const {
    const auto& ctrl = core.widgets.at("bool_input");
    const auto& ind = core.widgets.at("bool_result");
    const auto panel_width = layout_i64(core.panel.layout, "width", 420);
    const auto panel_height = layout_i64(core.panel.layout, "height", 150);
    const bool uses_native_kernel = native_kernel_bridge != nullptr;

    std::string diagnostics;
    if (last_error.has_value()) {
        diagnostics = "<div class='diagnostic error'>" + html_escape(*last_error) + "</div>";
    }

    std::ostringstream html;
    html << "<!doctype html><html lang='en'><head><meta charset='utf-8'><title>" << html_escape(core.panel.title) << "</title>";
    html << "<style>"
            "body{font-family:Segoe UI,Arial,sans-serif;margin:24px;background:#f3f6f8;color:#1f2933;}"
            "h1{font-size:24px;margin:0 0 12px 0;}"
            "p.meta{margin:0 0 20px 0;color:#52606d;}"
            ".runtime-facts{display:flex;flex-wrap:wrap;gap:8px;margin:-8px 0 18px 0;}"
            ".runtime-facts div{display:flex;gap:6px;align-items:baseline;padding:6px 8px;border:1px solid #d9e2ec;border-radius:6px;background:#ffffff;}"
            ".runtime-facts dt{margin:0;color:#52606d;font-size:11px;font-weight:700;text-transform:uppercase;}"
            ".runtime-facts dd{margin:0;color:#1f2933;font-size:12px;font-weight:600;}"
            ".front-panel{position:relative;background:#ffffff;border-radius:10px;box-shadow:0 4px 14px rgba(15,23,42,0.08);overflow:hidden;}"
            ".frog-widget{position:absolute;box-sizing:border-box;}"
            ".boolean-widget{border:0;padding:0;background:transparent;font:inherit;color:inherit;overflow:visible;}"
            ".boolean-control{cursor:pointer;}"
            ".boolean-indicator{pointer-events:none;}"
            ".boolean-skin{position:absolute;inset:0;width:100%;height:100%;object-fit:fill;display:block;pointer-events:none;z-index:2;}"
            ".missing-skin{background:#e5e7eb;border:1px solid #9ca3af;border-radius:6px;}"
            ".boolean-caption-overlay{position:absolute;left:0;top:0;transform:translateY(-50%);text-align:left;font-size:14px;font-weight:600;line-height:1;color:#1f2933;white-space:nowrap;pointer-events:none;z-index:3;}"
            ".boolean-state-face{position:absolute;left:var(--boolean-inner-left);top:var(--boolean-inner-top);width:var(--boolean-inner-width);height:var(--boolean-inner-height);border:2px solid var(--boolean-inner-border);border-radius:7px;background:var(--boolean-fill);box-shadow:inset 0 1px 0 rgba(255,255,255,.6),0 1px 2px rgba(15,23,42,.16);transition:background var(--boolean-transition),border-color var(--boolean-transition),box-shadow var(--boolean-transition),transform var(--boolean-transition);z-index:1;}"
            ".boolean-widget[data-realization-variant='circular'] .boolean-state-face{border-radius:50%;}"
            ".boolean-widget[data-frog-frame-visible='false'] .boolean-state-face{box-shadow:none;}"
            ".boolean-control:hover .boolean-state-face{background:var(--boolean-hover-fill);border-color:var(--boolean-hover-inner-border);box-shadow:inset 0 1px 0 rgba(255,255,255,.72),0 2px 5px rgba(15,23,42,.18);}"
            ".boolean-control[data-frog-frame-visible='false']:hover .boolean-state-face{box-shadow:none;}"
            ".boolean-control:active .boolean-state-face{background:var(--boolean-pressed-fill);border-color:var(--boolean-pressed-inner-border);box-shadow:inset 0 2px 4px rgba(15,23,42,.22);transform:translateY(var(--boolean-pressed-inset));}"
            ".boolean-control[data-frog-frame-visible='false']:active .boolean-state-face{box-shadow:none;}"
            ".boolean-control:focus-visible .boolean-state-face{outline:var(--boolean-focus-width) solid var(--boolean-focus-color);}"
            ".boolean-state-overlay{position:absolute;left:0;right:0;top:49px;transform:translateY(-50%);text-align:center;font-size:18px;font-weight:700;line-height:1;color:var(--boolean-text);pointer-events:none;z-index:4;}"
            ".actions{margin-top:16px;display:flex;gap:12px;align-items:center;}"
            ".state-link{font-size:16px;}"
            ".diagnostic{margin:12px 0;padding:10px 12px;border-radius:6px;}"
            ".diagnostic.error{background:#fff1f2;color:#9f1239;border:1px solid #fecdd3;}"
            "</style></head><body>";
    html << "<h1>" << html_escape(core.panel.title) << "</h1>";
    html << "<p class='meta'>Example 06 - .wfrog front panel + Default Boolean realization assets + C++ runtime</p>";
    html << "<dl class='runtime-facts' aria-label='Runtime facts'>";
    html << "<div><dt>Runtime</dt><dd>C++ reference runtime</dd></div>";
    html << "<div><dt>Execution</dt><dd>" << (uses_native_kernel ? "native kernel bridge" : "boolean contract executor") << "</dd></div>";
    html << "<div><dt>Compiler backend</dt><dd>" << (uses_native_kernel ? "LLVM native bool kernel artifact" : "none for Example 06") << "</dd></div>";
    html << "</dl>";
    html << diagnostics;
    html << "<form method='post' action='/run'>";
    html << "<div class='front-panel' data-panel-id='" << html_escape(core.panel.panel_id)
         << "' data-coordinate-space='panel_pixels' data-runtime-language='cpp'";
    html << " data-compiler-backend='" << (uses_native_kernel ? "llvm" : "none") << "'";
    html << " data-execution-path='" << (uses_native_kernel ? "native_kernel_bridge" : "cpp_boolean_contract_executor") << "'";
    html << " style='width:" << css_px(panel_width) << ";height:" << css_px(panel_height) << ";'>";
    html << render_boolean_widget(ctrl);
    html << render_boolean_widget(ind);
    html << "</div><div class='actions'><a class='state-link' href='/state.json'>state.json</a></div></form></body></html>";
    return html.str();
}

void BooleanBrowserUiRuntime::serve(const std::string& host, std::uint16_t port, bool should_open_browser) {
    NetworkBootstrap network_bootstrap;
    (void)network_bootstrap;

    socket_t server = ::socket(AF_INET, SOCK_STREAM, 0);
    if (server == invalid_socket) {
        throw std::runtime_error("Unable to create server socket.");
    }

#ifndef _WIN32
    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    if (inet_pton(AF_INET, host.c_str(), &address.sin_addr) != 1) {
        close_socket(server);
        throw std::runtime_error("Only numeric IPv4 host values are supported by this minimal runtime.");
    }

    if (bind(server, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0) {
        close_socket(server);
        throw std::runtime_error("Unable to bind server socket.");
    }
    if (listen(server, 16) != 0) {
        close_socket(server);
        throw std::runtime_error("Unable to listen on server socket.");
    }

    sockaddr_in bound_address{};
#ifdef _WIN32
    int bound_length = sizeof(bound_address);
#else
    socklen_t bound_length = sizeof(bound_address);
#endif
    if (getsockname(server, reinterpret_cast<sockaddr*>(&bound_address), &bound_length) != 0) {
        close_socket(server);
        throw std::runtime_error("Unable to inspect bound server socket.");
    }

    const std::string url = "http://" + host + ":" + std::to_string(ntohs(bound_address.sin_port)) + "/";
    std::cout << url << std::endl;
    if (should_open_browser) {
        open_in_browser(url);
    }

    while (true) {
        socket_t client = accept(server, nullptr, nullptr);
        if (client == invalid_socket) {
            continue;
        }
        try {
            const auto raw = receive_request(client);
            const auto request = parse_request(raw);
            if (request.method == "GET" && request.path == "/") {
                send_response(client, "200 OK", "text/html; charset=utf-8", render_html());
            } else if (request.method == "GET" && request.path == "/state.json") {
                send_response(client, "200 OK", "application/json; charset=utf-8", frog::json::stringify(core.execution_artifact(), true, 2));
            } else if (request.method == "GET" && request.path.rfind("/asset/", 0) == 0) {
                const std::string asset_id = request.path.substr(std::string("/asset/").size());
                const auto asset_it = core.asset_map.find(asset_id);
                if (asset_it == core.asset_map.end() || !std::filesystem::exists(asset_it->second)) {
                    send_response(client, "404 Not Found", "text/plain; charset=utf-8", "missing asset");
                } else {
                    send_response(client, "200 OK", "image/svg+xml", read_text_file(asset_it->second));
                }
            } else if (request.method == "POST" && request.path == "/run") {
                try {
                    const auto form_value = parse_form_value(request.body, "input_value").value_or("false");
                    run_once(parse_bool_form_value(form_value));
                } catch (const std::exception& error) {
                    last_error = error.what();
                }
                send_response(client, "303 See Other", "text/plain; charset=utf-8", "", std::make_pair(std::string("Location"), std::string("/")));
            } else {
                send_response(client, "404 Not Found", "text/plain; charset=utf-8", "not found");
            }
        } catch (const std::exception& error) {
            try {
                send_response(client, "500 Internal Server Error", "text/plain; charset=utf-8", error.what());
            } catch (...) {
            }
        }
        close_socket(client);
    }
}

StringBrowserUiRuntime::StringBrowserUiRuntime(
    std::filesystem::path contract_path,
    std::filesystem::path wfrog_path,
    std::shared_ptr<const NativeStringKernelBridge> native_kernel_bridge_)
    : core(std::move(contract_path), std::move(wfrog_path)),
      native_kernel_bridge(std::move(native_kernel_bridge_)) {}

frog::json::Value StringBrowserUiRuntime::run_once(const std::string& input_value) {
    try {
        frog::json::Value artifact = native_kernel_bridge == nullptr
            ? core.execute(input_value)
            : core.execute_with_native_kernel_bridge(*native_kernel_bridge, input_value);
        last_error.reset();
        return artifact;
    } catch (const std::exception& error) {
        last_error = error.what();
        throw;
    }
}

std::string StringBrowserUiRuntime::render_html() const {
    const auto& ctrl = core.widgets.at("str_input");
    const auto& ind = core.widgets.at("str_result");
    const auto panel_width = layout_i64(core.panel.layout, "width", 560);
    const auto panel_height = layout_i64(core.panel.layout, "height", 170);
    const bool uses_native_kernel = native_kernel_bridge != nullptr;

    std::string diagnostics;
    if (last_error.has_value()) {
        diagnostics = "<div class='diagnostic error'>" + html_escape(*last_error) + "</div>";
    }

    std::ostringstream html;
    html << "<!doctype html><html lang='en'><head><meta charset='utf-8'><title>" << html_escape(core.panel.title) << "</title>";
    html << "<style>"
            "body{font-family:Segoe UI,Arial,sans-serif;margin:24px;background:#f3f6f8;color:#1f2933;}"
            "h1{font-size:24px;margin:0 0 12px 0;}"
            "p.meta{margin:0 0 20px 0;color:#52606d;}"
            ".runtime-facts{display:flex;flex-wrap:wrap;gap:8px;margin:-8px 0 18px 0;}"
            ".runtime-facts div{display:flex;gap:6px;align-items:baseline;padding:6px 8px;border:1px solid #d9e2ec;border-radius:6px;background:#ffffff;}"
            ".runtime-facts dt{margin:0;color:#52606d;font-size:11px;font-weight:700;text-transform:uppercase;}"
            ".runtime-facts dd{margin:0;color:#1f2933;font-size:12px;font-weight:600;}"
            ".front-panel{position:relative;background:#ffffff;border-radius:10px;box-shadow:0 4px 14px rgba(15,23,42,0.08);overflow:hidden;}"
            ".frog-widget{position:absolute;box-sizing:border-box;}"
            ".string-widget{font-family:Segoe UI,Arial,sans-serif;}"
            ".string-skin{position:absolute;inset:0;width:100%;height:100%;display:block;}"
            ".string-skin svg{width:100%;height:100%;display:block;--frog-string-label-display:inherit;--frog-string-caption-display:inherit;--frog-string-placeholder-display:inherit;--frog-string-frame-fill:inherit;--frog-string-frame-stroke:inherit;--frog-string-frame-stroke-width:inherit;--frog-string-text-region-fill:inherit;--frog-string-text-region-stroke:inherit;--frog-string-text-region-stroke-width:inherit;--frog-string-text-fill:inherit;--frog-string-text-font-size:inherit;--frog-string-text-font-weight:inherit;}"
            ".string-skin #label_text,.string-skin #caption_text,.string-skin #placeholder,.string-skin #text_value{display:none;}"
            ".string-control:hover .string-skin svg{--frog-string-text-region-fill:var(--frog-string-text-region-fill-hover);--frog-string-text-region-stroke:var(--frog-string-text-region-stroke-hover);--frog-string-text-region-stroke-width:var(--frog-string-text-region-stroke-width-hover);}"
            ".string-caption-overlay{position:absolute;transform:translateY(-50%);font-size:14px;font-weight:600;line-height:1;white-space:nowrap;pointer-events:none;}"
            ".string-value-overlay{position:absolute;box-sizing:border-box;font-family:Segoe UI,Arial,sans-serif;line-height:1.2;border:0;background:transparent;}"
            ".string-control-editor{padding:0 8px;outline:0;}"
            ".string-control-editor:focus{outline:0;}"
            ".string-indicator-value{display:flex;align-items:center;padding:0 8px;pointer-events:none;}"
            ".actions{margin-top:16px;display:flex;gap:12px;align-items:center;}"
            "button{padding:8px 14px;border:0;border-radius:6px;cursor:pointer;background:#0f62fe;color:#ffffff;font-weight:600;}"
            ".diagnostic{margin:12px 0;padding:10px 12px;border-radius:6px;}"
            ".diagnostic.error{background:#fff1f2;color:#9f1239;border:1px solid #fecdd3;}"
            "</style></head><body>";
    html << "<h1>" << html_escape(core.panel.title) << "</h1>";
    html << "<p class='meta'>Example 07 - .wfrog front panel + Default String realization assets + C++ runtime</p>";
    html << "<dl class='runtime-facts' aria-label='Runtime facts'>";
    html << "<div><dt>Runtime</dt><dd>C++ reference runtime</dd></div>";
    html << "<div><dt>Execution</dt><dd>" << (uses_native_kernel ? "native kernel bridge" : "string contract executor") << "</dd></div>";
    html << "<div><dt>Compiler backend</dt><dd>" << (uses_native_kernel ? "LLVM native string kernel artifact" : "none for Example 07") << "</dd></div>";
    html << "</dl>";
    html << diagnostics;
    html << "<form method='post' action='/run'>";
    html << "<div class='front-panel' data-panel-id='" << html_escape(core.panel.panel_id)
         << "' data-coordinate-space='panel_pixels' data-runtime-language='cpp'";
    html << " data-compiler-backend='" << (uses_native_kernel ? "llvm" : "none") << "'";
    html << " data-execution-path='" << (uses_native_kernel ? "native_kernel_bridge" : "cpp_string_contract_executor") << "'";
    html << " style='width:" << css_px(panel_width) << ";height:" << css_px(panel_height) << ";'>";
    html << render_string_widget(ctrl);
    html << render_string_widget(ind);
    html << "</div><div class='actions'><button type='submit'>Run Example 07</button><a class='state-link' href='/state.json'>state.json</a></div></form></body></html>";
    return html.str();
}

void StringBrowserUiRuntime::serve(const std::string& host, std::uint16_t port, bool should_open_browser) {
    NetworkBootstrap network_bootstrap;
    (void)network_bootstrap;

    socket_t server = ::socket(AF_INET, SOCK_STREAM, 0);
    if (server == invalid_socket) {
        throw std::runtime_error("Unable to create server socket.");
    }

#ifndef _WIN32
    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    if (inet_pton(AF_INET, host.c_str(), &address.sin_addr) != 1) {
        close_socket(server);
        throw std::runtime_error("Only numeric IPv4 host values are supported by this minimal runtime.");
    }

    if (bind(server, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0) {
        close_socket(server);
        throw std::runtime_error("Unable to bind server socket.");
    }
    if (listen(server, 16) != 0) {
        close_socket(server);
        throw std::runtime_error("Unable to listen on server socket.");
    }

    sockaddr_in bound_address{};
#ifdef _WIN32
    int bound_length = sizeof(bound_address);
#else
    socklen_t bound_length = sizeof(bound_address);
#endif
    if (getsockname(server, reinterpret_cast<sockaddr*>(&bound_address), &bound_length) != 0) {
        close_socket(server);
        throw std::runtime_error("Unable to inspect bound server socket.");
    }

    const std::string url = "http://" + host + ":" + std::to_string(ntohs(bound_address.sin_port)) + "/";
    std::cout << url << std::endl;
    if (should_open_browser) {
        open_in_browser(url);
    }

    while (true) {
        socket_t client = accept(server, nullptr, nullptr);
        if (client == invalid_socket) {
            continue;
        }
        try {
            const auto raw = receive_request(client);
            const auto request = parse_request(raw);
            if (request.method == "GET" && request.path == "/") {
                send_response(client, "200 OK", "text/html; charset=utf-8", render_html());
            } else if (request.method == "GET" && request.path == "/state.json") {
                send_response(client, "200 OK", "application/json; charset=utf-8", frog::json::stringify(core.execution_artifact(), true, 2));
            } else if (request.method == "GET" && request.path.rfind("/asset/", 0) == 0) {
                const std::string asset_id = request.path.substr(std::string("/asset/").size());
                const auto asset_it = core.asset_map.find(asset_id);
                if (asset_it == core.asset_map.end() || !std::filesystem::exists(asset_it->second)) {
                    send_response(client, "404 Not Found", "text/plain; charset=utf-8", "missing asset");
                } else {
                    send_response(client, "200 OK", "image/svg+xml", read_text_file(asset_it->second));
                }
            } else if (request.method == "POST" && request.path == "/run") {
                try {
                    const auto form_value = parse_form_value(request.body, "input_text").value_or("hello world");
                    run_once(form_value);
                } catch (const std::exception& error) {
                    last_error = error.what();
                }
                send_response(client, "303 See Other", "text/plain; charset=utf-8", "", std::make_pair(std::string("Location"), std::string("/")));
            } else {
                send_response(client, "404 Not Found", "text/plain; charset=utf-8", "not found");
            }
        } catch (const std::exception& error) {
            try {
                send_response(client, "500 Internal Server Error", "text/plain; charset=utf-8", error.what());
            } catch (...) {
            }
        }
        close_socket(client);
    }
}

} // namespace frog::runtime
