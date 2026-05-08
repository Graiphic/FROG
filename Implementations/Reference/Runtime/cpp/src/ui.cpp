#include "ui.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "json.hpp"

#ifdef _WIN32
#include <shellapi.h>
#include <winsock2.h>
#include <ws2tcpip.h>
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
    return is_safe_hex_color(value) ? value : fallback;
}

struct SvgGeometry {
    double view_width = 220.0;
    double view_height = 88.0;
    double label_x = 16.0;
    double label_y = 24.0;
    double value_x = 22.0;
    double value_y = 62.0;
    double value_box_x = 14.0;
    double value_box_y = 40.0;
    double value_box_width = 192.0;
    double value_box_height = 32.0;
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

void parse_translate_anchor(const std::string& svg, const std::string& id, double& x, double& y) {
    const auto transform = svg_attribute(svg, id, "transform");
    if (!transform.has_value()) {
        return;
    }
    const std::string prefix = "translate(";
    const auto start = transform->find(prefix);
    const auto end = transform->find(')', start == std::string::npos ? 0 : start);
    if (start == std::string::npos || end == std::string::npos) {
        return;
    }
    std::string payload = transform->substr(start + prefix.size(), end - start - prefix.size());
    std::replace(payload.begin(), payload.end(), ',', ' ');
    std::istringstream input(payload);
    double parsed_x = x;
    double parsed_y = y;
    if (input >> parsed_x >> parsed_y) {
        x = parsed_x;
        y = parsed_y;
    }
}

SvgGeometry load_svg_geometry(const WidgetState& widget) {
    SvgGeometry geometry;
    if (widget.asset_path.empty() || !std::filesystem::exists(widget.asset_path)) {
        return geometry;
    }
    const auto svg = read_text_file(widget.asset_path);
    parse_viewbox(svg, geometry);
    parse_translate_anchor(svg, "label_anchor", geometry.label_x, geometry.label_y);
    parse_translate_anchor(svg, "value_anchor", geometry.value_x, geometry.value_y);
    geometry.value_box_x = svg_attribute_double(svg, "value_box", "x", geometry.value_box_x);
    geometry.value_box_y = svg_attribute_double(svg, "value_box", "y", geometry.value_box_y);
    geometry.value_box_width = svg_attribute_double(svg, "value_box", "width", geometry.value_box_width);
    geometry.value_box_height = svg_attribute_double(svg, "value_box", "height", geometry.value_box_height);
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

std::string render_numeric_widget(const WidgetState& widget) {
    const bool is_control = widget.role == "control";
    const auto geometry = load_svg_geometry(widget);
    const auto x = layout_i64(widget.layout, "x", 0);
    const auto y = layout_i64(widget.layout, "y", 0);
    const auto width = layout_i64(widget.layout, "width", 160);
    const auto height = layout_i64(widget.layout, "height", 48);
    const auto value = property_u16(widget.properties, "value", 0);
    const auto label = property_string(widget.properties, "label", widget.widget_id);
    const auto color = safe_css_color(property_string(widget.properties, "foreground_color", "#1f2933"), "#1f2933");
    const auto route = asset_route(widget);

    std::ostringstream html;
    html << "<section class='frog-widget numeric-widget " << (is_control ? "numeric-control" : "numeric-indicator") << "'";
    html << " data-widget-id='" << html_escape(widget.widget_id) << "'";
    html << " data-class-ref='" << html_escape(widget.class_ref) << "'";
    html << " data-role='" << html_escape(widget.role) << "'";
    if (!route.empty()) {
        html << " data-asset-route='" << html_escape(route) << "'";
    }
    html << " style='position:absolute;left:" << css_px(x) << ";top:" << css_px(y) << ";width:" << css_px(width)
         << ";height:" << css_px(height) << ";";
    if (!property_bool(widget.properties, "visible", true)) {
        html << "display:none;";
    }
    html << "'>";

    if (!route.empty()) {
        html << "<img class='numeric-skin' src='" << html_escape(route) << "' alt='' aria-hidden='true' />";
    } else {
        html << "<div class='numeric-skin missing-skin'></div>";
    }

    html << "<span class='numeric-label-overlay' data-svg-anchor='label_anchor' style='"
         << svg_anchor_style(geometry.label_x, geometry.label_y, geometry)
         << "color:" << html_escape(color) << ";'>" << html_escape(label) << "</span>";

    const auto value_style = svg_box_style(
        geometry.value_box_x,
        geometry.value_box_y,
        geometry.value_box_width,
        geometry.value_box_height,
        geometry);

    if (is_control) {
        html << "<input id='" << html_escape(widget.widget_id) << "_value' name='input_value' type='number' min='0' max='65535'";
        html << " class='numeric-value-overlay numeric-control-editor' data-svg-part='value_box' data-svg-anchor='value_anchor'";
        html << " style='" << value_style << "color:" << html_escape(color) << ";'";
        html << " value='" << value << "'";
        if (!property_bool(widget.properties, "enabled", true)) {
            html << " disabled";
        }
        html << " />";
    } else {
        html << "<output class='numeric-value-overlay numeric-indicator-value' data-svg-part='value_box' data-svg-anchor='value_anchor'";
        html << " style='" << value_style << "color:" << html_escape(color) << ";'>" << value << "</output>";
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
    std::optional<std::filesystem::path> wfrog_path)
    : core(contract_path.value_or(default_contract_path()), wfrog_path.value_or(default_wfrog_path())) {}

std::string BrowserUiRuntime::render_html() const {
    const auto& ctrl = core.widgets.at("ctrl_input");
    const auto& ind = core.widgets.at("ind_result");
    const auto snapshot = frog::json::stringify(core.execution_artifact(), true, 2);
    const auto panel_width = layout_i64(core.panel.layout, "width", 460);
    const auto panel_height = layout_i64(core.panel.layout, "height", 170);

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
            ".front-panel{position:relative;background:#ffffff;border-radius:10px;box-shadow:0 4px 14px rgba(15,23,42,0.08);overflow:hidden;}"
            ".frog-widget{position:absolute;box-sizing:border-box;}"
            ".numeric-widget{font-family:Segoe UI,Arial,sans-serif;}"
            ".numeric-skin{position:absolute;inset:0;width:100%;height:100%;object-fit:fill;display:block;}"
            ".missing-skin{background:#e5e7eb;border:1px solid #9ca3af;border-radius:6px;}"
            ".numeric-label-overlay{position:absolute;transform:translateY(-50%);font-size:10px;font-weight:700;line-height:1;white-space:nowrap;pointer-events:none;text-shadow:0 1px 1px rgba(0,0,0,0.18);}"
            ".numeric-value-overlay{position:absolute;box-sizing:border-box;font-family:Consolas,Segoe UI Mono,monospace;font-size:11px;font-weight:700;line-height:1;border:0;background:transparent;}"
            ".numeric-control-editor{padding:0 4px;border-radius:4px;outline:1px solid rgba(15,23,42,0.18);background:rgba(255,255,255,0.72);appearance:textfield;}"
            ".numeric-control-editor:focus{outline:2px solid #0f62fe;background:rgba(255,255,255,0.9);}"
            ".numeric-indicator-value{display:flex;align-items:center;padding:0 4px;pointer-events:none;}"
            ".actions{margin-top:16px;display:flex;gap:12px;align-items:center;}"
            "button{padding:8px 14px;border:0;border-radius:6px;cursor:pointer;background:#0f62fe;color:#ffffff;font-weight:600;}"
            ".diagnostic{margin:12px 0;padding:10px 12px;border-radius:6px;}"
            ".diagnostic.error{background:#fff1f2;color:#9f1239;border:1px solid #fecdd3;}"
            "summary{cursor:pointer;margin-top:16px;font-weight:600;}"
            "pre{white-space:pre-wrap;word-break:break-word;background:#0b1020;color:#dbeafe;padding:12px;border-radius:8px;font-size:12px;}"
            "</style></head><body>";
    html << "<h1>" << html_escape(core.panel.title) << "</h1>";
    html << "<p class='meta'>Example 05 — contract + .wfrog + browser host runtime</p>";
    html << diagnostics;
    html << "<form method='post' action='/run'>";
    html << "<div class='front-panel' data-panel-id='" << html_escape(core.panel.panel_id) << "' data-coordinate-space='panel_pixels'";
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
                    core.execute(parse_u16_form_value(form_value));
                    last_error.reset();
                } catch (const std::exception& error) {
                    last_error = error.what();
                }
                send_response(client, "303 See Other", "text/plain; charset=utf-8", "", std::make_pair(std::string("Location"), std::string("/")));
            } else {
                send_response(client, "404 Not Found", "text/plain; charset=utf-8", "not found");
            }
        } catch (const std::exception& error) {
            send_response(client, "500 Internal Server Error", "text/plain; charset=utf-8", error.what());
        }
        close_socket(client);
    }
}

} // namespace frog::runtime
