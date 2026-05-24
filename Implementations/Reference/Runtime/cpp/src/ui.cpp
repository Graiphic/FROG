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
#include <vector>

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

std::string safe_css_signed_length(const std::string& value, const std::string& fallback) {
    if (value.size() <= 2 || value.substr(value.size() - 2) != "px") {
        return fallback;
    }
    const auto number = value.substr(0, value.size() - 2);
    bool saw_digit = false;
    bool saw_dot = false;
    for (std::size_t index = 0; index < number.size(); ++index) {
        const auto ch = number[index];
        if (index == 0 && ch == '-') {
            continue;
        }
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

std::string format_css_px(double value) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(3) << value;
    auto text = out.str();
    while (!text.empty() && text.back() == '0') {
        text.pop_back();
    }
    if (!text.empty() && text.back() == '.') {
        text.pop_back();
    }
    if (text == "-0") {
        text = "0";
    }
    return text + "px";
}

std::string scale_css_px_length(const std::string& value, double scale) {
    const auto number = value.substr(0, value.size() - 2);
    return format_css_px(std::stod(number) * scale);
}

std::string maybe_scaled_css_length(
    const Object& properties,
    const std::string& key,
    const std::string& mode_key,
    const std::string& fallback,
    double scale,
    bool allow_signed = false) {
    const auto safe_value = allow_signed
        ? safe_css_signed_length(property_string(properties, key, fallback), fallback)
        : safe_css_length(property_string(properties, key, fallback), fallback);
    const auto mode = property_string(properties, mode_key, "fixed_css_px");
    return mode == "scale_with_widget" ? scale_css_px_length(safe_value, scale) : safe_value;
}

std::string safe_css_font_style(const std::string& value, const std::string& fallback) {
    return value == "normal" || value == "italic" || value == "oblique" ? value : fallback;
}

std::string safe_css_font_family(const std::string& value, const std::string& fallback) {
    if (value.empty()) {
        return fallback;
    }
    for (const auto ch : value) {
        const bool allowed =
            std::isalnum(static_cast<unsigned char>(ch)) != 0 ||
            ch == ' ' || ch == ',' || ch == '-' || ch == '_' || ch == '\'' || ch == '"';
        if (!allowed) {
            return fallback;
        }
    }
    return value;
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
    double value_text_x = 22.0;
    double value_text_y = 96.0;
    double state_text_x = 80.0;
    double state_text_y = 48.0;
    double selector_face_x = 246.0;
    double selector_face_y = 82.0;
    double selector_face_width = 24.0;
    double selector_face_height = 28.0;
    double increment_up_x = 278.0;
    double increment_up_y = 82.0;
    double increment_up_width = 24.0;
    double increment_up_height = 13.0;
    double increment_down_x = 278.0;
    double increment_down_y = 97.0;
    double increment_down_width = 24.0;
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

std::optional<std::size_t> find_svg_element_with_part(const std::string& svg, const std::string& part) {
    const std::string marker = "data-frog-part=\"" + part + "\"";
    const auto marker_pos = svg.find(marker);
    if (marker_pos == std::string::npos) {
        return std::nullopt;
    }
    const auto tag_start = svg.rfind('<', marker_pos);
    const auto tag_end = svg.find('>', marker_pos);
    if (tag_start == std::string::npos || tag_end == std::string::npos || tag_start > tag_end) {
        return std::nullopt;
    }
    return tag_start;
}

std::optional<std::string> svg_part_attribute(const std::string& svg, const std::string& part, const std::string& attribute) {
    const auto tag_start = find_svg_element_with_part(svg, part);
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

double svg_part_attribute_double(const std::string& svg, const std::string& part, const std::string& attribute, double fallback) {
    const auto value = svg_part_attribute(svg, part, attribute);
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
    geometry.state_text_x = svg_part_attribute_double(svg, "state_text", "x", geometry.state_text_x);
    geometry.state_text_y = svg_part_attribute_double(svg, "state_text", "y", geometry.state_text_y);
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

std::string state_text_anchor_style(const Object& properties, const SvgGeometry& geometry) {
    const auto x = property_number(properties, "state_text.anchor.x").value_or(geometry.state_text_x);
    const auto y = property_number(properties, "state_text.anchor.y").value_or(geometry.state_text_y);
    return svg_anchor_style(x, y, geometry);
}

std::string svg_box_style(double x, double y, double width, double height, const SvgGeometry& geometry) {
    std::ostringstream style;
    style << "left:" << css_percent(pct(x, geometry.view_width)) << ";";
    style << "top:" << css_percent(pct(y, geometry.view_height)) << ";";
    style << "width:" << css_percent(pct(width, geometry.view_width)) << ";";
    style << "height:" << css_percent(pct(height, geometry.view_height)) << ";";
    return style.str();
}

std::string svg_dropdown_style(double x, double y, double width, double height, const SvgGeometry& geometry) {
    std::ostringstream style;
    style << "left:" << css_percent(pct(x, geometry.view_width)) << ";";
    style << "top:" << css_percent(pct(y + height, geometry.view_height)) << ";";
    style << "width:" << css_percent(pct(width, geometry.view_width)) << ";";
    return style.str();
}

double widget_style_scale(const Object& properties, std::int64_t width, std::int64_t height) {
    const auto reference_width = property_number(properties, "style.scale.reference_width").value_or(static_cast<double>(width));
    const auto reference_height = property_number(properties, "style.scale.reference_height").value_or(static_cast<double>(height));
    if (reference_width <= 0.0 || reference_height <= 0.0) {
        return 1.0;
    }
    return std::min(static_cast<double>(width) / reference_width, static_cast<double>(height) / reference_height);
}

std::string asset_route(const WidgetState& widget) {
    return widget.asset_id.has_value() ? "/asset/" + *widget.asset_id : std::string();
}

std::string render_numeric_skin(const WidgetState& widget, bool is_control, const std::string& color) {
    if (widget.asset_path.empty() || !std::filesystem::exists(widget.asset_path)) {
        return "<div class='numeric-skin missing-skin'></div>";
    }
    const auto frame_fill = safe_css_color(property_string(widget.properties, "style.frame.fill_color", "transparent"), "transparent");
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
    const auto label_color = safe_css_color(
        property_string(widget.properties, "style.caption.text_color", property_string(widget.properties, "label_color", "#111827")),
        "#111827");
    const auto label_size = safe_css_length(property_string(widget.properties, "style.caption.font_size", "12px"), "12px");
    const auto label_weight = safe_css_font_weight(property_string(widget.properties, "style.caption.font_weight", "400"), "400");
    const auto label_family = safe_css_font_family(
        property_string(widget.properties, "style.caption.font_family", "system-ui, Segoe UI, Arial, sans-serif"),
        "system-ui, Segoe UI, Arial, sans-serif");
    const auto text_color = safe_css_color(property_string(widget.properties, "style.text_value.color", "#111827"), "#111827");
    const auto text_size = safe_css_length(property_string(widget.properties, "style.text_value.font_size", "11px"), "11px");
    const auto text_weight = safe_css_font_weight(property_string(widget.properties, "style.text_value.font_weight", "700"), "700");
    const auto text_family = safe_css_font_family(
        property_string(widget.properties, "style.text_value.font_family", "Consolas, Segoe UI Mono, monospace"),
        "Consolas, Segoe UI Mono, monospace");
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
         << ";height:" << css_px(height) << ";"
         << "--frog-numeric-caption-color:" << html_escape(label_color) << ";"
         << "--frog-numeric-caption-font-size:" << html_escape(label_size) << ";"
         << "--frog-numeric-caption-font-weight:" << html_escape(label_weight) << ";"
         << "--frog-numeric-caption-font-family:" << html_escape(label_family) << ";"
         << "--frog-numeric-text-color:" << html_escape(text_color) << ";"
         << "--frog-numeric-text-font-size:" << html_escape(text_size) << ";"
         << "--frog-numeric-text-font-weight:" << html_escape(text_weight) << ";"
         << "--frog-numeric-text-font-family:" << html_escape(text_family) << ";";
    if (!property_bool(widget.properties, "visible", true)) {
        html << "display:none;";
    }
    html << "'>";

    html << render_numeric_skin(widget, is_control, color);

    html << "<span class='numeric-label-overlay' data-frog-part='caption' data-svg-anchor='caption.anchor' style='"
          << caption_anchor_style(widget.properties, geometry)
          << "'>" << html_escape(label) << "</span>";

    const auto value_style = svg_box_style(
        geometry.value_face_x,
        geometry.value_face_y,
        geometry.value_face_width,
        geometry.value_face_height,
        geometry);

    if (is_control) {
        html << "<input id='" << html_escape(widget.widget_id) << "_value' name='input_value' type='number' min='" << minimum << "' max='" << maximum << "' step='" << step << "'";
        html << " class='numeric-value-overlay numeric-control-editor' data-frog-part='text_value' data-svg-anchor='text_value.center'";
        html << " style='" << value_style << "'";
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
        html << " style='" << value_style << "'>" << value << "</output>";
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
    const auto label_size = safe_css_length(property_string(widget.properties, "caption.style.font_size", "14px"), "14px");
    const auto label_weight = safe_css_font_weight(property_string(widget.properties, "caption.style.font_weight", "600"), "600");
    const auto label_family = safe_css_font_family(
        property_string(widget.properties, "caption.style.font_family", "system-ui, Segoe UI, Arial, sans-serif"),
        "system-ui, Segoe UI, Arial, sans-serif");
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
         << ";height:" << css_px(height) << ";"
         << "--frog-string-caption-color:" << html_escape(label_color) << ";"
         << "--frog-string-caption-font-size:" << html_escape(label_size) << ";"
         << "--frog-string-caption-font-weight:" << html_escape(label_weight) << ";"
         << "--frog-string-caption-font-family:" << html_escape(label_family) << ";";
    if (!property_bool(widget.properties, "visible", true)) {
        html << "display:none;";
    }
    html << "'>";

    html << render_string_skin(widget);
    html << "<span class='string-caption-overlay' data-frog-part='caption' data-svg-anchor='caption.anchor' style='"
          << caption_anchor_style(widget.properties, geometry)
          << "'>" << html_escape(label) << "</span>";

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

SvgGeometry load_path_svg_geometry(const WidgetState& widget) {
    SvgGeometry geometry;
    geometry.view_width = 520.0;
    geometry.view_height = 150.0;
    geometry.caption_x = 16.0;
    geometry.caption_y = 46.0;
    geometry.value_face_x = 22.0;
    geometry.value_face_y = 82.0;
    geometry.value_face_width = 390.0;
    geometry.value_face_height = 28.0;
    geometry.selector_face_x = 424.0;
    geometry.selector_face_y = 82.0;
    geometry.selector_face_width = 34.0;
    geometry.selector_face_height = 28.0;
    if (widget.asset_path.empty() || !std::filesystem::exists(widget.asset_path)) {
        return geometry;
    }
    const auto svg = read_text_file(widget.asset_path);
    parse_viewbox(svg, geometry);
    geometry.caption_x = svg_attribute_double(svg, "caption_text", "x", geometry.caption_x);
    geometry.caption_y = svg_attribute_double(svg, "caption_text", "y", geometry.caption_y);
    geometry.value_face_x = svg_attribute_double(svg, "path_face", "x", geometry.value_face_x);
    geometry.value_face_y = svg_attribute_double(svg, "path_face", "y", geometry.value_face_y);
    geometry.value_face_width = svg_attribute_double(svg, "path_face", "width", geometry.value_face_width);
    geometry.value_face_height = svg_attribute_double(svg, "path_face", "height", geometry.value_face_height);
    geometry.value_text_x = svg_attribute_double(svg, "path_display", "x", geometry.value_face_x);
    geometry.value_text_y = svg_attribute_double(svg, "path_display", "y", geometry.value_face_y + (geometry.value_face_height / 2.0));
    geometry.selector_face_x = svg_attribute_double(svg, "browse_button", "x", geometry.selector_face_x);
    geometry.selector_face_y = svg_attribute_double(svg, "browse_button", "y", geometry.selector_face_y);
    geometry.selector_face_width = svg_attribute_double(svg, "browse_button", "width", geometry.selector_face_width);
    geometry.selector_face_height = svg_attribute_double(svg, "browse_button", "height", geometry.selector_face_height);
    return geometry;
}

std::string render_path_skin(const WidgetState& widget) {
    if (widget.asset_path.empty() || !std::filesystem::exists(widget.asset_path)) {
        return "<div class='path-skin missing-skin'></div>";
    }
    const auto face_fill = safe_css_color(property_string(widget.properties, "style.path_face.fill_color", "#ffffff"), "#ffffff");
    const auto face_stroke = safe_css_color(property_string(widget.properties, "style.path_face.border_color", "#64748b"), "#64748b");
    const auto face_stroke_width = safe_css_length(property_string(widget.properties, "style.path_face.border_width", "2px"), "2px");
    const auto face_hover_fill = safe_css_color(property_string(widget.properties, "style.path_face.fill_color.hover", face_fill), face_fill);
    const auto face_hover_stroke = safe_css_color(property_string(widget.properties, "style.path_face.border_color.hover", face_stroke), face_stroke);
    const auto text_fill = safe_css_color(property_string(widget.properties, "style.path_display.color", "#111827"), "#111827");
    const auto text_size = safe_css_length(property_string(widget.properties, "style.path_display.font_size", "15px"), "15px");
    const auto text_weight = safe_css_font_weight(property_string(widget.properties, "style.path_display.font_weight", "400"), "400");
    const auto button_fill = safe_css_color(property_string(widget.properties, "style.browse_button.fill_color", "#f8fafc"), "#f8fafc");
    const auto button_hover_fill = safe_css_color(property_string(widget.properties, "style.browse_button.fill_color.hover", button_fill), button_fill);
    const auto button_stroke = safe_css_color(property_string(widget.properties, "style.browse_button.border_color", "#64748b"), "#64748b");
    const auto button_hover_stroke = safe_css_color(property_string(widget.properties, "style.browse_button.border_color.hover", button_stroke), button_stroke);
    const auto button_stroke_width = safe_css_length(property_string(widget.properties, "style.browse_button.border_width", "1px"), "1px");
    const auto button_text_fill = safe_css_color(property_string(widget.properties, "style.browse_button.text_color", "#111827"), "#111827");
    const auto button_text_size = safe_css_length(property_string(widget.properties, "style.browse_button.text_font_size", "13px"), "13px");
    const auto icon_fill = safe_css_color(property_string(widget.properties, "style.path_icon.fill_color", "#facc15"), "#facc15");
    const auto icon_front_fill = safe_css_color(property_string(widget.properties, "style.path_icon.front_fill_color", "#fde68a"), "#fde68a");
    const auto icon_stroke = safe_css_color(property_string(widget.properties, "style.path_icon.stroke_color", "#b45309"), "#b45309");
    const auto icon_highlight = safe_css_color(property_string(widget.properties, "style.path_icon.highlight_color", "#fff7cc"), "#fff7cc");
    const bool icon_visible = property_bool(widget.properties, "display.icon_visible", true);
    const bool browse_visible = property_bool(widget.properties, "browse.button_visible", widget.role == "control");
    const bool validation_visible = property_bool(widget.properties, "display.validation_marker_visible", false);
    const bool overflow_visible = property_bool(widget.properties, "display.text_overflow_visible", false);

    std::ostringstream style;
    style << "--frog-path-label-display:none;";
    style << "--frog-path-caption-display:none;";
    style << "--frog-path-face-fill:" << html_escape(face_fill) << ";";
    style << "--frog-path-face-stroke:" << html_escape(face_stroke) << ";";
    style << "--frog-path-face-stroke-width:" << html_escape(face_stroke_width) << ";";
    style << "--frog-path-text-fill:" << html_escape(text_fill) << ";";
    style << "--frog-path-text-font-size:" << html_escape(text_size) << ";";
    style << "--frog-path-text-font-weight:" << html_escape(text_weight) << ";";
    style << "--frog-path-button-fill:" << html_escape(button_fill) << ";";
    style << "--frog-path-button-fill-hover:" << html_escape(button_hover_fill) << ";";
    style << "--frog-path-button-stroke:" << html_escape(button_stroke) << ";";
    style << "--frog-path-button-stroke-hover:" << html_escape(button_hover_stroke) << ";";
    style << "--frog-path-button-stroke-width:" << html_escape(button_stroke_width) << ";";
    style << "--frog-path-button-text-fill:" << html_escape(button_text_fill) << ";";
    style << "--frog-path-button-text-font-size:" << html_escape(button_text_size) << ";";
    style << "--frog-path-icon-display:" << (icon_visible ? "inline" : "none") << ";";
    style << "--frog-path-icon-fill:" << html_escape(icon_fill) << ";";
    style << "--frog-path-icon-front-fill:" << html_escape(icon_front_fill) << ";";
    style << "--frog-path-icon-stroke:" << html_escape(icon_stroke) << ";";
    style << "--frog-path-icon-highlight:" << html_escape(icon_highlight) << ";";
    style << "--frog-path-browse-display:" << (browse_visible ? "inline" : "none") << ";";
    style << "--frog-path-validation-display:" << (validation_visible ? "inline" : "none") << ";";
    style << "--frog-path-overflow-display:" << (overflow_visible ? "inline" : "none") << ";";
    style << "--frog-path-face-fill-hover:" << html_escape(face_hover_fill) << ";";
    style << "--frog-path-face-stroke-hover:" << html_escape(face_hover_stroke) << ";";

    std::ostringstream html;
    html << "<div class='path-skin' aria-hidden='true' style='" << style.str() << "'>";
    html << read_text_file(widget.asset_path);
    html << "</div>";
    return html.str();
}

std::string render_path_widget(const WidgetState& widget) {
    const bool is_control = widget.role == "control";
    const auto geometry = load_path_svg_geometry(widget);
    const auto x = layout_i64(widget.layout, "x", 0);
    const auto y = layout_i64(widget.layout, "y", 0);
    const auto width = layout_i64(widget.layout, "width", 300);
    const auto height = layout_i64(widget.layout, "height", 120);
    const auto value = property_string(widget.properties, "value");
    const auto label = property_string(widget.properties, "caption.text", widget.widget_id);
    const auto label_color = safe_css_color(property_string(widget.properties, "caption.style.text_color", "#111827"), "#111827");
    const auto label_size = safe_css_length(property_string(widget.properties, "caption.style.font_size", "14px"), "14px");
    const auto label_weight = safe_css_font_weight(property_string(widget.properties, "caption.style.font_weight", "600"), "600");
    const auto label_family = safe_css_font_family(
        property_string(widget.properties, "caption.style.font_family", "system-ui, Segoe UI, Arial, sans-serif"),
        "system-ui, Segoe UI, Arial, sans-serif");
    const auto text_color = safe_css_color(property_string(widget.properties, "style.path_display.color", "#111827"), "#111827");
    const auto text_size = safe_css_length(property_string(widget.properties, "style.path_display.font_size", "15px"), "15px");
    const auto text_weight = safe_css_font_weight(property_string(widget.properties, "style.path_display.font_weight", "400"), "400");
    const auto text_padding = safe_css_length(property_string(widget.properties, "style.path_display.padding_inline", "8px"), "8px");
    const auto text_baseline_offset = safe_css_signed_length(property_string(widget.properties, "style.path_display.baseline_offset", "0px"), "0px");
    const auto computed_line_height = css_px((geometry.value_face_height / geometry.view_height) * static_cast<double>(height));
    const auto text_line_height = safe_css_length(property_string(widget.properties, "style.path_display.line_height", computed_line_height), computed_line_height);
    const auto button_fill = safe_css_color(property_string(widget.properties, "style.browse_button.fill_color", "#f8fafc"), "#f8fafc");
    const auto button_hover_fill = safe_css_color(property_string(widget.properties, "style.browse_button.fill_color.hover", button_fill), button_fill);
    const auto public_input_id = property_string(
        widget.properties,
        "binding.public_input_id",
        property_string(widget.properties, "binding.preview_input_id", widget.widget_id + "_value"));
    const auto route = asset_route(widget);
    const bool icon_visible = property_bool(widget.properties, "display.icon_visible", true);
    const auto value_text_x = icon_visible ? std::max(geometry.value_face_x, geometry.value_text_x) : geometry.value_face_x;
    const auto value_text_width = std::max(0.0, geometry.value_face_width - (value_text_x - geometry.value_face_x));
    const auto value_style = svg_box_style(
        value_text_x,
        geometry.value_face_y,
        value_text_width,
        geometry.value_face_height,
        geometry);
    const auto browse_style = svg_box_style(
        geometry.selector_face_x,
        geometry.selector_face_y,
        geometry.selector_face_width,
        geometry.selector_face_height,
        geometry);
    const bool browse_visible = property_bool(widget.properties, "browse.button_visible", is_control);
    const bool interaction_enabled = property_bool(widget.properties, "interaction.enabled", true);

    std::ostringstream html;
    html << "<section class='frog-widget path-widget " << (is_control ? "path-control" : "path-indicator") << "'";
    html << " data-widget-id='" << html_escape(widget.widget_id) << "'";
    html << " data-class-ref='" << html_escape(widget.class_ref) << "'";
    html << " data-role='" << html_escape(widget.role) << "'";
    html << " data-frog-visual-law='wfrog-realization-state-map'";
    html << " data-frog-browse-visible='" << (browse_visible ? "true" : "false") << "'";
    if (!route.empty()) {
        html << " data-asset-route='" << html_escape(route) << "'";
    }
    html << " style='position:absolute;left:" << css_px(x) << ";top:" << css_px(y) << ";width:" << css_px(width)
         << ";height:" << css_px(height)
         << ";--frog-path-caption-color:" << html_escape(label_color)
         << ";--frog-path-caption-font-size:" << html_escape(label_size)
         << ";--frog-path-caption-font-weight:" << html_escape(label_weight)
         << ";--frog-path-caption-font-family:" << html_escape(label_family)
         << ";--frog-path-button-fill:" << html_escape(button_fill)
         << ";--frog-path-button-fill-hover:" << html_escape(button_hover_fill) << ";";
    if (!property_bool(widget.properties, "visible", true)) {
        html << "display:none;";
    }
    html << "'>";

    html << render_path_skin(widget);
    html << "<span class='path-caption-overlay' data-frog-part='caption' data-svg-anchor='caption.anchor' style='"
          << caption_anchor_style(widget.properties, geometry)
          << "'>" << html_escape(label) << "</span>";

    if (is_control) {
        html << "<input id='" << html_escape(widget.widget_id) << "_value' name='" << html_escape(public_input_id) << "' type='text'";
        html << " class='path-value-overlay path-control-editor' data-frog-part='path_display' data-svg-anchor='path_display.left_center'";
        html << " data-frog-input-id='" << html_escape(public_input_id) << "'";
        html << " style='" << value_style << "color:" << html_escape(text_color) << ";font-size:" << html_escape(text_size)
             << ";font-weight:" << html_escape(text_weight) << ";padding:0 " << html_escape(text_padding)
             << ";line-height:" << html_escape(text_line_height)
             << ";transform:translateY(" << html_escape(text_baseline_offset) << ");'";
        html << " value='" << html_escape(value) << "'";
        if (!interaction_enabled) {
            html << " disabled";
        }
        html << " />";
        html << "<input id='" << html_escape(widget.widget_id) << "_file_picker' type='file' class='path-file-picker' tabindex='-1' aria-hidden='true'";
        html << " onchange=\"frogPathPicked(this,'" << html_escape(widget.widget_id) << "_value')\" />";
        if (browse_visible) {
            html << "<label for='" << html_escape(widget.widget_id) << "_file_picker' class='path-browse-overlay' data-frog-part='browse_button'";
            html << " aria-label='Browse " << html_escape(label) << "' style='" << browse_style << "'></label>";
        }
    } else {
        html << "<output class='path-value-overlay path-indicator-value' data-frog-part='path_display' data-svg-anchor='path_display.left_center'";
        html << " style='" << value_style << "color:" << html_escape(text_color) << ";font-size:" << html_escape(text_size)
             << ";font-weight:" << html_escape(text_weight) << ";padding:0 " << html_escape(text_padding)
             << ";line-height:" << html_escape(text_line_height)
             << ";transform:translateY(" << html_escape(text_baseline_offset) << ");'>" << html_escape(value) << "</output>";
    }

    html << "</section>";
    return html.str();
}

SvgGeometry load_enum_svg_geometry(const WidgetState& widget) {
    SvgGeometry geometry;
    geometry.view_width = 380.0;
    geometry.view_height = 150.0;
    geometry.caption_x = 16.0;
    geometry.caption_y = 46.0;
    geometry.value_face_x = 22.0;
    geometry.value_face_y = 82.0;
    geometry.value_face_width = 214.0;
    geometry.value_face_height = 28.0;
    geometry.selector_face_x = 246.0;
    geometry.selector_face_y = 82.0;
    geometry.selector_face_width = 24.0;
    geometry.selector_face_height = 28.0;
    geometry.increment_up_x = 278.0;
    geometry.increment_up_y = 82.0;
    geometry.increment_up_width = 24.0;
    geometry.increment_up_height = 13.0;
    geometry.increment_down_x = 278.0;
    geometry.increment_down_y = 97.0;
    geometry.increment_down_width = 24.0;
    geometry.increment_down_height = 13.0;
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
    geometry.selector_face_x = svg_attribute_double(svg, "selector_face", "x", geometry.selector_face_x);
    geometry.selector_face_y = svg_attribute_double(svg, "selector_face", "y", geometry.selector_face_y);
    geometry.selector_face_width = svg_attribute_double(svg, "selector_face", "width", geometry.selector_face_width);
    geometry.selector_face_height = svg_attribute_double(svg, "selector_face", "height", geometry.selector_face_height);
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

struct EnumUiItem {
    std::string id;
    std::string text;
    std::uint16_t numeric_value = 0;
    bool enabled = true;
};

std::vector<EnumUiItem> enum_ui_items(const WidgetState& widget) {
    const auto it = widget.properties.find("items");
    if (it == widget.properties.end() || !it->second.is_array()) {
        throw std::runtime_error("Enum widget " + widget.widget_id + " must define items in front-panel instance properties.");
    }
    std::vector<EnumUiItem> items;
    for (const auto& value : it->second.as_array()) {
        if (!value.is_object()) {
            throw std::runtime_error("Enum item must be an object.");
        }
        const auto& object = value.as_object();
        const auto id = property_string(object, "id");
        const auto text = property_string(object, "text");
        const auto numeric_it = object.find("numeric_value");
        if (id.empty() || text.empty() || numeric_it == object.end() || !numeric_it->second.is_number()) {
            throw std::runtime_error("Enum item must publish id, text, and numeric_value.");
        }
        const auto raw = numeric_it->second.as_i64();
        if (raw < 0 || raw > 65535) {
            throw std::runtime_error("Enum item numeric_value must remain in the u16 domain.");
        }
        items.push_back(EnumUiItem{
            id,
            text,
            static_cast<std::uint16_t>(raw),
            property_bool(object, "enabled", true),
        });
    }
    if (items.empty()) {
        throw std::runtime_error("Enum widget " + widget.widget_id + " must define at least one item.");
    }
    return items;
}

const EnumUiItem& selected_enum_item(const std::vector<EnumUiItem>& items, const std::string& id) {
    const auto it = std::find_if(items.begin(), items.end(), [&](const EnumUiItem& item) { return item.id == id; });
    if (it == items.end()) {
        throw std::runtime_error("Enum widget value must resolve to a declared item.");
    }
    return *it;
}

std::string render_enum_skin(const WidgetState& widget) {
    if (widget.asset_path.empty() || !std::filesystem::exists(widget.asset_path)) {
        return "<div class='enum-skin missing-skin'></div>";
    }
    const auto frame_fill = safe_css_color(property_string(widget.properties, "style.frame.fill_color", "transparent"), "transparent");
    const auto frame_stroke = safe_css_color(property_string(widget.properties, "style.frame.border_color", "transparent"), "transparent");
    const auto frame_stroke_width = safe_css_length(property_string(widget.properties, "style.frame.border_width", "0px"), "0px");
    const bool frame_visible = property_bool(widget.properties, "style.frame.visible", false) ||
                               (frame_stroke != "transparent" && frame_stroke_width != "0" && frame_stroke_width != "0px");
    const auto value_face_fill = safe_css_color(property_string(widget.properties, "style.value_face.fill_color", "#ffffff"), "#ffffff");
    const auto value_face_stroke = safe_css_color(property_string(widget.properties, "style.value_face.border_color", "#64748b"), "#64748b");
    const auto value_face_stroke_width = safe_css_length(property_string(widget.properties, "style.value_face.border_width", "2px"), "2px");
    const bool selector_visible = property_bool(widget.properties, "display.selector_visible", widget.role == "control");
    const bool increment_visible = property_bool(widget.properties, "display.increment_buttons_visible", false);
    const bool digital_visible = property_bool(widget.properties, "display.digital_display_visible", false);
    const bool overflow_visible = property_bool(widget.properties, "display.text_overflow_visible", false);
    const auto selector_fill = safe_css_color(property_string(widget.properties, "style.selector_face.fill_color", "#f1f5f9"), "#f1f5f9");
    const auto selector_stroke =
        safe_css_color(property_string(widget.properties, "style.selector_face.border_color", "#64748b"), "#64748b");
    const auto selector_stroke_width =
        safe_css_length(property_string(widget.properties, "style.selector_face.border_width", "1px"), "1px");
    const auto selector_symbol =
        safe_css_color(property_string(widget.properties, "style.selector_face.symbol_color", "#111827"), "#111827");
    std::ostringstream style;
    style << "--frog-enum-label-display:none;";
    style << "--frog-enum-caption-display:none;";
    style << "--frog-enum-value-display:none;";
    style << "--frog-enum-frame-display:" << (frame_visible ? "inline" : "none") << ";";
    style << "--frog-enum-frame-fill:" << html_escape(frame_fill) << ";";
    style << "--frog-enum-frame-stroke:" << html_escape(frame_stroke) << ";";
    style << "--frog-enum-frame-stroke-width:" << html_escape(frame_stroke_width) << ";";
    style << "--frog-enum-value-face-fill:" << html_escape(value_face_fill) << ";";
    style << "--frog-enum-value-face-stroke:" << html_escape(value_face_stroke) << ";";
    style << "--frog-enum-value-face-stroke-width:" << html_escape(value_face_stroke_width) << ";";
    style << "--frog-enum-selector-display:" << (selector_visible ? "inline" : "none") << ";";
    style << "--frog-enum-selector-fill:" << html_escape(selector_fill) << ";";
    style << "--frog-enum-selector-stroke:" << html_escape(selector_stroke) << ";";
    style << "--frog-enum-selector-stroke-width:" << html_escape(selector_stroke_width) << ";";
    style << "--frog-enum-selector-symbol:" << html_escape(selector_symbol) << ";";
    style << "--frog-enum-increment-display:" << (increment_visible ? "inline" : "none") << ";";
    style << "--frog-enum-digital-display:" << (digital_visible ? "inline" : "none") << ";";
    style << "--frog-enum-overflow-display:" << (overflow_visible ? "inline" : "none") << ";";

    std::ostringstream html;
    html << "<div class='enum-skin' aria-hidden='true' style='" << style.str() << "'>";
    html << read_text_file(widget.asset_path);
    html << "</div>";
    return html.str();
}

std::string render_enum_widget(const WidgetState& widget) {
    const bool is_control = widget.role == "control";
    const auto geometry = load_enum_svg_geometry(widget);
    const auto x = layout_i64(widget.layout, "x", 0);
    const auto y = layout_i64(widget.layout, "y", 0);
    const auto width = layout_i64(widget.layout, "width", 260);
    const auto height = layout_i64(widget.layout, "height", 110);
    const auto value = property_string(widget.properties, "value");
    const auto items = enum_ui_items(widget);
    const auto& selected = selected_enum_item(items, value);
    const auto style_scale = widget_style_scale(widget.properties, width, height);
    const auto label = property_string(widget.properties, "caption.text", widget.widget_id);
    const auto label_color = safe_css_color(property_string(widget.properties, "caption.style.text_color", "#111827"), "#111827");
    const auto label_size = safe_css_length(property_string(widget.properties, "caption.style.font_size", "14px"), "14px");
    const auto label_weight = safe_css_font_weight(property_string(widget.properties, "caption.style.font_weight", "600"), "600");
    const auto label_family = safe_css_font_family(
        property_string(widget.properties, "caption.style.font_family", "system-ui, Segoe UI, Arial, sans-serif"),
        "system-ui, Segoe UI, Arial, sans-serif");
    const auto text_color = safe_css_color(property_string(widget.properties, "style.value_display.color", "#111827"), "#111827");
    const auto text_size = maybe_scaled_css_length(
        widget.properties, "style.value_display.font_size", "style.value_display.font_size_mode", "16px", style_scale);
    const auto text_weight = safe_css_font_weight(property_string(widget.properties, "style.value_display.font_weight", "400"), "400");
    const auto text_vertical_offset = maybe_scaled_css_length(
        widget.properties, "style.value_display.vertical_offset", "style.value_display.vertical_offset_mode", "0px", style_scale, true);
    const auto text_padding_inline = maybe_scaled_css_length(
        widget.properties, "style.value_display.padding_inline", "style.value_display.padding_inline_mode", "8px", style_scale);
    const auto route = asset_route(widget);
    const bool selector_visible = property_bool(widget.properties, "display.selector_visible", is_control);
    const bool interaction_enabled = property_bool(widget.properties, "interaction.enabled", true);
    const auto selector_fill =
        safe_css_color(property_string(widget.properties, "style.selector_face.fill_color", "#f1f5f9"), "#f1f5f9");
    const auto selector_hover_fill =
        safe_css_color(property_string(widget.properties, "style.selector_face.fill_color.hover", selector_fill), selector_fill);
    const auto selector_stroke =
        safe_css_color(property_string(widget.properties, "style.selector_face.border_color", "#64748b"), "#64748b");
    const auto selector_hover_stroke =
        safe_css_color(property_string(widget.properties, "style.selector_face.border_color.hover", selector_stroke), selector_stroke);
    const auto selector_stroke_width = maybe_scaled_css_length(
        widget.properties, "style.selector_face.border_width", "style.selector_face.border_width_mode", "1px", style_scale);
    const auto selector_radius = maybe_scaled_css_length(
        widget.properties, "style.selector_face.border_radius", "style.selector_face.border_radius_mode", "1px", style_scale);
    const auto selector_symbol =
        safe_css_color(property_string(widget.properties, "style.selector_face.symbol_color", "#111827"), "#111827");
    const auto selector_hover_symbol =
        safe_css_color(property_string(widget.properties, "style.selector_face.symbol_color.hover", selector_symbol), selector_symbol);
    const auto selector_symbol_width = maybe_scaled_css_length(
        widget.properties, "style.selector_face.symbol_width", "style.selector_face.symbol_size_mode", "10px", style_scale);
    const auto selector_symbol_height = maybe_scaled_css_length(
        widget.properties, "style.selector_face.symbol_height", "style.selector_face.symbol_size_mode", "7px", style_scale);
    const auto value_hover_fill =
        safe_css_color(property_string(widget.properties, "style.value_face.fill_color.hover", "transparent"), "transparent");
    const auto dropdown_fill =
        safe_css_color(property_string(widget.properties, "style.dropdown.fill_color", "#ffffff"), "#ffffff");
    const auto dropdown_border =
        safe_css_color(property_string(widget.properties, "style.dropdown.border_color", "#64748b"), "#64748b");
    const auto dropdown_border_width = maybe_scaled_css_length(
        widget.properties, "style.dropdown.border_width", "style.dropdown.border_width_mode", "1px", style_scale);
    const auto dropdown_option_fill =
        safe_css_color(property_string(widget.properties, "style.dropdown.option.fill_color", "#ffffff"), "#ffffff");
    const auto dropdown_option_text =
        safe_css_color(property_string(widget.properties, "style.dropdown.option.text_color", text_color), text_color);
    const auto dropdown_option_hover_fill = safe_css_color(
        property_string(widget.properties, "style.dropdown.option.hover_fill_color", "#2563eb"), "#2563eb");
    const auto dropdown_option_hover_text = safe_css_color(
        property_string(widget.properties, "style.dropdown.option.hover_text_color", "#ffffff"), "#ffffff");
    const auto dropdown_option_selected_fill = safe_css_color(
        property_string(widget.properties, "style.dropdown.option.selected_fill_color", dropdown_option_hover_fill),
        dropdown_option_hover_fill);
    const auto dropdown_option_selected_text = safe_css_color(
        property_string(widget.properties, "style.dropdown.option.selected_text_color", dropdown_option_hover_text),
        dropdown_option_hover_text);
    const auto dropdown_option_font_size = maybe_scaled_css_length(
        widget.properties, "style.dropdown.option.font_size", "style.dropdown.option.font_size_mode", text_size, style_scale);
    const auto dropdown_option_font_weight =
        safe_css_font_weight(property_string(widget.properties, "style.dropdown.option.font_weight", text_weight), text_weight);
    const auto dropdown_option_font_style =
        safe_css_font_style(property_string(widget.properties, "style.dropdown.option.font_style", "normal"), "normal");
    const auto dropdown_option_font_family = safe_css_font_family(
        property_string(widget.properties, "style.dropdown.option.font_family", "Segoe UI,Arial,sans-serif"),
        "Segoe UI,Arial,sans-serif");
    const auto dropdown_option_padding_inline = maybe_scaled_css_length(
        widget.properties, "style.dropdown.option.padding_inline", "style.dropdown.option.padding_inline_mode", text_padding_inline, style_scale);
    const auto dropdown_option_height = maybe_scaled_css_length(
        widget.properties, "style.dropdown.option.height", "style.dropdown.option.height_mode", "28px", style_scale);
    const auto value_style = svg_box_style(
        geometry.value_face_x,
        geometry.value_face_y,
        geometry.value_face_width,
        geometry.value_face_height,
        geometry);
    const auto dropdown_style = svg_dropdown_style(
        geometry.value_face_x,
        geometry.value_face_y,
        geometry.value_face_width,
        geometry.value_face_height,
        geometry);
    const auto selector_style = svg_box_style(
        geometry.selector_face_x,
        geometry.selector_face_y,
        geometry.selector_face_width,
        geometry.selector_face_height,
        geometry);
    const bool digital_visible = property_bool(widget.properties, "display.digital_display_visible", false);
    const bool increment_visible = property_bool(widget.properties, "display.increment_buttons_visible", false);
    const bool overflow_visible = property_bool(widget.properties, "display.text_overflow_visible", false);

    std::ostringstream html;
    html << "<section class='frog-widget enum-widget " << (is_control ? "enum-control" : "enum-indicator") << "'";
    html << " data-widget-id='" << html_escape(widget.widget_id) << "'";
    html << " data-class-ref='" << html_escape(widget.class_ref) << "'";
    html << " data-role='" << html_escape(widget.role) << "'";
    html << " data-frog-visual-law='wfrog-realization-state-map'";
    html << " data-frog-digital-display-visible='" << (digital_visible ? "true" : "false") << "'";
    html << " data-frog-increment-buttons-visible='" << (increment_visible ? "true" : "false") << "'";
    html << " data-frog-selector-visible='" << (selector_visible ? "true" : "false") << "'";
    html << " data-frog-text-overflow-visible='" << (overflow_visible ? "true" : "false") << "'";
    if (!route.empty()) {
        html << " data-asset-route='" << html_escape(route) << "'";
    }
    html << " style='position:absolute;left:" << css_px(x) << ";top:" << css_px(y) << ";width:" << css_px(width)
         << ";height:" << css_px(height) << ";--frog-enum-selector-fill:" << html_escape(selector_fill)
         << ";--frog-enum-caption-color:" << html_escape(label_color)
         << ";--frog-enum-caption-font-size:" << html_escape(label_size)
         << ";--frog-enum-caption-font-weight:" << html_escape(label_weight)
         << ";--frog-enum-caption-font-family:" << html_escape(label_family)
         << ";--frog-enum-selector-stroke:" << html_escape(selector_stroke)
         << ";--frog-enum-selector-stroke-width:" << html_escape(selector_stroke_width)
         << ";--frog-enum-selector-radius:" << html_escape(selector_radius)
         << ";--frog-enum-selector-symbol:" << html_escape(selector_symbol)
         << ";--frog-enum-selector-symbol-width:" << html_escape(selector_symbol_width)
         << ";--frog-enum-selector-symbol-height:" << html_escape(selector_symbol_height)
         << ";--frog-enum-selector-hover-fill:" << html_escape(selector_hover_fill)
         << ";--frog-enum-selector-hover-stroke:" << html_escape(selector_hover_stroke)
         << ";--frog-enum-selector-hover-symbol:" << html_escape(selector_hover_symbol)
         << ";--frog-enum-value-hover-fill:" << html_escape(value_hover_fill)
         << ";--frog-enum-text-padding-inline:" << html_escape(text_padding_inline)
         << ";--frog-enum-dropdown-fill:" << html_escape(dropdown_fill)
         << ";--frog-enum-dropdown-border:" << html_escape(dropdown_border)
         << ";--frog-enum-dropdown-border-width:" << html_escape(dropdown_border_width)
         << ";--frog-enum-dropdown-option-fill:" << html_escape(dropdown_option_fill)
         << ";--frog-enum-dropdown-option-text:" << html_escape(dropdown_option_text)
         << ";--frog-enum-dropdown-option-hover-fill:" << html_escape(dropdown_option_hover_fill)
         << ";--frog-enum-dropdown-option-hover-text:" << html_escape(dropdown_option_hover_text)
         << ";--frog-enum-dropdown-option-selected-fill:" << html_escape(dropdown_option_selected_fill)
         << ";--frog-enum-dropdown-option-selected-text:" << html_escape(dropdown_option_selected_text)
         << ";--frog-enum-dropdown-option-font-family:" << html_escape(dropdown_option_font_family)
         << ";--frog-enum-dropdown-option-font-size:" << html_escape(dropdown_option_font_size)
         << ";--frog-enum-dropdown-option-font-weight:" << html_escape(dropdown_option_font_weight)
         << ";--frog-enum-dropdown-option-font-style:" << html_escape(dropdown_option_font_style)
         << ";--frog-enum-dropdown-option-padding-inline:" << html_escape(dropdown_option_padding_inline)
         << ";--frog-enum-dropdown-option-height:" << html_escape(dropdown_option_height) << ";";
    if (!property_bool(widget.properties, "visible", true)) {
        html << "display:none;";
    }
    html << "'>";

    html << render_enum_skin(widget);
    html << "<span class='enum-caption-overlay' data-frog-part='caption' data-svg-anchor='caption.anchor' style='"
          << caption_anchor_style(widget.properties, geometry)
          << "'>" << html_escape(label) << "</span>";

    if (is_control) {
        html << "<button id='" << html_escape(widget.widget_id) << "_display' type='button'";
        html << " class='enum-value-display-overlay enum-display-button' data-frog-part='value_display' data-svg-anchor='value_display.left_center'";
        html << " aria-haspopup='listbox' aria-expanded='false' aria-controls='" << html_escape(widget.widget_id) << "_dropdown'";
        html << " onclick=\"frogToggleEnumDropdown('" << html_escape(widget.widget_id) << "_dropdown','" << html_escape(widget.widget_id) << "_display')\"";
        html << " style='"
             << value_style << "color:" << html_escape(text_color) << ";font-size:" << html_escape(text_size)
             << ";font-weight:" << html_escape(text_weight) << ";--frog-enum-text-vertical-offset:"
             << html_escape(text_vertical_offset) << ";'";
        if (!interaction_enabled) {
            html << " disabled";
        }
        html << ">" << html_escape(selected.text) << "</button>";
        if (selector_visible) {
            html << "<button type='button' class='enum-selector-overlay enum-selector-button' data-frog-part='selector_face'";
            html << " aria-label='Open " << html_escape(label) << "'";
            html << " aria-haspopup='listbox' aria-expanded='false' aria-controls='" << html_escape(widget.widget_id) << "_dropdown'";
            html << " onclick=\"frogToggleEnumDropdown('" << html_escape(widget.widget_id) << "_dropdown','" << html_escape(widget.widget_id) << "_display')\"";
            html << " style='" << selector_style << "'";
            if (!interaction_enabled) {
                html << " disabled";
            }
            html << "></button>";
        }
        html << "<select id='" << html_escape(widget.widget_id) << "_value' name='mode_value'";
        html << " class='enum-select-state' data-frog-part='value_state' aria-hidden='true' tabindex='-1'";
        html << " aria-label='" << html_escape(label) << "'";
        html << " onchange=\"frogUpdateEnumDisplay(this,'" << html_escape(widget.widget_id) << "_display')\"";
        html << " oninput=\"frogUpdateEnumDisplay(this,'" << html_escape(widget.widget_id) << "_display')\"";
        html << " hidden";
        if (!interaction_enabled) {
            html << " disabled";
        }
        html << ">";
        for (const auto& item : items) {
            html << "<option value='" << html_escape(item.id) << "'";
            if (item.id == selected.id) {
                html << " selected";
            }
            if (!item.enabled) {
                html << " disabled";
            }
            html << ">" << html_escape(item.text) << "</option>";
        }
        html << "</select>";
        html << "<div id='" << html_escape(widget.widget_id) << "_dropdown' class='enum-dropdown' data-frog-part='dropdown'";
        html << " role='listbox' aria-label='" << html_escape(label) << " options' hidden style='" << dropdown_style << "'>";
        for (const auto& item : items) {
            html << "<button type='button' class='enum-dropdown-option'";
            html << " role='option' data-enum-value='" << html_escape(item.id) << "'";
            html << " aria-selected='" << (item.id == selected.id ? "true" : "false") << "'";
            html << " onclick=\"frogSelectEnumOption(this,'" << html_escape(widget.widget_id) << "_value','"
                 << html_escape(widget.widget_id) << "_display','" << html_escape(widget.widget_id) << "_dropdown')\"";
            if (!item.enabled || !interaction_enabled) {
                html << " disabled";
            }
            html << ">" << html_escape(item.text) << "</button>";
        }
        html << "</div>";
    } else {
        html << "<output class='enum-value-overlay enum-indicator-value' data-frog-part='value_display' data-svg-anchor='value_display.left_center'";
        html << " style='" << value_style << "color:" << html_escape(text_color) << ";font-size:" << html_escape(text_size)
             << ";font-weight:" << html_escape(text_weight) << ";--frog-enum-text-vertical-offset:"
             << html_escape(text_vertical_offset) << ";'>" << html_escape(selected.text) << "</output>";
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
    const auto caption_color = safe_css_color(property_string(widget.properties, "caption.style.text_color", "#111827"), "#111827");
    const auto caption_size = safe_css_length(property_string(widget.properties, "caption.style.font_size", "18px"), "18px");
    const auto caption_weight = safe_css_font_weight(property_string(widget.properties, "caption.style.font_weight", "600"), "600");
    const auto caption_family = safe_css_font_family(
        property_string(widget.properties, "caption.style.font_family", "system-ui, Segoe UI, Arial, sans-serif"),
        "system-ui, Segoe UI, Arial, sans-serif");
    const std::string false_fill = state_property(widget.properties, "style.inner.fill_color", "false", "#ffffff");
    const std::string true_fill = state_property(widget.properties, "style.inner.fill_color", "true", "#8bd86f");
    const std::string state_fill = value ? true_fill : false_fill;
    const std::string hover_fill = state_property(widget.properties, "style.inner.fill_color", hover_state, value ? "#9be884" : "#eef6ff");
    const std::string pressed_fill = state_property(widget.properties, "style.inner.fill_color", pressed_state, value ? "#6fc657" : "#dbeafe");
    const std::string false_border = state_property(widget.properties, "style.outer.border_color", "false", "#111827");
    const std::string true_border = state_property(widget.properties, "style.outer.border_color", "true", "#184a24");
    const std::string state_border = value ? true_border : false_border;
    const std::string hover_border = state_property(widget.properties, "style.outer.border_color", hover_state, value ? "#166534" : "#2563eb");
    const std::string pressed_border = state_property(widget.properties, "style.outer.border_color", pressed_state, value ? "#14532d" : "#1d4ed8");
    const std::string false_inner_border = state_property(widget.properties, "style.inner.border_color", "false", false_border);
    const std::string true_inner_border = state_property(widget.properties, "style.inner.border_color", "true", true_border);
    const std::string state_inner_border = value ? true_inner_border : false_inner_border;
    const std::string hover_inner_border = state_property(widget.properties, "style.inner.border_color", hover_state, hover_border);
    const std::string pressed_inner_border = state_property(widget.properties, "style.inner.border_color", pressed_state, pressed_border);
    const std::string false_text_color = state_property(widget.properties, "state_text.style.text_color", "false", "#111827");
    const std::string true_text_color = state_property(widget.properties, "state_text.style.text_color", "true", "#0b3d19");
    const std::string text_color = value ? true_text_color : false_text_color;
    const std::string false_state_text = property_string(widget.properties, "state_text.false_text", "FALSE");
    const std::string true_state_text = property_string(widget.properties, "state_text.true_text", "TRUE");
    const std::string transition_ms = property_string(widget.properties, "style.transition.duration_ms", "120");
    const std::string transition_timing = property_string(widget.properties, "style.transition.timing", "ease-out");
    const std::string pressed_inset = property_string(widget.properties, "style.pressed.inset", "1px");
    const bool state_text_visible = property_bool(widget.properties, "state_text.visible", true);
    const bool frame_visible = property_bool(widget.properties, "style.frame.visible", true);
    const bool focus_visible = property_bool(widget.properties, "style.focus_ring.visible", false);
    const std::string text_size = safe_css_length(property_string(widget.properties, "state_text.style.font_size", "18px"), "18px");
    const std::string text_weight = safe_css_font_weight(property_string(widget.properties, "state_text.style.font_weight", "400"), "400");
    const std::string inner_left = property_string(widget.properties, "style.inner.left", variant == "circular" ? "52px" : "18px");
    const std::string inner_top = property_string(widget.properties, "style.inner.top", variant == "circular" ? "23px" : "31px");
    const std::string inner_width = property_string(widget.properties, "style.inner.width", variant == "circular" ? "56px" : "124px");
    const std::string inner_height = property_string(widget.properties, "style.inner.height", variant == "circular" ? "56px" : "34px");
    const std::string inner_border_width = safe_css_length(property_string(widget.properties, "style.inner.border_width", "2px"), "2px");
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
    html << " data-frog-fill-false='" << html_escape(false_fill) << "'";
    html << " data-frog-fill-true='" << html_escape(true_fill) << "'";
    html << " data-frog-inner-border-false='" << html_escape(false_inner_border) << "'";
    html << " data-frog-inner-border-true='" << html_escape(true_inner_border) << "'";
    html << " data-frog-text-color-false='" << html_escape(false_text_color) << "'";
    html << " data-frog-text-color-true='" << html_escape(true_text_color) << "'";
    html << " data-frog-text-false='" << html_escape(false_state_text) << "'";
    html << " data-frog-text-true='" << html_escape(true_state_text) << "'";
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
         << "--boolean-inner-border-width:" << html_escape(inner_border_width) << ";"
         << "--boolean-text:" << html_escape(text_color) << ";"
         << "--boolean-text-font-size:" << html_escape(text_size) << ";"
         << "--boolean-text-font-weight:" << html_escape(text_weight) << ";"
         << "--boolean-caption-color:" << html_escape(caption_color) << ";"
         << "--boolean-caption-font-size:" << html_escape(caption_size) << ";"
         << "--boolean-caption-font-weight:" << html_escape(caption_weight) << ";"
         << "--boolean-caption-font-family:" << html_escape(caption_family) << ";"
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
        html << "<span class='boolean-state-overlay' data-frog-part='state_text' data-svg-anchor='state_text.center' style='"
             << state_text_anchor_style(widget.properties, geometry) << "'>" << html_escape(state_text) << "</span>";
    }
    html << (is_control ? "</button>" : "</section>");
    return html.str();
}

struct ButtonSvgGeometry {
    double view_width = 340.0;
    double view_height = 220.0;
    double caption_x = 16.0;
    double caption_y = 46.0;
    double face_x = 26.0;
    double face_y = 86.0;
    double face_width = 288.0;
    double face_height = 80.0;
    double state_text_x = 170.0;
    double state_text_y = 127.0;
};

void parse_button_viewbox(const std::string& svg, ButtonSvgGeometry& geometry) {
    SvgGeometry viewbox_probe;
    viewbox_probe.view_width = geometry.view_width;
    viewbox_probe.view_height = geometry.view_height;
    parse_viewbox(svg, viewbox_probe);
    geometry.view_width = viewbox_probe.view_width;
    geometry.view_height = viewbox_probe.view_height;
}

ButtonSvgGeometry load_button_svg_geometry(const WidgetState& widget) {
    ButtonSvgGeometry geometry;
    if (widget.asset_path.empty() || !std::filesystem::exists(widget.asset_path)) {
        return geometry;
    }
    const auto svg = read_text_file(widget.asset_path);
    parse_button_viewbox(svg, geometry);
    geometry.caption_x = svg_part_attribute_double(svg, "caption", "x", geometry.caption_x);
    geometry.caption_y = svg_part_attribute_double(svg, "caption", "y", geometry.caption_y);
    geometry.face_x = svg_part_attribute_double(svg, "face", "x", geometry.face_x);
    geometry.face_y = svg_part_attribute_double(svg, "face", "y", geometry.face_y);
    geometry.face_width = svg_part_attribute_double(svg, "face", "width", geometry.face_width);
    geometry.face_height = svg_part_attribute_double(svg, "face", "height", geometry.face_height);
    geometry.state_text_x = svg_part_attribute_double(svg, "state_text", "x", geometry.state_text_x);
    geometry.state_text_y = svg_part_attribute_double(svg, "state_text", "y", geometry.state_text_y);
    return geometry;
}

std::string button_anchor_style(double x, double y, const ButtonSvgGeometry& geometry) {
    std::ostringstream style;
    style << "left:" << css_percent(pct(x, geometry.view_width)) << ";";
    style << "top:" << css_percent(pct(y, geometry.view_height)) << ";";
    return style.str();
}

std::string button_box_style(double x, double y, double width, double height, const ButtonSvgGeometry& geometry) {
    std::ostringstream style;
    style << "left:" << css_percent(pct(x, geometry.view_width)) << ";";
    style << "top:" << css_percent(pct(y, geometry.view_height)) << ";";
    style << "width:" << css_percent(pct(width, geometry.view_width)) << ";";
    style << "height:" << css_percent(pct(height, geometry.view_height)) << ";";
    return style.str();
}

std::string button_caption_anchor_style(const Object& properties, const ButtonSvgGeometry& geometry) {
    const auto x = property_number(properties, "caption.anchor.x").value_or(geometry.caption_x);
    const auto y = property_number(properties, "caption.anchor.y").value_or(geometry.caption_y);
    const auto align = property_string(properties, "caption.align.horizontal", "left");
    std::ostringstream style;
    style << button_anchor_style(x, y, geometry);
    style << "transform:" << caption_transform_for_align(align) << ";";
    style << "text-align:" << caption_text_align(align) << ";";
    style << "color:" << html_escape(safe_css_color(property_string(properties, "caption.style.text_color", "#111827"), "#111827")) << ";";
    if (!property_bool(properties, "caption.visible", true)) {
        style << "display:none;";
    }
    return style.str();
}

std::string inline_svg_asset(const WidgetState& widget) {
    if (widget.asset_path.empty() || !std::filesystem::exists(widget.asset_path)) {
        throw std::runtime_error("Button widget " + widget.widget_id + " requires an existing .wfrog SVG asset.");
    }
    return read_text_file(widget.asset_path);
}

std::string render_button_widget(const WidgetState& widget) {
    const auto geometry = load_button_svg_geometry(widget);
    const auto x = layout_i64(widget.layout, "x", 0);
    const auto y = layout_i64(widget.layout, "y", 0);
    const auto width = layout_i64(widget.layout, "width", 220);
    const auto height = layout_i64(widget.layout, "height", 140);
    const auto mechanical_action = property_string(widget.properties, "behavior.mechanical_action", "");
    const bool physical_pressed = property_bool(widget.properties, "pressed", false);
    const bool value = mechanical_action == "switch_until_released"
        ? physical_pressed
        : property_bool(widget.properties, "value", property_bool(widget.properties, "pressed", false));
    const std::string visual_state = value ? "true" : "false";
    const std::string hover_state = value ? "hover_true" : "hover_false";
    const std::string pressed_state = value ? "pressed_true" : "pressed_false";
    const std::string transition_state = value ? "transition_true_to_false" : "transition_false_to_true";
    const std::string caption = property_string(widget.properties, "caption.text", widget.widget_id);
    const std::string false_state_text = property_string(widget.properties, "state_text.false_text", "OFF");
    const std::string true_state_text = property_string(widget.properties, "state_text.true_text", "ON");
    const std::string state_text = value ? true_state_text : false_state_text;
    const std::string route = asset_route(widget);
    const auto input_id = property_string(widget.properties, "binding.public_input_id", "");

    const auto frame_fill = safe_css_color(property_string(widget.properties, "style.frame.fill_color", "transparent"), "transparent");
    const auto frame_stroke = safe_css_color(property_string(widget.properties, "style.frame.border_color", "transparent"), "transparent");
    const auto frame_width = safe_css_length(property_string(widget.properties, "style.frame.border_width", "0px"), "0px");
    const auto face_fill_false = safe_css_color(state_property(widget.properties, "style.face.fill_color", "false", "#e2e8f0"), "#e2e8f0");
    const auto face_fill_true = safe_css_color(state_property(widget.properties, "style.face.fill_color", "true", face_fill_false), face_fill_false);
    const auto face_fill = value ? face_fill_true : face_fill_false;
    const auto face_hover_fill_false = safe_css_color(state_property(widget.properties, "style.face.fill_color", "hover_false", face_fill_false), face_fill_false);
    const auto face_hover_fill_true = safe_css_color(state_property(widget.properties, "style.face.fill_color", "hover_true", face_fill_true), face_fill_true);
    const auto face_hover_fill = value ? face_hover_fill_true : face_hover_fill_false;
    const auto face_pressed_fill_false = safe_css_color(state_property(widget.properties, "style.face.fill_color", "pressed_false", face_fill_false), face_fill_false);
    const auto face_pressed_fill_true = safe_css_color(state_property(widget.properties, "style.face.fill_color", "pressed_true", face_fill_true), face_fill_true);
    const auto face_pressed_fill = value ? face_pressed_fill_true : face_pressed_fill_false;
    const auto face_stroke = safe_css_color(state_property(widget.properties, "style.face.border_color", visual_state, "#334155"), "#334155");
    const auto face_stroke_width = safe_css_length(property_string(widget.properties, "style.face.border_width", "1px"), "1px");
    const auto state_face_fill_false = safe_css_color(state_property(widget.properties, "style.state_face.fill_color", "false", "transparent"), "transparent");
    const auto state_face_fill_true = safe_css_color(state_property(widget.properties, "style.state_face.fill_color", "true", state_face_fill_false), state_face_fill_false);
    const auto state_face_fill = value ? state_face_fill_true : state_face_fill_false;
    const auto state_face_hover_fill_false = safe_css_color(state_property(widget.properties, "style.state_face.fill_color", "hover_false", state_face_fill_false), state_face_fill_false);
    const auto state_face_hover_fill_true = safe_css_color(state_property(widget.properties, "style.state_face.fill_color", "hover_true", state_face_fill_true), state_face_fill_true);
    const auto state_face_hover_fill = value ? state_face_hover_fill_true : state_face_hover_fill_false;
    const auto state_face_pressed_fill_false = safe_css_color(state_property(widget.properties, "style.state_face.fill_color", "pressed_false", state_face_fill_false), state_face_fill_false);
    const auto state_face_pressed_fill_true = safe_css_color(state_property(widget.properties, "style.state_face.fill_color", "pressed_true", state_face_fill_true), state_face_fill_true);
    const auto state_face_pressed_fill = value ? state_face_pressed_fill_true : state_face_pressed_fill_false;
    const auto state_face_stroke_false = safe_css_color(state_property(widget.properties, "style.state_face.border_color", "false", "transparent"), "transparent");
    const auto state_face_stroke_true = safe_css_color(state_property(widget.properties, "style.state_face.border_color", "true", state_face_stroke_false), state_face_stroke_false);
    const auto state_face_stroke = value ? state_face_stroke_true : state_face_stroke_false;
    const auto state_face_hover_stroke_false = safe_css_color(state_property(widget.properties, "style.state_face.border_color", "hover_false", state_face_stroke_false), state_face_stroke_false);
    const auto state_face_hover_stroke_true = safe_css_color(state_property(widget.properties, "style.state_face.border_color", "hover_true", state_face_stroke_true), state_face_stroke_true);
    const auto state_face_hover_stroke = value ? state_face_hover_stroke_true : state_face_hover_stroke_false;
    const auto state_face_pressed_stroke_false = safe_css_color(state_property(widget.properties, "style.state_face.border_color", "pressed_false", state_face_stroke_false), state_face_stroke_false);
    const auto state_face_pressed_stroke_true = safe_css_color(state_property(widget.properties, "style.state_face.border_color", "pressed_true", state_face_stroke_true), state_face_stroke_true);
    const auto state_face_pressed_stroke = value ? state_face_pressed_stroke_true : state_face_pressed_stroke_false;
    const auto state_face_stroke_width = safe_css_length(property_string(widget.properties, "style.state_face.border_width", "0px"), "0px");
    const auto caption_size = safe_css_length(property_string(widget.properties, "caption.style.font_size", "18px"), "18px");
    const auto caption_weight = safe_css_font_weight(property_string(widget.properties, "caption.style.font_weight", "600"), "600");
    const auto caption_family = safe_css_font_family(
        property_string(widget.properties, "caption.style.font_family", "system-ui, Segoe UI, Arial, sans-serif"),
        "system-ui, Segoe UI, Arial, sans-serif");
    const auto false_text_color = safe_css_color(state_property(widget.properties, "state_text.style.text_color", "false", "#111827"), "#111827");
    const auto true_text_color = safe_css_color(state_property(widget.properties, "state_text.style.text_color", "true", "#06381c"), "#06381c");
    const auto text_color = value ? true_text_color : false_text_color;
    const auto text_size = safe_css_length(property_string(widget.properties, "state_text.style.font_size", "20px"), "20px");
    const auto text_weight = safe_css_font_weight(property_string(widget.properties, "state_text.style.font_weight", "400"), "400");
    const auto focus_color = safe_css_color(property_string(widget.properties, "style.focus_ring.color", "#2563eb"), "#2563eb");
    const auto focus_width = property_bool(widget.properties, "style.focus_ring.visible", true)
        ? safe_css_length(property_string(widget.properties, "style.focus_ring.width", "3px"), "3px")
        : "0px";
    const auto pressed_inset = safe_css_length(property_string(widget.properties, "style.pressed.inset", "2px"), "2px");
    const bool pressed_applies_when_value_true = property_bool(widget.properties, "style.pressed.apply_when_value_true", false);
    const bool pressed_applies_while_active = property_bool(widget.properties, "style.pressed.apply_while_active", true);
    const bool hover_applies_when_value_false_only = property_bool(widget.properties, "style.hover.apply_when_value_false_only", false);
    const auto transition_ms = property_string(widget.properties, "style.transition.duration_ms", "120");
    const auto transition_timing = property_string(widget.properties, "style.transition.timing", "ease-out");
    const bool state_text_visible = property_bool(widget.properties, "state_text.visible", true);
    int output_pulse_duration_ms = 0;
    if (const auto source_duration = property_number(widget.properties, "behavior.output_pulse.duration_ms")) {
        output_pulse_duration_ms = std::clamp(static_cast<int>(*source_duration), 0, 5000);
    }

    std::ostringstream html;
    html << "<div class='frog-widget button-widget button-control'";
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
    html << " data-frog-physical-pressed='" << (physical_pressed ? "true" : "false") << "'";
    html << " data-frog-mechanical-action='" << html_escape(mechanical_action) << "'";
    html << " data-frog-output-pulse-duration-ms='" << output_pulse_duration_ms << "'";
    html << " data-realization-variant='" << html_escape(property_string(widget.properties, "realization.variant", "rectangular")) << "'";
    html << " data-frog-visual-law='wfrog-realization-state-map'";
    html << " data-frog-visual-state='" << html_escape(visual_state) << "'";
    html << " data-frog-hover-state='" << html_escape(hover_state) << "'";
    html << " data-frog-pressed-state='" << html_escape(pressed_state) << "'";
    html << " data-frog-transition-state='" << html_escape(transition_state) << "'";
    html << " data-frog-pressed-applies-when-value-true='" << (pressed_applies_when_value_true ? "true" : "false") << "'";
    html << " data-frog-pressed-applies-while-active='" << (pressed_applies_while_active ? "true" : "false") << "'";
    html << " data-frog-hover-applies-when-value-false-only='" << (hover_applies_when_value_false_only ? "true" : "false") << "'";
    html << " data-frog-state-text-visible='" << (state_text_visible ? "true" : "false") << "'";
    html << " data-frog-state-text-false='" << html_escape(false_state_text) << "'";
    html << " data-frog-state-text-true='" << html_escape(true_state_text) << "'";
    html << " data-frog-state-text-color-false='" << html_escape(false_text_color) << "'";
    html << " data-frog-state-text-color-true='" << html_escape(true_text_color) << "'";
    html << " data-frog-button-face-fill-false='" << html_escape(face_fill_false) << "'";
    html << " data-frog-button-face-fill-true='" << html_escape(face_fill_true) << "'";
    html << " data-frog-button-face-hover-fill-false='" << html_escape(face_hover_fill_false) << "'";
    html << " data-frog-button-face-hover-fill-true='" << html_escape(face_hover_fill_true) << "'";
    html << " data-frog-button-face-pressed-fill-false='" << html_escape(face_pressed_fill_false) << "'";
    html << " data-frog-button-face-pressed-fill-true='" << html_escape(face_pressed_fill_true) << "'";
    html << " data-frog-button-state-face-fill-false='" << html_escape(state_face_fill_false) << "'";
    html << " data-frog-button-state-face-fill-true='" << html_escape(state_face_fill_true) << "'";
    html << " data-frog-button-state-face-hover-fill-false='" << html_escape(state_face_hover_fill_false) << "'";
    html << " data-frog-button-state-face-hover-fill-true='" << html_escape(state_face_hover_fill_true) << "'";
    html << " data-frog-button-state-face-pressed-fill-false='" << html_escape(state_face_pressed_fill_false) << "'";
    html << " data-frog-button-state-face-pressed-fill-true='" << html_escape(state_face_pressed_fill_true) << "'";
    html << " data-frog-button-state-face-stroke-false='" << html_escape(state_face_stroke_false) << "'";
    html << " data-frog-button-state-face-stroke-true='" << html_escape(state_face_stroke_true) << "'";
    html << " data-frog-button-state-face-hover-stroke-false='" << html_escape(state_face_hover_stroke_false) << "'";
    html << " data-frog-button-state-face-hover-stroke-true='" << html_escape(state_face_hover_stroke_true) << "'";
    html << " data-frog-button-state-face-pressed-stroke-false='" << html_escape(state_face_pressed_stroke_false) << "'";
    html << " data-frog-button-state-face-pressed-stroke-true='" << html_escape(state_face_pressed_stroke_true) << "'";
    html << " style='position:absolute;left:" << css_px(x) << ";top:" << css_px(y)
         << ";width:" << css_px(width) << ";height:" << css_px(height) << ";"
         << "--frog-button-frame-fill:" << html_escape(frame_fill) << ";"
         << "--frog-button-frame-stroke:" << html_escape(frame_stroke) << ";"
         << "--frog-button-frame-stroke-width:" << html_escape(frame_width) << ";"
         << "--frog-button-face-fill:" << html_escape(face_fill) << ";"
         << "--frog-button-face-hover-fill:" << html_escape(face_hover_fill) << ";"
         << "--frog-button-face-pressed-fill:" << html_escape(face_pressed_fill) << ";"
         << "--frog-button-face-stroke:" << html_escape(face_stroke) << ";"
         << "--frog-button-face-stroke-width:" << html_escape(face_stroke_width) << ";"
         << "--frog-button-state-face-fill:" << html_escape(state_face_fill) << ";"
         << "--frog-button-state-face-hover-fill:" << html_escape(state_face_hover_fill) << ";"
         << "--frog-button-state-face-pressed-fill:" << html_escape(state_face_pressed_fill) << ";"
         << "--frog-button-state-face-stroke:" << html_escape(state_face_stroke) << ";"
         << "--frog-button-state-face-hover-stroke:" << html_escape(state_face_hover_stroke) << ";"
         << "--frog-button-state-face-pressed-stroke:" << html_escape(state_face_pressed_stroke) << ";"
         << "--frog-button-state-face-stroke-width:" << html_escape(state_face_stroke_width) << ";"
         << "--frog-button-caption-font-size:" << html_escape(caption_size) << ";"
         << "--frog-button-caption-font-weight:" << html_escape(caption_weight) << ";"
         << "--frog-button-caption-font-family:" << html_escape(caption_family) << ";"
         << "--frog-button-state-text-fill:" << html_escape(text_color) << ";"
         << "--frog-button-state-text-font-size:" << html_escape(text_size) << ";"
         << "--frog-button-state-text-font-weight:" << html_escape(text_weight) << ";"
         << "--frog-button-focus-color:" << html_escape(focus_color) << ";"
         << "--frog-button-focus-width:" << html_escape(focus_width) << ";"
         << "--frog-button-pressed-inset:" << html_escape(pressed_inset) << ";"
         << "--frog-button-transition:" << html_escape(transition_ms) << "ms " << html_escape(transition_timing) << ";";
    if (!property_bool(widget.properties, "visible", true)) {
        html << "display:none;";
    }
    html << "'>";
    html << "<div class='button-skin' data-frog-asset-consumed='true' aria-hidden='true'>" << inline_svg_asset(widget) << "</div>";
    html << "<span class='button-caption-overlay' data-frog-part='caption' data-svg-anchor='caption.anchor' style='"
         << button_caption_anchor_style(widget.properties, geometry) << "'>" << html_escape(caption) << "</span>";
    if (state_text_visible) {
        html << "<span class='button-state-overlay' data-frog-part='state_text' data-svg-anchor='state_text.center' style='"
             << button_anchor_style(geometry.state_text_x, geometry.state_text_y, geometry) << "'>" << html_escape(state_text) << "</span>";
    }
    html << "<button class='button-press-overlay' type='button'";
    html << " name='" << html_escape(input_id) << "' value='true'";
    html << " aria-label='" << html_escape(caption) << "'";
    html << " aria-pressed='" << (physical_pressed ? "true" : "false") << "'";
    html << " data-frog-part='face' data-frog-event='pressed' data-frog-public-input-id='" << html_escape(input_id) << "'";
    html << " data-frog-host-overlay='input' data-frog-align-to-part='face'";
    html << " style='" << button_box_style(geometry.face_x, geometry.face_y, geometry.face_width, geometry.face_height, geometry) << "'>";
    html << "</button></div>";
    return html.str();
}

std::string button_widget_script() {
    return R"FROGJS(<script>
(() => {
  const form = document.querySelector("form[action='/run']");
  const overlay = document.querySelector(".button-press-overlay[data-frog-part='face'][data-frog-host-overlay='input']");
  const buttonWidget = overlay ? overlay.closest(".button-widget[data-class-ref='frog.widgets.button']") : null;
  const indicator = document.querySelector(".boolean-indicator[data-class-ref='frog.widgets.boolean_indicator']");
  const readButton = document.querySelector(".program-read-action[data-frog-event='read']");
  const readStatus = document.querySelector(".program-read-status");
  if (!form || !buttonWidget || !overlay || !indicator) {
    return;
  }

  const buttonStateText = buttonWidget.querySelector(".button-state-overlay[data-frog-part='state_text']");
  const stateText = indicator.querySelector("[data-frog-part='state_text']");
  const inputId = overlay.dataset.frogPublicInputId || overlay.name || "";
  const mechanicalAction = buttonWidget.dataset.frogMechanicalAction || "";
  const latchAction = mechanicalAction === "latch_when_pressed" ||
    mechanicalAction === "latch_when_released" ||
    mechanicalAction === "latch_until_released";
  const pulseDurationMs = Math.max(0, Math.min(5000, Number.parseInt(buttonWidget.dataset.frogOutputPulseDurationMs || "0", 10) || 0));
  if (!inputId || (
      mechanicalAction !== "switch_until_released" &&
      mechanicalAction !== "switch_when_pressed" &&
      mechanicalAction !== "switch_when_released" &&
      mechanicalAction !== "latch_when_pressed" &&
      mechanicalAction !== "latch_when_released" &&
      mechanicalAction !== "latch_until_released")) {
    return;
  }
  let eventQueue = Promise.resolve();
  let pulseResetTimer = 0;

  const buttonProperty = (base, value) => {
    const suffix = value ? "True" : "False";
    return buttonWidget.dataset[`${base}${suffix}`] || "";
  };

  const indicatorProperty = (base, value) => {
    const suffix = value ? "True" : "False";
    return indicator.dataset[`${base}${suffix}`] || "";
  };

  const applyIndicator = (value) => {
    const state = value ? "true" : "false";
    indicator.dataset.currentValue = state;
    indicator.dataset.frogVisualState = state;
    indicator.dataset.frogHoverState = value ? "hover_true" : "hover_false";
    indicator.dataset.frogPressedState = value ? "pressed_true" : "pressed_false";
    indicator.dataset.frogTransitionState = value ? "transition_false_to_true" : "transition_true_to_false";
    indicator.style.setProperty("--boolean-fill", indicatorProperty("frogFill", value));
    indicator.style.setProperty("--boolean-inner-border", indicatorProperty("frogInnerBorder", value));
    indicator.style.setProperty("--boolean-text", indicatorProperty("frogTextColor", value));
    if (stateText) {
      stateText.textContent = indicatorProperty("frogText", value);
    }
  };

  const applyButton = (value, physicalPressed) => {
    overlay.setAttribute("aria-pressed", physicalPressed ? "true" : "false");
    buttonWidget.dataset.frogPhysicalPressed = physicalPressed ? "true" : "false";
    buttonWidget.dataset.currentValue = value ? "true" : "false";
    buttonWidget.dataset.frogVisualState = value ? "true" : "false";
    buttonWidget.dataset.frogHoverState = value ? "hover_true" : "hover_false";
    buttonWidget.dataset.frogPressedState = value ? "pressed_true" : "pressed_false";
    buttonWidget.style.setProperty("--frog-button-face-fill", buttonProperty("frogButtonFaceFill", value));
    buttonWidget.style.setProperty("--frog-button-face-hover-fill", buttonProperty("frogButtonFaceHoverFill", value));
    buttonWidget.style.setProperty("--frog-button-face-pressed-fill", buttonProperty("frogButtonFacePressedFill", value));
    buttonWidget.style.setProperty("--frog-button-state-face-fill", buttonProperty("frogButtonStateFaceFill", value));
    buttonWidget.style.setProperty("--frog-button-state-face-hover-fill", buttonProperty("frogButtonStateFaceHoverFill", value));
    buttonWidget.style.setProperty("--frog-button-state-face-pressed-fill", buttonProperty("frogButtonStateFacePressedFill", value));
    buttonWidget.style.setProperty("--frog-button-state-face-stroke", buttonProperty("frogButtonStateFaceStroke", value));
    buttonWidget.style.setProperty("--frog-button-state-face-hover-stroke", buttonProperty("frogButtonStateFaceHoverStroke", value));
    buttonWidget.style.setProperty("--frog-button-state-face-pressed-stroke", buttonProperty("frogButtonStateFacePressedStroke", value));
    buttonWidget.style.setProperty("--frog-button-state-text-fill", buttonProperty("frogStateTextColor", value));
    if (buttonStateText) {
      buttonStateText.textContent = buttonProperty("frogStateText", value);
      buttonStateText.style.color = buttonProperty("frogStateTextColor", value);
    }
  };

  const applyArtifact = (artifact) => {
    const ui = artifact && artifact.outputs && artifact.outputs.ui ? artifact.outputs.ui : {};
    const summary = artifact && artifact.execution_summary ? artifact.execution_summary : {};
    const buttonValue = Boolean(ui[buttonWidget.dataset.widgetId]);
    const indicatorValue = Boolean(ui[indicator.dataset.widgetId]);
    const latchPulseVisible = latchAction &&
      pulseDurationMs > 0 &&
      summary.program_read_performed &&
      summary.program_read_value;
    applyButton(latchPulseVisible ? true : buttonValue, Boolean(summary.button_physical_pressed));
    applyIndicator(indicatorValue);
    if (readStatus && summary.program_read_performed) {
      readStatus.textContent = `Last read: ${summary.program_read_value ? "TRUE" : "FALSE"}`;
      readStatus.dataset.frogLastRead = summary.program_read_value ? "true" : "false";
    }
    const shouldResetLatchPulse = latchPulseVisible &&
      (mechanicalAction === "latch_when_pressed" || !summary.button_physical_pressed);
    if (shouldResetLatchPulse) {
      window.clearTimeout(pulseResetTimer);
      pulseResetTimer = window.setTimeout(() => publishEvent("read"), pulseDurationMs);
    }
  };

  const publishEvent = (eventName) => {
    const body = new URLSearchParams();
    body.set("frog_event", eventName);
    body.set(inputId, buttonWidget.dataset.currentValue === "true" ? "true" : "false");
    eventQueue = eventQueue
      .catch(() => {})
      .then(() => fetch("/event", {
        method: "POST",
        headers: {"Content-Type": "application/x-www-form-urlencoded;charset=UTF-8"},
        body
      }))
      .then((response) => response.ok ? response.json() : null)
      .then((artifact) => {
        if (artifact) {
          applyArtifact(artifact);
        }
      })
      .catch(() => {});
  };

  const press = (event) => {
    if (event && event.button !== undefined && event.button !== 0) {
      return;
    }
    if (event) {
      event.preventDefault();
    }
    publishEvent("press");
  };

  const release = (event) => {
    if (event) {
      event.preventDefault();
    }
    publishEvent("release");
  };

  form.addEventListener("submit", (event) => event.preventDefault());
  overlay.addEventListener("click", (event) => event.preventDefault());

  let armed = false;
  const arm = (event) => {
    armed = true;
    press(event);
  };
  const releaseIfArmed = (event) => {
    if (!armed) {
      return;
    }
    armed = false;
    release(event);
  };
  const cancel = (event) => {
    if (event) {
      event.preventDefault();
    }
    armed = false;
  };

  if (window.PointerEvent) {
    overlay.addEventListener("pointerdown", (event) => {
      if (overlay.setPointerCapture) {
        overlay.setPointerCapture(event.pointerId);
      }
      arm(event);
    });
    overlay.addEventListener("pointerup", releaseIfArmed);
    overlay.addEventListener("pointercancel", cancel);
    overlay.addEventListener("lostpointercapture", cancel);
  } else {
    overlay.addEventListener("mousedown", arm);
    window.addEventListener("mouseup", releaseIfArmed);
    overlay.addEventListener("touchstart", arm, {passive: false});
    overlay.addEventListener("touchend", releaseIfArmed, {passive: false});
    overlay.addEventListener("touchcancel", cancel, {passive: false});
  }
  overlay.addEventListener("blur", cancel);
  overlay.addEventListener("keydown", (event) => {
    if ((event.key !== " " && event.key !== "Enter") || event.repeat) {
      return;
    }
    armed = true;
    press(event);
  });
  overlay.addEventListener("keyup", (event) => {
    if (event.key !== " " && event.key !== "Enter") {
      return;
    }
    releaseIfArmed(event);
  });
  if (readButton) {
    readButton.addEventListener("click", (event) => {
      event.preventDefault();
      publishEvent("read");
    });
  }
})();
</script>)FROGJS";
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
            ".numeric-label-overlay{position:absolute;transform:translateY(-50%);font-size:var(--frog-numeric-caption-font-size);font-family:var(--frog-numeric-caption-font-family);font-weight:var(--frog-numeric-caption-font-weight);color:var(--frog-numeric-caption-color);line-height:1;white-space:nowrap;pointer-events:none;}"
            ".numeric-value-overlay{position:absolute;box-sizing:border-box;font-family:var(--frog-numeric-text-font-family);font-size:var(--frog-numeric-text-font-size);font-weight:var(--frog-numeric-text-font-weight);color:var(--frog-numeric-text-color);line-height:1;border:0;background:transparent;}"
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
    html << "<p class='meta'>Example 05 - .frog front panel + Default Numeric .wfrog realization assets + C++ runtime</p>";
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
    html << "</form>";
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
            ".boolean-caption-overlay{position:absolute;left:0;top:0;transform:translateY(-50%);text-align:left;font-size:var(--boolean-caption-font-size);font-weight:var(--boolean-caption-font-weight);font-family:var(--boolean-caption-font-family);line-height:1;color:var(--boolean-caption-color);white-space:nowrap;pointer-events:none;z-index:3;}"
            ".boolean-state-face{position:absolute;left:var(--boolean-inner-left);top:var(--boolean-inner-top);width:var(--boolean-inner-width);height:var(--boolean-inner-height);border:var(--boolean-inner-border-width) solid var(--boolean-inner-border);border-radius:7px;background:var(--boolean-fill);box-shadow:inset 0 1px 0 rgba(255,255,255,.6),0 1px 2px rgba(15,23,42,.16);transition:background var(--boolean-transition),border-color var(--boolean-transition),box-shadow var(--boolean-transition),transform var(--boolean-transition);z-index:1;}"
            ".boolean-widget[data-realization-variant='circular'] .boolean-state-face{border-radius:50%;}"
            ".boolean-widget[data-frog-frame-visible='false'] .boolean-state-face{box-shadow:none;}"
            ".boolean-control:hover .boolean-state-face{background:var(--boolean-hover-fill);border-color:var(--boolean-hover-inner-border);box-shadow:inset 0 1px 0 rgba(255,255,255,.72),0 2px 5px rgba(15,23,42,.18);}"
            ".boolean-control[data-frog-frame-visible='false']:hover .boolean-state-face{box-shadow:none;}"
            ".boolean-control:active .boolean-state-face{background:var(--boolean-pressed-fill);border-color:var(--boolean-pressed-inner-border);box-shadow:inset 0 2px 4px rgba(15,23,42,.22);transform:translateY(var(--boolean-pressed-inset));}"
            ".boolean-control[data-frog-frame-visible='false']:active .boolean-state-face{box-shadow:none;}"
            ".boolean-control:focus-visible .boolean-state-face{outline:var(--boolean-focus-width) solid var(--boolean-focus-color);}"
            ".boolean-state-overlay{position:absolute;transform:translate(-50%,-50%);text-align:center;font-size:var(--boolean-text-font-size);font-weight:var(--boolean-text-font-weight);line-height:1;color:var(--boolean-text);pointer-events:none;z-index:4;white-space:nowrap;}"
            ".actions{margin-top:16px;display:flex;gap:12px;align-items:center;}"
            ".state-link{font-size:16px;}"
            ".diagnostic{margin:12px 0;padding:10px 12px;border-radius:6px;}"
            ".diagnostic.error{background:#fff1f2;color:#9f1239;border:1px solid #fecdd3;}"
            "</style></head><body>";
    html << "<h1>" << html_escape(core.panel.title) << "</h1>";
    html << "<p class='meta'>Example 06 - .frog front panel + Default Boolean .wfrog realization assets + C++ runtime</p>";
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
            ".front-panel{position:relative;background:#ffffff;border-radius:10px;box-shadow:0 4px 14px rgba(15,23,42,0.08);overflow:visible;}"
            ".frog-widget{position:absolute;box-sizing:border-box;}"
            ".string-widget{font-family:Segoe UI,Arial,sans-serif;}"
            ".string-skin{position:absolute;inset:0;width:100%;height:100%;display:block;}"
            ".string-skin svg{width:100%;height:100%;display:block;--frog-string-label-display:inherit;--frog-string-caption-display:inherit;--frog-string-placeholder-display:inherit;--frog-string-frame-fill:inherit;--frog-string-frame-stroke:inherit;--frog-string-frame-stroke-width:inherit;--frog-string-text-region-fill:inherit;--frog-string-text-region-stroke:inherit;--frog-string-text-region-stroke-width:inherit;--frog-string-text-fill:inherit;--frog-string-text-font-size:inherit;--frog-string-text-font-weight:inherit;}"
            ".string-skin #label_text,.string-skin #caption_text,.string-skin #placeholder,.string-skin #text_value{display:none;}"
            ".string-control:hover .string-skin svg{--frog-string-text-region-fill:var(--frog-string-text-region-fill-hover);--frog-string-text-region-stroke:var(--frog-string-text-region-stroke-hover);--frog-string-text-region-stroke-width:var(--frog-string-text-region-stroke-width-hover);}"
            ".string-caption-overlay{position:absolute;transform:translateY(-50%);font-size:var(--frog-string-caption-font-size);font-weight:var(--frog-string-caption-font-weight);font-family:var(--frog-string-caption-font-family);color:var(--frog-string-caption-color);line-height:1;white-space:nowrap;pointer-events:none;}"
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
    html << "<p class='meta'>Example 07 - .frog front panel + Default String .wfrog realization assets + C++ runtime</p>";
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

EnumBrowserUiRuntime::EnumBrowserUiRuntime(
    std::filesystem::path contract_path,
    std::filesystem::path wfrog_path,
    std::shared_ptr<const NativeEnumKernelBridge> native_kernel_bridge_)
    : core(std::move(contract_path), std::move(wfrog_path)),
      native_kernel_bridge(std::move(native_kernel_bridge_)) {}

frog::json::Value EnumBrowserUiRuntime::run_once(const std::string& input_value) {
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

std::string EnumBrowserUiRuntime::render_html() const {
    const auto& ctrl = core.widgets.at("mode_input");
    const auto& ind = core.widgets.at("mode_result");
    const auto panel_width = layout_i64(core.panel.layout, "width", 620);
    const auto panel_height = layout_i64(core.panel.layout, "height", 180);
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
            ".enum-widget{font-family:Segoe UI,Arial,sans-serif;}"
            ".enum-skin{position:absolute;inset:0;width:100%;height:100%;display:block;}"
            ".enum-skin svg{width:100%;height:100%;display:block;}"
            ".enum-skin #label_text,.enum-skin #caption_text,.enum-skin #value_display{display:none;}"
            ".enum-caption-overlay{position:absolute;transform:translateY(-50%);font-size:var(--frog-enum-caption-font-size);font-weight:var(--frog-enum-caption-font-weight);font-family:var(--frog-enum-caption-font-family);color:var(--frog-enum-caption-color);line-height:1;white-space:nowrap;pointer-events:none;}"
            ".enum-value-overlay{position:absolute;box-sizing:border-box;font-family:Segoe UI,Arial,sans-serif;line-height:1.2;border:0;background:transparent;}"
            ".enum-value-display-overlay{position:absolute;box-sizing:border-box;display:flex;align-items:center;padding:0 var(--frog-enum-text-padding-inline);font-family:Segoe UI,Arial,sans-serif;line-height:normal;transform:translateY(var(--frog-enum-text-vertical-offset));z-index:3;}"
            ".enum-widget .enum-display-button{border:0;background:transparent;text-align:left;justify-content:flex-start;appearance:none;cursor:pointer;}"
            ".enum-widget .enum-display-button:focus,.enum-widget .enum-display-button:focus-visible,.enum-widget .enum-display-button:active{outline:0;box-shadow:none;}"
            ".enum-select-state{display:none;}"
            ".enum-selector-overlay{position:absolute;box-sizing:border-box;display:flex;align-items:center;justify-content:center;border-style:solid;border-width:var(--frog-enum-selector-stroke-width);border-radius:var(--frog-enum-selector-radius);}"
            ".enum-selector-overlay::after{content:'';width:0;height:0;border-left:calc(var(--frog-enum-selector-symbol-width) / 2) solid transparent;border-right:calc(var(--frog-enum-selector-symbol-width) / 2) solid transparent;border-top:var(--frog-enum-selector-symbol-height) solid currentColor;}"
            ".enum-control:has(.enum-display-button:hover) .enum-skin #value_face,.enum-control:has(.enum-dropdown:not([hidden])) .enum-skin #value_face{fill:var(--frog-enum-value-hover-fill) !important;}"
            ".enum-indicator-value{display:flex;align-items:center;padding:0 var(--frog-enum-text-padding-inline);pointer-events:none;line-height:normal;transform:translateY(var(--frog-enum-text-vertical-offset));}"
            ".enum-dropdown{position:absolute;box-sizing:border-box;z-index:30;background:var(--frog-enum-dropdown-fill);border:var(--frog-enum-dropdown-border-width) solid var(--frog-enum-dropdown-border);}"
            ".enum-dropdown[hidden]{display:none;}"
            ".enum-dropdown-option{width:100%;min-height:var(--frog-enum-dropdown-option-height);display:flex;align-items:center;justify-content:flex-start;padding:0 var(--frog-enum-dropdown-option-padding-inline);border:0;border-radius:0;background:var(--frog-enum-dropdown-option-fill);color:var(--frog-enum-dropdown-option-text);font-family:var(--frog-enum-dropdown-option-font-family);font-size:var(--frog-enum-dropdown-option-font-size);font-weight:var(--frog-enum-dropdown-option-font-weight);font-style:var(--frog-enum-dropdown-option-font-style);text-align:left;cursor:pointer;}"
            ".enum-dropdown-option:hover,.enum-dropdown-option:focus{background:var(--frog-enum-dropdown-option-hover-fill);color:var(--frog-enum-dropdown-option-hover-text);outline:0;}"
            ".enum-dropdown-option[aria-selected='true']{background:var(--frog-enum-dropdown-option-selected-fill);color:var(--frog-enum-dropdown-option-selected-text);}"
            ".actions{margin-top:16px;display:flex;gap:12px;align-items:center;}"
            "button{padding:8px 14px;border:0;border-radius:6px;cursor:pointer;background:#0f62fe;color:#ffffff;font-weight:600;}"
            ".enum-widget .enum-selector-button{padding:0;border-style:solid;border-width:var(--frog-enum-selector-stroke-width);border-radius:var(--frog-enum-selector-radius);font-weight:400;cursor:pointer;appearance:none;z-index:4;background:var(--frog-enum-selector-fill);border-color:var(--frog-enum-selector-stroke);color:var(--frog-enum-selector-symbol);}"
            ".enum-widget .enum-selector-button:hover{background:var(--frog-enum-selector-hover-fill);border-color:var(--frog-enum-selector-hover-stroke);color:var(--frog-enum-selector-hover-symbol);}"
            ".enum-widget .enum-selector-button:focus,.enum-widget .enum-selector-button:focus-visible,.enum-widget .enum-selector-button:active{outline:0;box-shadow:none;}"
            ".diagnostic{margin:12px 0;padding:10px 12px;border-radius:6px;}"
            ".diagnostic.error{background:#fff1f2;color:#9f1239;border:1px solid #fecdd3;}"
            "</style><script>"
            "function frogCloseEnumDropdown(menuId,buttonId){const m=document.getElementById(menuId);if(m){m.hidden=true;}const b=document.getElementById(buttonId);if(b){b.setAttribute('aria-expanded','false');}}"
            "function frogCloseOtherEnumDropdowns(menuId){document.querySelectorAll('.enum-dropdown').forEach(function(m){if(m.id!==menuId){m.hidden=true;}});document.querySelectorAll('.enum-display-button,.enum-selector-button').forEach(function(b){if(b.getAttribute('aria-controls')!==menuId){b.setAttribute('aria-expanded','false');}});}"
            "function frogToggleEnumDropdown(menuId,buttonId){const m=document.getElementById(menuId);const b=document.getElementById(buttonId);if(!m){return;}const opening=m.hidden;frogCloseOtherEnumDropdowns(menuId);m.hidden=!opening;if(b){b.setAttribute('aria-expanded',opening?'true':'false');}}"
            "function frogUpdateEnumDisplay(select,displayId){const d=document.getElementById(displayId);if(!d){return;}const o=select.options[select.selectedIndex];if(o){d.textContent=o.textContent;}const menu=document.getElementById(select.id.replace('_value','_dropdown'));if(menu){menu.querySelectorAll('.enum-dropdown-option').forEach(function(option){option.setAttribute('aria-selected',option.getAttribute('data-enum-value')===select.value?'true':'false');});}}"
            "function frogSelectEnumOption(option,selectId,displayId,menuId){const s=document.getElementById(selectId);const d=document.getElementById(displayId);if(!s||!option){return;}const value=option.getAttribute('data-enum-value');s.value=value;if(d){d.textContent=option.textContent;}frogUpdateEnumDisplay(s,displayId);frogCloseEnumDropdown(menuId,displayId);s.dispatchEvent(new Event('input',{bubbles:true}));s.dispatchEvent(new Event('change',{bubbles:true}));}"
            "document.addEventListener('click',function(event){if(!event.target.closest('.enum-widget')){document.querySelectorAll('.enum-dropdown').forEach(function(m){m.hidden=true;});document.querySelectorAll('.enum-display-button,.enum-selector-button').forEach(function(b){b.setAttribute('aria-expanded','false');});}});"
            "</script></head><body>";
    html << "<h1>" << html_escape(core.panel.title) << "</h1>";
    html << "<p class='meta'>Example 08 - .frog front panel + Default Enum .wfrog realization assets + C++ runtime</p>";
    html << "<dl class='runtime-facts' aria-label='Runtime facts'>";
    html << "<div><dt>Runtime</dt><dd>C++ reference runtime</dd></div>";
    html << "<div><dt>Execution</dt><dd>" << (uses_native_kernel ? "native kernel bridge" : "enum contract executor") << "</dd></div>";
    html << "<div><dt>Compiler backend</dt><dd>" << (uses_native_kernel ? "LLVM native enum kernel artifact" : "none for Example 08") << "</dd></div>";
    html << "</dl>";
    html << diagnostics;
    html << "<form method='post' action='/run'>";
    html << "<div class='front-panel' data-panel-id='" << html_escape(core.panel.panel_id)
         << "' data-coordinate-space='panel_pixels' data-runtime-language='cpp'";
    html << " data-compiler-backend='" << (uses_native_kernel ? "llvm" : "none") << "'";
    html << " data-execution-path='" << (uses_native_kernel ? "native_kernel_bridge" : "cpp_enum_contract_executor") << "'";
    html << " style='width:" << css_px(panel_width) << ";height:" << css_px(panel_height) << ";'>";
    html << render_enum_widget(ctrl);
    html << render_enum_widget(ind);
    html << "</div><div class='actions'><button type='submit'>Run Example 08</button><a class='state-link' href='/state.json'>state.json</a></div></form></body></html>";
    return html.str();
}

void EnumBrowserUiRuntime::serve(const std::string& host, std::uint16_t port, bool should_open_browser) {
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
                    const auto form_value = parse_form_value(request.body, "mode_value").value_or("run");
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

PathBrowserUiRuntime::PathBrowserUiRuntime(
    std::filesystem::path contract_path,
    std::filesystem::path wfrog_path,
    std::shared_ptr<const NativeStringKernelBridge> native_kernel_bridge_)
    : core(std::move(contract_path), std::move(wfrog_path)),
      native_kernel_bridge(std::move(native_kernel_bridge_)) {}

frog::json::Value PathBrowserUiRuntime::run_once(const std::string& input_value) {
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

std::string PathBrowserUiRuntime::render_html() const {
    const auto& ctrl = core.widgets.at("path_input");
    const auto& ind = core.widgets.at("path_result");
    const auto panel_width = layout_i64(core.panel.layout, "width", 700);
    const auto panel_height = layout_i64(core.panel.layout, "height", 180);
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
            ".path-widget{font-family:Segoe UI,Arial,sans-serif;}"
            ".path-skin{position:absolute;inset:0;width:100%;height:100%;display:block;}"
            ".path-skin svg{width:100%;height:100%;display:block;}"
            ".path-skin #label_text,.path-skin #caption_text,.path-skin #path_display{display:none;}"
            ".path-caption-overlay{position:absolute;transform:translateY(-50%);font-size:var(--frog-path-caption-font-size);font-weight:var(--frog-path-caption-font-weight);font-family:var(--frog-path-caption-font-family);color:var(--frog-path-caption-color);line-height:1;white-space:nowrap;pointer-events:none;}"
            ".path-value-overlay{position:absolute;box-sizing:border-box;font-family:Segoe UI,Arial,sans-serif;line-height:1;border:0;background:transparent;margin:0;}"
            ".path-control-editor{outline:0;appearance:none;-webkit-appearance:none;}"
            ".path-control-editor:focus{outline:0;}"
            ".path-indicator-value{display:flex;align-items:center;pointer-events:none;overflow:hidden;white-space:nowrap;text-overflow:ellipsis;}"
            ".path-file-picker{position:absolute;width:1px;height:1px;opacity:0;pointer-events:none;}"
            ".path-browse-overlay{position:absolute;box-sizing:border-box;cursor:pointer;background:transparent;border:0;}"
            ".path-control:has(.path-control-editor:hover) .path-skin #path_face{fill:var(--frog-path-face-fill-hover) !important;stroke:var(--frog-path-face-stroke-hover) !important;}"
            ".path-control:has(.path-browse-overlay:hover) .path-skin #browse_button{fill:var(--frog-path-button-fill-hover) !important;stroke:var(--frog-path-button-stroke-hover) !important;}"
            ".actions{margin-top:16px;display:flex;gap:12px;align-items:center;}"
            "button{padding:8px 14px;border:0;border-radius:6px;cursor:pointer;background:#0f62fe;color:#ffffff;font-weight:600;}"
            ".diagnostic{margin:12px 0;padding:10px 12px;border-radius:6px;}"
            ".diagnostic.error{background:#fff1f2;color:#9f1239;border:1px solid #fecdd3;}"
            "</style><script>"
            "function frogPathPicked(input,targetId){const target=document.getElementById(targetId);if(!target){return;}if(input.files&&input.files.length>0){target.value=input.files[0].name;target.dispatchEvent(new Event('input',{bubbles:true}));target.dispatchEvent(new Event('change',{bubbles:true}));}}"
            "</script></head><body>";
    html << "<h1>" << html_escape(core.panel.title) << "</h1>";
    html << "<p class='meta'>Example 09 - .frog front panel + Default Path .wfrog realization assets + C++ runtime</p>";
    html << "<dl class='runtime-facts' aria-label='Runtime facts'>";
    html << "<div><dt>Runtime</dt><dd>C++ reference runtime</dd></div>";
    html << "<div><dt>Execution</dt><dd>" << (uses_native_kernel ? "native kernel bridge" : "path contract executor") << "</dd></div>";
    html << "<div><dt>Compiler backend</dt><dd>" << (uses_native_kernel ? "LLVM native path kernel artifact" : "none for Example 09") << "</dd></div>";
    html << "</dl>";
    html << diagnostics;
    html << "<form method='post' action='/run'>";
    html << "<div class='front-panel' data-panel-id='" << html_escape(core.panel.panel_id)
         << "' data-coordinate-space='panel_pixels' data-runtime-language='cpp'";
    html << " data-compiler-backend='" << (uses_native_kernel ? "llvm" : "none") << "'";
    html << " data-execution-path='" << (uses_native_kernel ? "native_kernel_bridge" : "cpp_path_contract_executor") << "'";
    html << " style='width:" << css_px(panel_width) << ";height:" << css_px(panel_height) << ";'>";
    for (const auto& widget_ref : core.panel.widgets) {
        const auto widget_it = core.widgets.find(widget_ref.instance_id);
        if (widget_it != core.widgets.end()) {
            html << render_path_widget(widget_it->second);
        }
    }
    html << "</div><div class='actions'><button type='submit'>Run Example 09</button><a class='state-link' href='/state.json'>state.json</a></div></form></body></html>";
    return html.str();
}

void PathBrowserUiRuntime::serve(const std::string& host, std::uint16_t port, bool should_open_browser) {
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
                    std::map<std::string, std::string> form_values;
                    for (const auto& widget_ref : core.panel.widgets) {
                        const auto widget_it = core.widgets.find(widget_ref.instance_id);
                        if (widget_it == core.widgets.end() || widget_it->second.role != "control") {
                            continue;
                        }
                        const auto public_input_id = property_string(
                            widget_it->second.properties,
                            "binding.public_input_id",
                            property_string(widget_it->second.properties, "binding.preview_input_id"));
                        if (public_input_id.empty()) {
                            continue;
                        }
                        const auto form_value = parse_form_value(request.body, public_input_id);
                        if (form_value.has_value()) {
                            form_values[public_input_id] = *form_value;
                        }
                    }
                    if (native_kernel_bridge == nullptr) {
                        core.execute_all(form_values);
                    } else {
                        core.execute_all_with_native_kernel_bridge(*native_kernel_bridge, form_values);
                    }
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

ButtonBrowserUiRuntime::ButtonBrowserUiRuntime(
    std::filesystem::path contract_path,
    std::filesystem::path wfrog_path,
    std::shared_ptr<const NativeBoolKernelBridge> native_kernel_bridge_)
    : core(std::move(contract_path), std::move(wfrog_path)),
      native_kernel_bridge(std::move(native_kernel_bridge_)) {}

namespace {

struct ButtonMechanicalDiagramSpec {
    std::string title;
    std::vector<std::pair<int, std::string>> events;
    std::vector<std::pair<int, int>> wave_points;
    std::string note;
};

ButtonMechanicalDiagramSpec button_mechanical_diagram_spec(const std::string& action) {
    constexpr int output_off_y = 252;
    constexpr int output_on_y = 198;
    if (action == "switch_when_pressed") {
        return {
            "Switch when pressed",
            {{130, "Press"}, {250, "Release"}, {370, "Press again"}},
            {{70, output_off_y}, {130, output_off_y}, {130, output_on_y}, {370, output_on_y}, {370, output_off_y}, {450, output_off_y}},
            "Toggles when pressed. Release has no effect. Program read does not reset the value.",
        };
    }
    if (action == "switch_when_released") {
        return {
            "Switch when released",
            {{130, "Press"}, {250, "Release"}, {370, "Release again"}},
            {{70, output_off_y}, {250, output_off_y}, {250, output_on_y}, {370, output_on_y}, {370, output_off_y}, {450, output_off_y}},
            "Toggles when released. Press alone has no effect. Program read does not reset the value.",
        };
    }
    if (action == "switch_until_released") {
        return {
            "Switch until released",
            {{150, "Press"}, {330, "Release"}},
            {{70, output_off_y}, {150, output_off_y}, {150, output_on_y}, {330, output_on_y}, {330, output_off_y}, {450, output_off_y}},
            "ON while pressed. OFF when released. Program read does not reset the value.",
        };
    }
    if (action == "latch_when_released") {
        return {
            "Latch when released",
            {{130, "Press"}, {250, "Release"}, {330, "Read"}},
            {{70, output_off_y}, {250, output_off_y}, {250, output_on_y}, {330, output_on_y}, {330, output_off_y}, {450, output_off_y}},
            "Release latches TRUE. The program read emits one TRUE pulse, then the stored value resets to FALSE.",
        };
    }
    if (action == "latch_until_released") {
        return {
            "Latch until released",
            {{130, "Press"}, {230, "Read"}, {330, "Release"}},
            {{70, output_off_y}, {130, output_off_y}, {130, output_on_y}, {330, output_on_y}, {330, output_off_y}, {450, output_off_y}},
            "TRUE while pressed when read continuously. It resets after release once the program has read it.",
        };
    }
    return {
        "Latch when pressed",
        {{130, "Press"}, {210, "Read"}, {330, "Release"}},
        {{70, output_off_y}, {130, output_off_y}, {130, output_on_y}, {210, output_on_y}, {210, output_off_y}, {450, output_off_y}},
        "Press latches TRUE. The program read emits one TRUE pulse, then the stored value resets to FALSE.",
    };
}

std::vector<std::string> wrap_diagram_note(const std::string& text) {
    std::vector<std::string> lines;
    std::istringstream words(text);
    std::string word;
    std::string line;
    while (words >> word) {
        if (!line.empty() && line.size() + 1 + word.size() > 66) {
            lines.push_back(line);
            line.clear();
        }
        if (!line.empty()) {
            line += " ";
        }
        line += word;
    }
    if (!line.empty()) {
        lines.push_back(line);
    }
    return lines;
}

std::string render_button_mechanical_action_diagram(const std::string& action) {
    constexpr int output_label_y = 174;
    constexpr int output_on_label_y = 207;
    constexpr int output_off_label_y = 260;
    constexpr int output_marker_top_y = 192;
    constexpr int output_marker_bottom_y = 262;
    constexpr int description_label_y = 294;
    constexpr int description_first_line_y = 318;
    const auto spec = button_mechanical_diagram_spec(action);
    const auto note_lines = wrap_diagram_note(spec.note);
    std::ostringstream points;
    for (const auto& point : spec.wave_points) {
        if (points.tellp() > 0) {
            points << " ";
        }
        points << point.first << "," << point.second;
    }

    std::ostringstream svg;
    svg << "<section class='mechanical-diagram-card' aria-label='" << html_escape(spec.title) << " behavior diagram'>";
    svg << "<svg class='mechanical-diagram' viewBox='0 0 520 360' role='img' aria-labelledby='mechanical-title mechanical-desc'>";
    svg << "<title id='mechanical-title'>" << html_escape(spec.title) << "</title>";
    svg << "<desc id='mechanical-desc'>" << html_escape(spec.note) << "</desc>";
    svg << "<rect x='1' y='1' width='518' height='358' rx='8' fill='#ffffff' stroke='#d1d5db'/>";
    svg << "<text x='24' y='36' font-size='24' font-weight='700' fill='#111827'>" << html_escape(spec.title) << "</text>";
    svg << "<text x='24' y='76' font-size='14' fill='#374151'>Events</text>";
    svg << "<line x1='70' y1='112' x2='455' y2='112' stroke='#111827' stroke-width='2'/>";
    svg << "<path d='M455 112 l-14 -7 v14 z' fill='#111827'/>";
    svg << "<text x='430' y='136' font-size='12' fill='#374151'>Time</text>";
    for (const auto& event : spec.events) {
        const bool read = event.second == "Read";
        const bool active = event.second.find("Press") != std::string::npos || read;
        const char* color = read ? "#15803d" : (active ? "#1d4ed8" : "#111827");
        svg << "<line x1='" << event.first << "' y1='84' x2='" << event.first << "' y2='112' stroke='" << color << "' stroke-width='2'/>";
        svg << "<circle cx='" << event.first << "' cy='112' r='5' fill='#ffffff' stroke='" << color << "' stroke-width='2'/>";
        svg << "<text x='" << event.first << "' y='68' text-anchor='middle' font-size='13' fill='" << color << "'>" << html_escape(event.second) << "</text>";
    }
    svg << "<line x1='24' y1='150' x2='496' y2='150' stroke='#e5e7eb'/>";
    svg << "<text x='24' y='" << output_label_y << "' font-size='14' fill='#374151'>Output</text>";
    svg << "<text x='24' y='" << output_on_label_y << "' font-size='13' fill='#1d4ed8'>ON</text>";
    svg << "<text x='24' y='" << output_off_label_y << "' font-size='13' fill='#111827'>OFF</text>";
    svg << "<polyline points='" << points.str() << "' fill='none' stroke='#111827' stroke-width='3' stroke-linejoin='miter'/>";
    svg << "<polyline points='" << points.str() << "' fill='none' stroke='#1d4ed8' stroke-width='2' stroke-linejoin='miter' stroke-dasharray='0 1000'/>";
    for (const auto& event : spec.events) {
        const bool read = event.second == "Read";
        const char* color = read ? "#15803d" : "#111827";
        svg << "<line x1='" << event.first << "' y1='" << output_marker_top_y << "' x2='" << event.first << "' y2='" << output_marker_bottom_y << "' stroke='" << color << "' stroke-width='1.5' stroke-dasharray='5 5'/>";
    }
    svg << "<text x='24' y='" << description_label_y << "' font-size='12' fill='#6b7280'>Description</text>";
    int note_y = description_first_line_y;
    for (const auto& line : note_lines) {
        svg << "<text x='260' y='" << note_y << "' text-anchor='middle' font-size='13' fill='#374151'>" << html_escape(line) << "</text>";
        note_y += 18;
    }
    svg << "</svg></section>";
    return svg.str();
}

} // namespace

frog::json::Value ButtonBrowserUiRuntime::run_once(bool trigger_pressed) {
    try {
        frog::json::Value artifact = native_kernel_bridge == nullptr
            ? core.execute(trigger_pressed)
            : core.execute_with_native_kernel_bridge(*native_kernel_bridge, trigger_pressed);
        last_error.reset();
        return artifact;
    } catch (const std::exception& error) {
        last_error = error.what();
        throw;
    }
}

frog::json::Value ButtonBrowserUiRuntime::apply_event(const std::string& event_name) {
    try {
        frog::json::Value artifact;
        const auto action = property_string(core.widgets.at(core.control_widget_id).properties, "behavior.mechanical_action", "");
        const bool latch_action = action == "latch_when_pressed" ||
            action == "latch_when_released" ||
            action == "latch_until_released";
        if (event_name == "press") {
            artifact = core.press_control();
        } else if (event_name == "release") {
            artifact = core.release_control();
        } else if (event_name == "read") {
            artifact = read_once();
        } else {
            throw std::runtime_error("Unknown Button runtime event: " + event_name);
        }
        if (latch_action && (event_name == "press" || event_name == "release")) {
            artifact = read_once();
        }
        last_error.reset();
        return artifact;
    } catch (const std::exception& error) {
        last_error = error.what();
        throw;
    }
}

frog::json::Value ButtonBrowserUiRuntime::read_once() {
    try {
        frog::json::Value artifact = native_kernel_bridge == nullptr
            ? core.read_program_value()
            : core.read_program_value_with_native_kernel_bridge(*native_kernel_bridge);
        last_error.reset();
        return artifact;
    } catch (const std::exception& error) {
        last_error = error.what();
        throw;
    }
}

std::string ButtonBrowserUiRuntime::render_html() const {
    const auto& button = core.widgets.at(core.control_widget_id);
    const auto mechanical_action = property_string(button.properties, "behavior.mechanical_action", "");
    const bool switch_when_pressed = mechanical_action == "switch_when_pressed";
    const bool switch_when_released = mechanical_action == "switch_when_released";
    const bool latch_when_pressed = mechanical_action == "latch_when_pressed";
    const bool latch_when_released = mechanical_action == "latch_when_released";
    const bool latch_until_released = mechanical_action == "latch_until_released";
    const bool latch_action = latch_when_pressed || latch_when_released || latch_until_released;
    const auto& indicator = core.widgets.at(core.indicator_widget_id);
    const auto panel_width = layout_i64(core.panel.layout, "width", 520);
    const auto panel_height = layout_i64(core.panel.layout, "height", 180);
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
            ".mechanical-diagram-card{width:520px;max-width:100%;margin:0 0 18px 0;}"
            ".mechanical-diagram{display:block;width:100%;height:auto;}"
            ".front-panel{position:relative;background:#ffffff;border-radius:10px;box-shadow:0 4px 14px rgba(15,23,42,0.08);overflow:hidden;}"
            ".frog-widget{position:absolute;box-sizing:border-box;}"
            ".button-widget{overflow:visible;}"
            ".button-skin{position:absolute;inset:0;width:100%;height:100%;display:block;z-index:1;}"
            ".button-skin svg{width:100%;height:100%;display:block;}"
            ".button-skin [data-frog-part='label'],.button-skin [data-frog-part='caption'],.button-skin [data-frog-part='state_text']{display:none!important;}"
            ".button-skin [data-frog-part='frame']{fill:var(--frog-button-frame-fill)!important;stroke:var(--frog-button-frame-stroke)!important;stroke-width:var(--frog-button-frame-stroke-width)!important;}"
            ".button-skin [data-frog-part='face']{fill:var(--frog-button-face-fill)!important;stroke:var(--frog-button-face-stroke)!important;stroke-width:var(--frog-button-face-stroke-width)!important;transition:fill var(--frog-button-transition),stroke var(--frog-button-transition),transform var(--frog-button-transition);}"
            ".button-skin [data-frog-part='state_face']{fill:var(--frog-button-state-face-fill)!important;stroke:var(--frog-button-state-face-stroke)!important;stroke-width:var(--frog-button-state-face-stroke-width)!important;transition:fill var(--frog-button-transition),stroke var(--frog-button-transition),transform var(--frog-button-transition);}"
            ".button-skin [data-frog-part='focus_ring']{display:none!important;stroke:var(--frog-button-focus-color)!important;stroke-width:var(--frog-button-focus-width)!important;}"
            ".button-widget[data-frog-hover-applies-when-value-false-only='false']:has(.button-press-overlay:hover) .button-skin [data-frog-part='face'],.button-widget[data-frog-hover-applies-when-value-false-only='true'][data-current-value='false']:has(.button-press-overlay:hover) .button-skin [data-frog-part='face']{fill:var(--frog-button-face-hover-fill)!important;}"
            ".button-widget[data-frog-hover-applies-when-value-false-only='false']:has(.button-press-overlay:hover) .button-skin [data-frog-part='state_face'],.button-widget[data-frog-hover-applies-when-value-false-only='true'][data-current-value='false']:has(.button-press-overlay:hover) .button-skin [data-frog-part='state_face']{fill:var(--frog-button-state-face-hover-fill)!important;stroke:var(--frog-button-state-face-hover-stroke)!important;}"
            ".button-widget[data-frog-pressed-applies-while-active='true']:has(.button-press-overlay:active) .button-skin [data-frog-part='face'],.button-widget[data-frog-pressed-applies-when-value-true='true'][data-current-value='true'] .button-skin [data-frog-part='face']{fill:var(--frog-button-face-pressed-fill)!important;transform:translateY(var(--frog-button-pressed-inset));}"
            ".button-widget[data-frog-pressed-applies-while-active='true']:has(.button-press-overlay:active) .button-skin [data-frog-part='state_face'],.button-widget[data-frog-pressed-applies-when-value-true='true'][data-current-value='true'] .button-skin [data-frog-part='state_face']{fill:var(--frog-button-state-face-pressed-fill)!important;stroke:var(--frog-button-state-face-pressed-stroke)!important;transform:translateY(var(--frog-button-pressed-inset));}"
            ".button-widget[data-frog-pressed-applies-when-value-true='true'][data-current-value='true'] .button-state-overlay{transform:translate(-50%,calc(-50% + var(--frog-button-pressed-inset)));}"
            ".button-widget:has(.button-press-overlay:focus-visible) .button-skin [data-frog-part='focus_ring']{display:inline!important;}"
            ".button-caption-overlay{position:absolute;left:0;top:0;transform:translateY(-50%);text-align:left;font-size:var(--frog-button-caption-font-size);font-weight:var(--frog-button-caption-font-weight);font-family:var(--frog-button-caption-font-family);line-height:1;white-space:nowrap;pointer-events:none;z-index:3;}"
            ".button-state-overlay{position:absolute;transform:translate(-50%,-50%);font-size:var(--frog-button-state-text-font-size);font-weight:var(--frog-button-state-text-font-weight);line-height:1;color:var(--frog-button-state-text-fill);pointer-events:none;z-index:6;max-width:70%;overflow:hidden;text-overflow:ellipsis;white-space:nowrap;}"
            ".button-press-overlay{position:absolute;box-sizing:border-box;margin:0;padding:0;border:0;background:transparent;cursor:pointer;appearance:none;z-index:5;}"
            ".button-press-overlay:focus,.button-press-overlay:focus-visible,.button-press-overlay:active{outline:0;box-shadow:none;}"
            ".boolean-widget{border:0;padding:0;background:transparent;font:inherit;color:inherit;overflow:visible;}"
            ".boolean-indicator{pointer-events:none;}"
            ".boolean-skin{position:absolute;inset:0;width:100%;height:100%;object-fit:fill;display:block;pointer-events:none;z-index:2;}"
            ".boolean-caption-overlay{position:absolute;left:0;top:0;transform:translateY(-50%);text-align:left;font-size:var(--boolean-caption-font-size);font-weight:var(--boolean-caption-font-weight);font-family:var(--boolean-caption-font-family);line-height:1;color:var(--boolean-caption-color);white-space:nowrap;pointer-events:none;z-index:3;}"
            ".boolean-state-face{position:absolute;left:var(--boolean-inner-left);top:var(--boolean-inner-top);width:var(--boolean-inner-width);height:var(--boolean-inner-height);border:var(--boolean-inner-border-width) solid var(--boolean-inner-border);border-radius:7px;background:var(--boolean-fill);box-shadow:inset 0 1px 0 rgba(255,255,255,.6),0 1px 2px rgba(15,23,42,.16);transition:background var(--boolean-transition),border-color var(--boolean-transition),box-shadow var(--boolean-transition),transform var(--boolean-transition);z-index:1;}"
            ".boolean-widget[data-realization-variant='circular'] .boolean-state-face{border-radius:50%;}"
            ".boolean-widget[data-frog-frame-visible='false'] .boolean-state-face{box-shadow:none;}"
            ".boolean-state-overlay{position:absolute;transform:translate(-50%,-50%);text-align:center;font-size:var(--boolean-text-font-size);font-weight:var(--boolean-text-font-weight);line-height:1;color:var(--boolean-text);pointer-events:none;z-index:4;white-space:nowrap;}"
            ".actions{margin-top:16px;display:flex;gap:12px;align-items:center;}"
            ".program-read-action{padding:6px 10px;border:1px solid #94a3b8;border-radius:4px;background:#ffffff;color:#111827;font:inherit;cursor:pointer;}"
            ".program-read-action:hover{background:#f8fafc;}"
            ".program-read-status{font-size:13px;color:#374151;min-width:112px;}"
            ".state-link{font-size:16px;}"
            ".diagnostic{margin:12px 0;padding:10px 12px;border-radius:6px;}"
            ".diagnostic.error{background:#fff1f2;color:#9f1239;border:1px solid #fecdd3;}"
            "</style></head><body>";
    html << "<h1>" << html_escape(core.panel.title) << "</h1>";
    html << "<p class='meta'>";
    if (latch_until_released) {
        html << "Example 15 - .frog latch_until_released Button value + Default Button/Boolean .wfrog realization assets + C++ runtime";
    } else if (latch_when_released) {
        html << "Example 14 - .frog latch_when_released Button value + Default Button/Boolean .wfrog realization assets + C++ runtime";
    } else if (latch_when_pressed) {
        html << "Example 13 - .frog latch_when_pressed Button value + Default Button/Boolean .wfrog realization assets + C++ runtime";
    } else if (switch_when_released) {
        html << "Example 12 - .frog switch_when_released Button value + Default Button/Boolean .wfrog realization assets + C++ runtime";
    } else if (switch_when_pressed) {
        html << "Example 11 - .frog switch_when_pressed Button value + Default Button/Boolean .wfrog realization assets + C++ runtime";
    } else {
        html << "Example 10 - .frog front panel + Default Button/Boolean .wfrog realization assets + C++ runtime";
    }
    html << "</p>";
    html << "<dl class='runtime-facts' aria-label='Runtime facts'>";
    html << "<div><dt>Runtime</dt><dd>C++ reference runtime</dd></div>";
    html << "<div><dt>Execution</dt><dd>" << (uses_native_kernel
        ? "native kernel bridge"
        : ((switch_when_pressed || switch_when_released)
            ? "button switch contract executor"
            : (latch_action ? "button latch contract executor" : "button contract executor"))) << "</dd></div>";
    html << "<div><dt>Compiler backend</dt><dd>";
    if (uses_native_kernel) {
        html << "LLVM native Button bool kernel artifact";
    } else if (latch_until_released) {
        html << "none for Example 15";
    } else if (latch_when_released) {
        html << "none for Example 14";
    } else if (latch_when_pressed) {
        html << "none for Example 13";
    } else if (switch_when_released) {
        html << "none for Example 12";
    } else if (switch_when_pressed) {
        html << "none for Example 11";
    } else {
        html << "none for Example 10";
    }
    html << "</dd></div>";
    html << "</dl>";
    html << diagnostics;
    html << render_button_mechanical_action_diagram(mechanical_action);
    html << "<form method='post' action='/run'>";
    html << "<div class='front-panel' data-panel-id='" << html_escape(core.panel.panel_id)
         << "' data-coordinate-space='panel_pixels' data-runtime-language='cpp'";
    html << " data-compiler-backend='" << (uses_native_kernel ? "llvm" : "none") << "'";
    html << " data-execution-path='";
    if (uses_native_kernel) {
        html << "native_kernel_bridge";
    } else if (latch_until_released) {
        html << "cpp_button_latch_until_released_contract_executor";
    } else if (latch_when_released) {
        html << "cpp_button_latch_when_released_contract_executor";
    } else if (latch_when_pressed) {
        html << "cpp_button_latch_when_pressed_contract_executor";
    } else if (switch_when_released) {
        html << "cpp_button_switch_when_released_contract_executor";
    } else if (switch_when_pressed) {
        html << "cpp_button_switch_when_pressed_contract_executor";
    } else {
        html << "cpp_button_contract_executor";
    }
    html << "'";
    html << " style='width:" << css_px(panel_width) << ";height:" << css_px(panel_height) << ";'>";
    html << render_button_widget(button);
    html << render_boolean_widget(indicator);
    html << "</div><div class='actions'><button class='program-read-action' type='button' data-frog-event='read'>Read</button>"
         << "<span class='program-read-status' data-frog-last-read='none'>Last read: none</span>"
         << "<a class='state-link' href='/state.json'>state.json</a></div></form>";
    html << button_widget_script();
    html << "</body></html>";
    return html.str();
}

void ButtonBrowserUiRuntime::serve(const std::string& host, std::uint16_t port, bool should_open_browser) {
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
            } else if (request.method == "POST" && (request.path == "/run" || request.path == "/event")) {
                try {
                    frog::json::Value artifact(nullptr);
                    const auto event_name = parse_form_value(request.body, "frog_event");
                    if (request.path == "/event" && event_name.has_value()) {
                        artifact = apply_event(*event_name);
                    } else {
                        const auto form_value = parse_form_value(request.body, core.public_input_id).value_or("false");
                        artifact = run_once(parse_bool_form_value(form_value));
                    }
                    if (request.path == "/event") {
                        send_response(client, "200 OK", "application/json; charset=utf-8", frog::json::stringify(artifact, true, 2));
                    } else {
                        send_response(client, "303 See Other", "text/plain; charset=utf-8", "", std::make_pair(std::string("Location"), std::string("/")));
                    }
                } catch (const std::exception& error) {
                    last_error = error.what();
                    if (request.path == "/event") {
                        send_response(client, "500 Internal Server Error", "application/json; charset=utf-8", frog::json::stringify(core.execution_artifact(), true, 2));
                    } else {
                        send_response(client, "303 See Other", "text/plain; charset=utf-8", "", std::make_pair(std::string("Location"), std::string("/")));
                    }
                }
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
