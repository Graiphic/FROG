use std::collections::BTreeMap;
use std::fmt::Write as _;
use std::fs;
use std::io::{BufRead, BufReader, Read, Write};
use std::net::{TcpListener, TcpStream};
use std::path::PathBuf;
use std::process::Command;

use serde_json::{json, to_string_pretty, Map, Value};

use crate::contract::{default_contract_path, default_wfrog_path};
use crate::diagnostics::{Result, RuntimeError};
use crate::execute::execute_reference_contract_case;
use crate::native_kernel::{NativeBoolKernelBridge, NativeKernelBridge};
use crate::runtime::{RuntimeCore, WidgetState};

pub struct BrowserUiRuntime {
    pub core: RuntimeCore,
    pub last_error: Option<String>,
    pub native_kernel_bridge: Option<NativeKernelBridge>,
}

impl BrowserUiRuntime {
    pub fn new(contract_path: Option<PathBuf>, wfrog_path: Option<PathBuf>) -> Result<Self> {
        Self::with_native_kernel_bridge(contract_path, wfrog_path, None)
    }

    pub fn with_native_kernel_bridge(
        contract_path: Option<PathBuf>,
        wfrog_path: Option<PathBuf>,
        native_kernel_bridge: Option<NativeKernelBridge>,
    ) -> Result<Self> {
        let contract = contract_path.unwrap_or(default_contract_path()?);
        let wfrog = wfrog_path.unwrap_or(default_wfrog_path()?);
        let core = RuntimeCore::from_paths(contract, wfrog)?;
        Ok(Self {
            core,
            last_error: None,
            native_kernel_bridge,
        })
    }

    pub fn run_once(&mut self, input_value: u16) -> Result<Value> {
        let artifact = if let Some(bridge) = &self.native_kernel_bridge {
            self.core.execute_with_native_kernel_bridge(bridge, Some(input_value))?
        } else {
            self.core.execute(Some(input_value))?
        };
        self.last_error = None;
        Ok(artifact)
    }

    pub fn execution_artifact(&self) -> Value {
        self.core.execution_artifact()
    }

    pub fn render_html(&self) -> String {
        let snapshot = self.core.execution_artifact();
        let panel = &snapshot["ui_runtime"]["panel"];
        let panel_layout = &panel["layout"];
        let panel_width = layout_i64(panel_layout, "width", 500);
        let panel_height = layout_i64(panel_layout, "height", 170);
        let ctrl_html = render_numeric_widget(self.core.widgets.get("ctrl_input").unwrap());
        let ind_html = render_numeric_widget(self.core.widgets.get("ind_result").unwrap());

        let mut diagnostics = String::new();
        if let Some(message) = &self.last_error {
            let _ = write!(
                diagnostics,
                "<div class='diagnostic error'>{}</div>",
                escape_html(message)
            );
        }
        let uses_native_kernel = self.native_kernel_bridge.is_some();

        format!(
            "<!doctype html><html lang='en'><head><meta charset='utf-8'><title>{title}</title>\
             <style>\
             body{{font-family:Segoe UI,Arial,sans-serif;margin:24px;background:#f3f6f8;color:#1f2933;}}\
             h1{{margin:0 0 12px 0;font-size:24px;}}\
             p.meta{{margin:0 0 20px 0;color:#52606d;}}\
             .runtime-facts{{display:flex;flex-wrap:wrap;gap:8px;margin:-8px 0 18px 0;}}\
             .runtime-facts div{{display:flex;gap:6px;align-items:baseline;padding:6px 8px;border:1px solid #d9e2ec;border-radius:6px;background:#ffffff;}}\
             .runtime-facts dt{{margin:0;color:#52606d;font-size:11px;font-weight:700;text-transform:uppercase;}}\
             .runtime-facts dd{{margin:0;color:#1f2933;font-size:12px;font-weight:600;}}\
             .front-panel{{position:relative;background:#ffffff;border-radius:10px;box-shadow:0 4px 14px rgba(15,23,42,0.08);overflow:hidden;}}\
             .frog-widget{{position:absolute;box-sizing:border-box;}}\
             .numeric-widget{{font-family:Segoe UI,Arial,sans-serif;}}\
             .numeric-skin{{position:absolute;inset:0;width:100%;height:100%;object-fit:fill;display:block;}}\
             .missing-skin{{background:#e5e7eb;border:1px solid #9ca3af;border-radius:6px;}}\
             .numeric-label-overlay{{position:absolute;transform:translateY(-50%);font-size:10px;font-weight:700;line-height:1;white-space:nowrap;pointer-events:none;}}\
             .numeric-value-overlay{{position:absolute;box-sizing:border-box;font-family:Consolas,Segoe UI Mono,monospace;font-size:11px;font-weight:700;line-height:1;border:0;background:transparent;}}\
             .numeric-control-editor{{padding:0 4px;border-radius:4px;outline:1px solid rgba(15,23,42,0.18);background:rgba(255,255,255,0.72);appearance:textfield;}}\
             .numeric-control-editor:focus{{outline:2px solid #0f62fe;background:rgba(255,255,255,0.9);}}\
             .numeric-indicator-value{{display:flex;align-items:center;padding:0 4px;pointer-events:none;}}\
             .actions{{margin-top:16px;display:flex;gap:12px;align-items:center;}}\
             button{{padding:8px 14px;border:0;border-radius:6px;cursor:pointer;background:#0f62fe;color:#ffffff;font-weight:600;}}\
             .diagnostic{{margin:12px 0;padding:10px 12px;border-radius:6px;}}\
             .diagnostic.error{{background:#fff1f2;color:#9f1239;border:1px solid #fecdd3;}}\
             summary{{cursor:pointer;margin-top:16px;font-weight:600;}}\
             pre{{white-space:pre-wrap;word-break:break-word;background:#0b1020;color:#dbeafe;padding:12px;border-radius:8px;font-size:12px;}}\
             </style></head><body>\
             <h1>{title}</h1>\
             <p class='meta'>Example 05 - .wfrog front panel + Rust runtime</p>\
             <dl class='runtime-facts' aria-label='Runtime facts'>\
             <div><dt>Runtime</dt><dd>Rust reference runtime</dd></div>\
             <div><dt>Execution</dt><dd>{execution_path}</dd></div>\
             <div><dt>Compiler backend</dt><dd>{compiler_backend}</dd></div>\
             </dl>\
             {diagnostics}\
             <form method='post' action='/run'>\
             <div class='front-panel' data-panel-id='{panel_id}' data-coordinate-space='panel_pixels' data-runtime-language='rust' data-compiler-backend='{compiler_backend_id}' data-execution-path='{execution_path_id}' style='width:{panel_width}px;height:{panel_height}px;'>\
             {ctrl_html}{ind_html}</div>\
             <div class='actions'><button type='submit'>Run Example 05</button><a href='/state.json'>state.json</a></div>\
             </form><details><summary>Current runtime snapshot</summary><pre>{snapshot_pretty}</pre></details>\
             </body></html>",
            title = escape_html(snapshot["ui_runtime"]["panel"]["title"].as_str().unwrap_or("FROG")),
            diagnostics = diagnostics,
            execution_path = if uses_native_kernel { "native kernel bridge" } else { "contract executor" },
            compiler_backend = if uses_native_kernel { "LLVM native kernel artifact" } else { "none in runtime path" },
            compiler_backend_id = if uses_native_kernel { "llvm" } else { "none" },
            execution_path_id = if uses_native_kernel { "native_kernel_bridge" } else { "contract_executor" },
            panel_id = escape_html(panel["panel_id"].as_str().unwrap_or("main_panel")),
            panel_width = panel_width,
            panel_height = panel_height,
            ctrl_html = ctrl_html,
            ind_html = ind_html,
            snapshot_pretty = escape_html(&to_string_pretty(&snapshot).unwrap()),
        )
    }

    pub fn serve(mut self, host: &str, port: u16, open_browser: bool) -> Result<()> {
        let listener = TcpListener::bind((host, port))?;
        let address = listener.local_addr()?;
        let url = format!("http://{}:{}/", address.ip(), address.port());

        if open_browser {
            let _ = open_in_browser(&url);
        }
        println!("{url}");

        for stream in listener.incoming() {
            let mut stream = stream?;
            if let Err(error) = self.handle_connection(&mut stream) {
                let _ = write_response(
                    &mut stream,
                    "500 Internal Server Error",
                    "text/plain; charset=utf-8",
                    format!("{error}").into_bytes(),
                    None,
                );
            }
        }
        Ok(())
    }

    fn handle_connection(&mut self, stream: &mut TcpStream) -> Result<()> {
        let request = read_request(stream)?;
        if request.method == "GET" && request.path == "/" {
            return write_response(
                stream,
                "200 OK",
                "text/html; charset=utf-8",
                self.render_html().into_bytes(),
                None,
            );
        }
        if request.method == "GET" && request.path == "/state.json" {
            let payload = to_string_pretty(&self.execution_artifact()).unwrap().into_bytes();
            return write_response(stream, "200 OK", "application/json; charset=utf-8", payload, None);
        }
        if request.method == "GET" && request.path.starts_with("/asset/") {
            let asset_id = request.path.trim_start_matches("/asset/");
            if let Some(path) = self.core.asset_map.get(asset_id) {
                if path.exists() {
                    return write_response(
                        stream,
                        "200 OK",
                        "image/svg+xml",
                        std::fs::read(path)?,
                        None,
                    );
                }
            }
            return write_response(
                stream,
                "404 Not Found",
                "text/plain; charset=utf-8",
                b"missing asset".to_vec(),
                None,
            );
        }
        if request.method == "POST" && request.path == "/run" {
            let body = String::from_utf8_lossy(&request.body);
            let value = parse_form_value(&body, "input_value").unwrap_or_else(|| "0".to_string());
            match value.parse::<u16>() {
                Ok(parsed) => {
                    if let Err(error) = self.run_once(parsed) {
                        self.last_error = Some(error.to_string());
                    }
                }
                Err(error) => self.last_error = Some(error.to_string()),
            }
            return write_response(
                stream,
                "303 See Other",
                "text/plain; charset=utf-8",
                Vec::new(),
                Some(("Location", "/".to_string())),
            );
        }
        write_response(
            stream,
            "404 Not Found",
            "text/plain; charset=utf-8",
            b"not found".to_vec(),
            None,
        )
    }
}

#[derive(Clone, Copy)]
struct NumericSvgGeometry {
    view_width: f64,
    view_height: f64,
    label_x: f64,
    label_y: f64,
    value_box_x: f64,
    value_box_y: f64,
    value_box_width: f64,
    value_box_height: f64,
}

impl Default for NumericSvgGeometry {
    fn default() -> Self {
        Self {
            view_width: 220.0,
            view_height: 88.0,
            label_x: 16.0,
            label_y: 24.0,
            value_box_x: 14.0,
            value_box_y: 40.0,
            value_box_width: 192.0,
            value_box_height: 32.0,
        }
    }
}

fn layout_i64(layout: &Value, key: &str, fallback: i64) -> i64 {
    layout
        .get(key)
        .and_then(|value| value.as_i64().or_else(|| value.as_u64().map(|item| item as i64)))
        .unwrap_or(fallback)
}

fn css_percent(value: f64) -> String {
    format!("{value:.6}%")
}

fn pct(value: f64, total: f64) -> f64 {
    if total <= 0.0 {
        0.0
    } else {
        (value / total) * 100.0
    }
}

fn safe_css_color(value: &str, fallback: &str) -> String {
    let bytes = value.as_bytes();
    let valid_len = bytes.len() == 7 || bytes.len() == 9;
    let valid = valid_len && bytes.first() == Some(&b'#') && bytes[1..].iter().all(|byte| byte.is_ascii_hexdigit());
    if valid {
        value.to_string()
    } else {
        fallback.to_string()
    }
}

fn property_string(properties: &Map<String, Value>, key: &str, fallback: &str) -> String {
    properties
        .get(key)
        .and_then(Value::as_str)
        .unwrap_or(fallback)
        .to_string()
}

fn property_bool(properties: &Map<String, Value>, key: &str, fallback: bool) -> bool {
    properties.get(key).and_then(Value::as_bool).unwrap_or(fallback)
}

fn property_u16(properties: &Map<String, Value>, key: &str, fallback: u16) -> u16 {
    properties
        .get(key)
        .and_then(Value::as_u64)
        .and_then(|value| u16::try_from(value).ok())
        .unwrap_or(fallback)
}

fn svg_attribute(svg: &str, element_id: &str, attribute: &str) -> Option<String> {
    let marker = format!("id=\"{element_id}\"");
    let id_pos = svg.find(&marker)?;
    let tag_start = svg[..id_pos].rfind('<')?;
    let tag_end = svg[id_pos..].find('>')? + id_pos;
    let tag = &svg[tag_start..tag_end];
    let attr_marker = format!("{attribute}=\"");
    let attr_start = tag.find(&attr_marker)? + attr_marker.len();
    let attr_end = tag[attr_start..].find('"')? + attr_start;
    Some(tag[attr_start..attr_end].to_string())
}

fn svg_attribute_f64(svg: &str, element_id: &str, attribute: &str, fallback: f64) -> f64 {
    svg_attribute(svg, element_id, attribute)
        .and_then(|value| value.parse::<f64>().ok())
        .unwrap_or(fallback)
}

fn parse_translate_anchor(svg: &str, anchor_id: &str, x: &mut f64, y: &mut f64) {
    let Some(transform) = svg_attribute(svg, anchor_id, "transform") else {
        return;
    };
    let Some(start) = transform.find("translate(") else {
        return;
    };
    let Some(end) = transform[start..].find(')') else {
        return;
    };
    let payload = &transform[start + "translate(".len()..start + end];
    let mut parts = payload
        .split(|character: char| character == ',' || character.is_whitespace())
        .filter(|part| !part.is_empty());
    if let (Some(parsed_x), Some(parsed_y)) = (parts.next(), parts.next()) {
        if let (Ok(next_x), Ok(next_y)) = (parsed_x.parse::<f64>(), parsed_y.parse::<f64>()) {
            *x = next_x;
            *y = next_y;
        }
    }
}

fn load_numeric_svg_geometry(widget: &WidgetState) -> NumericSvgGeometry {
    let mut geometry = NumericSvgGeometry::default();
    let Some(path) = &widget.asset_path else {
        return geometry;
    };
    let Ok(svg) = fs::read_to_string(path) else {
        return geometry;
    };

    if let Some(start) = svg.find("viewBox=\"") {
        let value_start = start + "viewBox=\"".len();
        if let Some(value_end) = svg[value_start..].find('"') {
            let viewbox = &svg[value_start..value_start + value_end];
            let parts: Vec<&str> = viewbox.split_whitespace().collect();
            if parts.len() == 4 {
                if let (Ok(width), Ok(height)) = (parts[2].parse::<f64>(), parts[3].parse::<f64>()) {
                    if width > 0.0 && height > 0.0 {
                        geometry.view_width = width;
                        geometry.view_height = height;
                    }
                }
            }
        }
    }

    parse_translate_anchor(&svg, "label_anchor", &mut geometry.label_x, &mut geometry.label_y);
    geometry.value_box_x = svg_attribute_f64(&svg, "value_box", "x", geometry.value_box_x);
    geometry.value_box_y = svg_attribute_f64(&svg, "value_box", "y", geometry.value_box_y);
    geometry.value_box_width = svg_attribute_f64(&svg, "value_box", "width", geometry.value_box_width);
    geometry.value_box_height = svg_attribute_f64(&svg, "value_box", "height", geometry.value_box_height);
    geometry
}

fn svg_anchor_style(x: f64, y: f64, geometry: NumericSvgGeometry) -> String {
    format!(
        "left:{};top:{};",
        css_percent(pct(x, geometry.view_width)),
        css_percent(pct(y, geometry.view_height))
    )
}

fn svg_box_style(x: f64, y: f64, width: f64, height: f64, geometry: NumericSvgGeometry) -> String {
    format!(
        "left:{};top:{};width:{};height:{};",
        css_percent(pct(x, geometry.view_width)),
        css_percent(pct(y, geometry.view_height)),
        css_percent(pct(width, geometry.view_width)),
        css_percent(pct(height, geometry.view_height))
    )
}

fn render_numeric_widget(widget: &WidgetState) -> String {
    let is_control = widget.role == "control";
    let geometry = load_numeric_svg_geometry(widget);
    let x = layout_i64(&widget.layout, "x", 0);
    let y = layout_i64(&widget.layout, "y", 0);
    let width = layout_i64(&widget.layout, "width", 160);
    let height = layout_i64(&widget.layout, "height", 48);
    let value = property_u16(&widget.properties, "value", 0);
    let label = property_string(&widget.properties, "label", &widget.widget_id);
    let value_color = safe_css_color(&property_string(&widget.properties, "foreground_color", "#1f2933"), "#1f2933");
    let label_color = safe_css_color(&property_string(&widget.properties, "label_color", "#111827"), "#111827");
    let route = widget.asset_id.as_ref().map(|id| format!("/asset/{id}")).unwrap_or_default();

    let mut style = format!("position:absolute;left:{x}px;top:{y}px;width:{width}px;height:{height}px;");
    if !property_bool(&widget.properties, "visible", true) {
        style.push_str("display:none;");
    }

    let mut html = String::new();
    let _ = write!(
        html,
        "<section class='frog-widget numeric-widget {}' data-widget-id='{}' data-class-ref='{}' data-role='{}' data-frog-visual-law='wfrog-realization-state-map'",
        if is_control { "numeric-control" } else { "numeric-indicator" },
        escape_html(&widget.widget_id),
        escape_html(&widget.class_ref),
        escape_html(&widget.role)
    );
    if !route.is_empty() {
        let _ = write!(html, " data-asset-route='{}'", escape_html(&route));
    }
    let _ = write!(html, " style='{style}'>");

    if route.is_empty() {
        html.push_str("<div class='numeric-skin missing-skin'></div>");
    } else {
        let _ = write!(html, "<img class='numeric-skin' src='{}' alt='' aria-hidden='true' />", escape_html(&route));
    }

    let label_style = svg_anchor_style(geometry.label_x, geometry.label_y, geometry);
    let value_style = svg_box_style(
        geometry.value_box_x,
        geometry.value_box_y,
        geometry.value_box_width,
        geometry.value_box_height,
        geometry,
    );
    let _ = write!(
        html,
        "<span class='numeric-label-overlay' data-svg-anchor='label_anchor' style='{}color:{};'>{}</span>",
        label_style,
        escape_html(&label_color),
        escape_html(&label)
    );

    if is_control {
        let _ = write!(
            html,
            "<input id='{}_value' name='input_value' type='number' min='0' max='65535' class='numeric-value-overlay numeric-control-editor' data-svg-part='value_box' data-svg-anchor='value_anchor' style='{}color:{};' value='{}'{} />",
            escape_html(&widget.widget_id),
            value_style,
            escape_html(&value_color),
            value,
            if property_bool(&widget.properties, "enabled", true) { "" } else { " disabled" }
        );
    } else {
        let _ = write!(
            html,
            "<output class='numeric-value-overlay numeric-indicator-value' data-svg-part='value_box' data-svg-anchor='value_anchor' style='{}color:{};'>{}</output>",
            value_style,
            escape_html(&value_color),
            value
        );
    }

    html.push_str("</section>");
    html
}

pub struct BooleanBrowserUiRuntime {
    pub contract: Value,
    pub wfrog: Value,
    pub asset_map: BTreeMap<String, PathBuf>,
    pub current_value: bool,
    pub last_result: bool,
    pub last_error: Option<String>,
    pub native_kernel_bridge: Option<NativeBoolKernelBridge>,
}

impl BooleanBrowserUiRuntime {
    pub fn new(contract_path: PathBuf, wfrog_path: PathBuf) -> Result<Self> {
        Self::with_native_kernel_bridge(contract_path, wfrog_path, None)
    }

    pub fn with_native_kernel_bridge(
        contract_path: PathBuf,
        wfrog_path: PathBuf,
        native_kernel_bridge: Option<NativeBoolKernelBridge>,
    ) -> Result<Self> {
        let contract: Value = serde_json::from_str(&fs::read_to_string(contract_path)?)?;
        let wfrog: Value = serde_json::from_str(&fs::read_to_string(&wfrog_path)?)?;
        let mut asset_map = BTreeMap::new();
        if let Some(assets) = wfrog["svg_assets"].as_array() {
            for asset in assets {
                if let (Some(asset_id), Some(path)) = (asset["asset_id"].as_str(), asset["path"].as_str()) {
                    asset_map.insert(asset_id.to_string(), wfrog_path.parent().unwrap_or_else(|| std::path::Path::new("")).join(path));
                }
            }
        }
        let current_value = wfrog["front_panels"][0]["widgets"]
            .as_array()
            .and_then(|widgets| {
                widgets.iter().find_map(|widget| {
                    if widget["instance_id"].as_str() == Some("bool_input") {
                        widget["props"]["value"].as_bool()
                    } else {
                        None
                    }
                })
            })
            .unwrap_or(true);

        Ok(Self {
            contract,
            wfrog,
            asset_map,
            current_value,
            last_result: current_value,
            last_error: None,
            native_kernel_bridge,
        })
    }

    pub fn run_once(&mut self, input_value: bool) -> Result<Value> {
        self.current_value = input_value;
        if let Some(bridge) = &self.native_kernel_bridge {
            if bridge.manifest().source_lowered_unit != "Examples/06_boolean_value_roundtrip/main.lowering.json" {
                return Err(RuntimeError::Message("Unexpected native bool kernel source lowered unit.".to_string()));
            }
            let result = bridge.run(input_value);
            if !result.ok {
                self.last_error = Some(bridge.manifest().diagnostic(result.error_code));
                return Err(RuntimeError::Message(bridge.manifest().diagnostic(result.error_code)));
            }
            self.last_result = result.result;
        } else {
            self.last_result = input_value;
        }
        match self.execution_artifact() {
            Ok(artifact) => {
                self.last_error = None;
                Ok(artifact)
            }
            Err(error) => {
                self.last_error = Some(error.to_string());
                Err(error)
            }
        }
    }

    pub fn execution_artifact(&self) -> Result<Value> {
        let mut artifact = execute_reference_contract_case(
            &self.contract,
            &json!({"input_value": self.current_value}),
            Some(&self.wfrog),
        )?;
        artifact["execution_summary"]["input_value"] = Value::Bool(self.current_value);
        artifact["execution_summary"]["result"] = Value::Bool(self.last_result);
        artifact["outputs"]["public"]["result"] = Value::Bool(self.last_result);
        artifact["outputs"]["ui"]["bool_input"] = Value::Bool(self.current_value);
        artifact["outputs"]["ui"]["bool_result"] = Value::Bool(self.last_result);
        if let Some(widgets) = artifact["ui_runtime"]["widgets"].as_array_mut() {
            for widget in widgets {
                if widget["widget_id"].as_str() == Some("bool_input") {
                    widget["runtime"]["value"] = Value::Bool(self.current_value);
                }
                if widget["widget_id"].as_str() == Some("bool_result") {
                    widget["runtime"]["value"] = Value::Bool(self.last_result);
                }
            }
        }
        Ok(artifact)
    }

    pub fn render_html(&self) -> String {
        let snapshot = self.execution_artifact().unwrap();
        let panel = &snapshot["ui_runtime"]["panel"];
        let widgets = snapshot["ui_runtime"]["widgets"].as_array().unwrap();
        let panel_width = panel["layout"]["width"].as_i64().unwrap_or(420);
        let panel_height = panel["layout"]["height"].as_i64().unwrap_or(150);

        let mut diagnostics = String::new();
        if let Some(message) = &self.last_error {
            let _ = write!(
                diagnostics,
                "<div class='diagnostic error'>{}</div>",
                escape_html(message)
            );
        }
        let uses_native_kernel = self.native_kernel_bridge.is_some();

        let rendered_widgets = widgets
            .iter()
            .map(render_boolean_widget)
            .collect::<Vec<String>>()
            .join("");

        format!(
            "<!doctype html><html lang='en'><head><meta charset='utf-8'><title>{title}</title>\
             <style>\
             body{{font-family:Segoe UI,Arial,sans-serif;margin:24px;background:#f3f6f8;color:#1f2933;}}\
             h1{{font-size:24px;margin:0 0 12px 0;}}\
             p.meta{{margin:0 0 20px 0;color:#52606d;}}\
             .runtime-facts{{display:flex;flex-wrap:wrap;gap:8px;margin:-8px 0 18px 0;}}\
             .runtime-facts div{{display:flex;gap:6px;align-items:baseline;padding:6px 8px;border:1px solid #d9e2ec;border-radius:6px;background:#ffffff;}}\
             .runtime-facts dt{{margin:0;color:#52606d;font-size:11px;font-weight:700;text-transform:uppercase;}}\
             .runtime-facts dd{{margin:0;color:#1f2933;font-size:12px;font-weight:600;}}\
             .front-panel{{position:relative;background:#ffffff;border-radius:10px;box-shadow:0 4px 14px rgba(15,23,42,0.08);overflow:hidden;}}\
             .frog-widget{{position:absolute;box-sizing:border-box;}}\
             .boolean-widget{{border:0;padding:0;background:transparent;font:inherit;color:inherit;overflow:visible;}}\
             .boolean-control{{cursor:pointer;}}\
             .boolean-indicator{{pointer-events:none;}}\
             .boolean-skin{{position:absolute;inset:0;width:100%;height:100%;object-fit:fill;display:block;pointer-events:none;z-index:2;}}\
             .boolean-caption-overlay{{position:absolute;left:var(--boolean-caption-left);top:6px;transform:var(--boolean-caption-transform);text-align:var(--boolean-caption-text-align);font-size:14px;font-weight:600;line-height:1;color:#1f2933;white-space:nowrap;pointer-events:none;z-index:3;}}\
             .boolean-state-face{{position:absolute;left:var(--boolean-inner-left);top:var(--boolean-inner-top);width:var(--boolean-inner-width);height:var(--boolean-inner-height);border:2px solid var(--boolean-inner-border);border-radius:7px;background:var(--boolean-fill);box-shadow:inset 0 1px 0 rgba(255,255,255,.6),0 1px 2px rgba(15,23,42,.16);transition:background var(--boolean-transition),border-color var(--boolean-transition),box-shadow var(--boolean-transition),transform var(--boolean-transition);z-index:1;}}\
             .boolean-widget[data-realization-variant='circular'] .boolean-state-face{{border-radius:50%;}}\
             .boolean-widget[data-frog-frame-visible='false'] .boolean-state-face{{box-shadow:none;}}\
             .boolean-control:hover .boolean-state-face{{background:var(--boolean-hover-fill);border-color:var(--boolean-hover-inner-border);box-shadow:inset 0 1px 0 rgba(255,255,255,.72),0 2px 5px rgba(15,23,42,.18);}}\
             .boolean-control[data-frog-frame-visible='false']:hover .boolean-state-face{{box-shadow:none;}}\
             .boolean-control:active .boolean-state-face{{background:var(--boolean-pressed-fill);border-color:var(--boolean-pressed-inner-border);box-shadow:inset 0 2px 4px rgba(15,23,42,.22);transform:translateY(var(--boolean-pressed-inset));}}\
             .boolean-control[data-frog-frame-visible='false']:active .boolean-state-face{{box-shadow:none;}}\
             .boolean-control:focus-visible .boolean-state-face{{outline:2px solid #2563eb;outline-offset:2px;}}\
             .boolean-state-overlay{{position:absolute;left:0;right:0;top:49px;transform:translateY(-50%);text-align:center;font-size:18px;font-weight:700;line-height:1;color:var(--boolean-text);pointer-events:none;z-index:4;}}\
             .actions{{margin-top:16px;display:flex;gap:12px;align-items:center;}}\
             .state-link{{font-size:16px;}}\
             .diagnostic{{margin:12px 0;padding:10px 12px;border-radius:6px;}}\
             .diagnostic.error{{background:#fff1f2;color:#9f1239;border:1px solid #fecdd3;}}\
             </style></head><body>\
             <h1>{title}</h1>\
             <p class='meta'>Example 06 - .wfrog front panel + Default Boolean realization assets + Rust runtime</p>\
             <dl class='runtime-facts' aria-label='Runtime facts'>\
             <div><dt>Runtime</dt><dd>Rust reference runtime</dd></div>\
             <div><dt>Execution</dt><dd>{execution_path}</dd></div>\
             <div><dt>Compiler backend</dt><dd>{compiler_backend}</dd></div>\
             </dl>\
             {diagnostics}\
             <form method='post' action='/run'>\
             <div class='front-panel' data-panel-id='{panel_id}' data-coordinate-space='panel_pixels' data-runtime-language='rust' data-compiler-backend='{compiler_backend_id}' data-execution-path='{execution_path_id}' style='width:{panel_width}px;height:{panel_height}px;'>\
             {rendered_widgets}\
             </div><div class='actions'><a class='state-link' href='/state.json'>state.json</a></div></form>\
             </body></html>",
            title = escape_html(panel["title"].as_str().unwrap_or("FROG")),
            diagnostics = diagnostics,
            execution_path = if uses_native_kernel { "native kernel bridge" } else { "boolean contract executor" },
            compiler_backend = if uses_native_kernel { "LLVM native bool kernel artifact" } else { "none for Example 06" },
            compiler_backend_id = if uses_native_kernel { "llvm" } else { "none" },
            execution_path_id = if uses_native_kernel { "native_kernel_bridge" } else { "rust_boolean_contract_executor" },
            panel_id = escape_html(panel["panel_id"].as_str().unwrap_or("")),
            panel_width = panel_width,
            panel_height = panel_height,
            rendered_widgets = rendered_widgets,
        )
    }

    pub fn serve(mut self, host: &str, port: u16, open_browser: bool) -> Result<()> {
        let listener = TcpListener::bind((host, port))?;
        let address = listener.local_addr()?;
        let url = format!("http://{}:{}/", address.ip(), address.port());

        if open_browser {
            let _ = open_in_browser(&url);
        }
        println!("{url}");

        for stream in listener.incoming() {
            let mut stream = stream?;
            if let Err(error) = self.handle_connection(&mut stream) {
                let _ = write_response(
                    &mut stream,
                    "500 Internal Server Error",
                    "text/plain; charset=utf-8",
                    format!("{error}").into_bytes(),
                    None,
                );
            }
        }
        Ok(())
    }

    fn handle_connection(&mut self, stream: &mut TcpStream) -> Result<()> {
        let request = read_request(stream)?;
        if request.method == "GET" && request.path == "/" {
            return write_response(
                stream,
                "200 OK",
                "text/html; charset=utf-8",
                self.render_html().into_bytes(),
                None,
            );
        }
        if request.method == "GET" && request.path == "/state.json" {
            let payload = to_string_pretty(&self.execution_artifact()?).unwrap().into_bytes();
            return write_response(stream, "200 OK", "application/json; charset=utf-8", payload, None);
        }
        if request.method == "GET" && request.path.starts_with("/asset/") {
            let asset_id = request.path.trim_start_matches("/asset/");
            if let Some(path) = self.asset_map.get(asset_id) {
                if path.exists() {
                    return write_response(
                        stream,
                        "200 OK",
                        "image/svg+xml",
                        fs::read(path)?,
                        None,
                    );
                }
            }
            return write_response(
                stream,
                "404 Not Found",
                "text/plain; charset=utf-8",
                b"missing asset".to_vec(),
                None,
            );
        }
        if request.method == "POST" && request.path == "/run" {
            let body = String::from_utf8_lossy(&request.body);
            let value = parse_form_value(&body, "input_value").unwrap_or_else(|| "false".to_string());
            match parse_bool_value(&value) {
                Ok(parsed) => {
                    if let Err(error) = self.run_once(parsed) {
                        self.last_error = Some(error.to_string());
                    }
                }
                Err(error) => self.last_error = Some(error.to_string()),
            }
            return write_response(
                stream,
                "303 See Other",
                "text/plain; charset=utf-8",
                Vec::new(),
                Some(("Location", "/".to_string())),
            );
        }
        write_response(
            stream,
            "404 Not Found",
            "text/plain; charset=utf-8",
            b"not found".to_vec(),
            None,
        )
    }
}

fn render_boolean_widget(widget: &Value) -> String {
    let runtime = &widget["runtime"];
    let layout = &widget["layout"];
    let value = runtime["value"].as_bool().unwrap_or(false);
    let is_control = widget["role"].as_str() == Some("control");
    let visual_state = if value { "true" } else { "false" };
    let hover_state = if value { "hover_true" } else { "hover_false" };
    let pressed_state = if value { "pressed_true" } else { "pressed_false" };
    let transition_state = if value { "transition_true_to_false" } else { "transition_false_to_true" };
    let variant = runtime_string(runtime, "realization.variant", if is_control { "rectangular" } else { "circular" });
    let next_value = if value { "false" } else { "true" };
    let state_text_visible = runtime_bool(runtime, "state_text.visible", true);
    let frame_visible = runtime_bool(runtime, "style.frame.visible", true);
    let caption_centered = runtime_string(runtime, "caption.align.horizontal", "left") == "center";

    let state_fill = state_property(runtime, "style.inner.fill_color", visual_state, if value { "#8bd86f" } else { "#ffffff" });
    let hover_fill = state_property(runtime, "style.inner.fill_color", hover_state, if value { "#9be884" } else { "#eef6ff" });
    let pressed_fill = state_property(runtime, "style.inner.fill_color", pressed_state, if value { "#6fc657" } else { "#dbeafe" });
    let state_border = state_property(runtime, "style.outer.border_color", visual_state, if value { "#184a24" } else { "#111827" });
    let hover_border = state_property(runtime, "style.outer.border_color", hover_state, if value { "#166534" } else { "#2563eb" });
    let pressed_border = state_property(runtime, "style.outer.border_color", pressed_state, if value { "#14532d" } else { "#1d4ed8" });
    let state_inner_border = state_property(runtime, "style.inner.border_color", visual_state, &state_border);
    let hover_inner_border = state_property(runtime, "style.inner.border_color", hover_state, &hover_border);
    let pressed_inner_border = state_property(runtime, "style.inner.border_color", pressed_state, &pressed_border);
    let text_color = state_property(runtime, "state_text.style.text_color", visual_state, if value { "#0b3d19" } else { "#111827" });
    let inner_left = runtime_string(runtime, "style.inner.left", if variant == "circular" { "52px" } else { "18px" });
    let inner_top = runtime_string(runtime, "style.inner.top", if variant == "circular" { "23px" } else { "31px" });
    let inner_width = runtime_string(runtime, "style.inner.width", if variant == "circular" { "56px" } else { "124px" });
    let inner_height = runtime_string(runtime, "style.inner.height", if variant == "circular" { "56px" } else { "34px" });
    let transition_ms = runtime_string(runtime, "style.transition.duration_ms", "120");
    let transition_timing = runtime_string(runtime, "style.transition.timing", "ease-out");
    let pressed_inset = runtime_string(runtime, "style.pressed.inset", "1px");
    let asset_ref = runtime["asset_ref"].as_str().unwrap_or("");
    let asset_route = asset_ref.strip_prefix("asset:").map(|id| format!("/asset/{id}")).unwrap_or_default();
    let widget_id = widget["widget_id"].as_str().unwrap_or("");
    let class_ref = widget["class_ref"].as_str().unwrap_or("");
    let role = widget["role"].as_str().unwrap_or("");
    let caption = runtime_string(runtime, "caption.text", widget_id);

    let style = format!(
        "position:absolute;left:{}px;top:{}px;width:{}px;height:{}px;\
         --boolean-fill:{};--boolean-hover-fill:{};--boolean-pressed-fill:{};\
         --boolean-border:{};--boolean-hover-border:{};--boolean-pressed-border:{};\
         --boolean-inner-border:{};--boolean-hover-inner-border:{};--boolean-pressed-inner-border:{};\
         --boolean-inner-left:{};--boolean-inner-top:{};--boolean-inner-width:{};--boolean-inner-height:{};\
         --boolean-caption-left:{};--boolean-caption-transform:{};--boolean-caption-text-align:{};\
         --boolean-text:{};--boolean-transition:{}ms {};--boolean-pressed-inset:{};",
        layout["x"].as_i64().unwrap_or(0),
        layout["y"].as_i64().unwrap_or(0),
        layout["width"].as_i64().unwrap_or(160),
        layout["height"].as_i64().unwrap_or(80),
        state_fill,
        hover_fill,
        pressed_fill,
        state_border,
        hover_border,
        pressed_border,
        state_inner_border,
        hover_inner_border,
        pressed_inner_border,
        inner_left,
        inner_top,
        inner_width,
        inner_height,
        if caption_centered { "50%" } else { "8px" },
        if caption_centered { "translateX(-50%)" } else { "none" },
        if caption_centered { "center" } else { "left" },
        text_color,
        transition_ms,
        transition_timing,
        pressed_inset,
    );
    let skin = format!(
        "<span class='boolean-state-face' data-frog-part='inner_face' aria-hidden='true'></span>\
         <img class='boolean-skin' src='{}' alt='' aria-hidden='true'>",
        escape_html(&asset_route)
    );
    let mut overlays = format!(
        "<span class='boolean-caption-overlay' data-frog-part='caption'>{}</span>",
        escape_html(&caption)
    );
    if state_text_visible {
        let text = runtime_string(runtime, if value { "state_text.true_text" } else { "state_text.false_text" }, if value { "TRUE" } else { "FALSE" });
        let _ = write!(
            overlays,
            "<span class='boolean-state-overlay' data-frog-part='state_text'>{}</span>",
            escape_html(&text)
        );
    }
    let attrs = format!(
        " data-widget-id='{}' data-class-ref='{}' data-role='{}' data-asset-ref='{}' data-asset-route='{}'\
         data-current-value='{}' data-realization-variant='{}' data-frog-visual-law='wfrog-realization-state-map'\
         data-frog-visual-state='{}' data-frog-hover-state='{}' data-frog-pressed-state='{}' data-frog-transition-state='{}'\
         data-frog-state-text-visible='{}' data-frog-frame-visible='{}'",
        escape_html(widget_id),
        escape_html(class_ref),
        escape_html(role),
        escape_html(asset_ref),
        escape_html(&asset_route),
        if value { "true" } else { "false" },
        escape_html(&variant),
        visual_state,
        hover_state,
        pressed_state,
        transition_state,
        if state_text_visible { "true" } else { "false" },
        if frame_visible { "true" } else { "false" },
    );
    if is_control {
        format!(
            "<button class='frog-widget boolean-widget boolean-control' type='submit' name='input_value' value='{next_value}' data-toggle-target='{next_value}' aria-pressed='{pressed}'{attrs} style='{style}'>{skin}{overlays}</button>",
            pressed = if value { "true" } else { "false" },
        )
    } else {
        format!(
            "<section class='frog-widget boolean-widget boolean-indicator' aria-readonly='true'{attrs} style='{style}'>{skin}{overlays}</section>"
        )
    }
}

fn runtime_string(value: &Value, key: &str, fallback: &str) -> String {
    value[key].as_str().unwrap_or(fallback).to_string()
}

fn runtime_bool(value: &Value, key: &str, fallback: bool) -> bool {
    value[key].as_bool().unwrap_or(fallback)
}

fn state_property(value: &Value, base: &str, state: &str, fallback: &str) -> String {
    runtime_string(value, &format!("{base}.{state}"), fallback)
}

fn parse_bool_value(value: &str) -> Result<bool> {
    match value.trim().to_ascii_lowercase().as_str() {
        "true" | "1" | "on" => Ok(true),
        "false" | "0" | "" => Ok(false),
        _ => Err(RuntimeError::Message("Boolean input must be true or false.".to_string())),
    }
}

#[derive(Debug)]
struct Request {
    method: String,
    path: String,
    body: Vec<u8>,
}

fn read_request(stream: &mut TcpStream) -> Result<Request> {
    let mut reader = BufReader::new(stream.try_clone()?);
    let mut request_line = String::new();
    reader.read_line(&mut request_line)?;
    if request_line.trim().is_empty() {
        return Err(RuntimeError::Message("Empty request.".to_string()));
    }
    let mut parts = request_line.split_whitespace();
    let method = parts.next().unwrap_or("").to_string();
    let path = parts.next().unwrap_or("/").to_string();

    let mut content_length: usize = 0;
    loop {
        let mut line = String::new();
        reader.read_line(&mut line)?;
        if line == "\r\n" || line == "\n" || line.is_empty() {
            break;
        }
        if let Some(value) = line.strip_prefix("Content-Length:") {
            content_length = value.trim().parse::<usize>()?;
        }
    }
    let mut body = vec![0u8; content_length];
    if content_length > 0 {
        reader.read_exact(&mut body)?;
    }
    Ok(Request { method, path, body })
}

fn write_response(
    stream: &mut TcpStream,
    status: &str,
    content_type: &str,
    body: Vec<u8>,
    extra_header: Option<(&str, String)>,
) -> Result<()> {
    let mut headers = format!(
        "HTTP/1.1 {status}\r\nContent-Type: {content_type}\r\nContent-Length: {}\r\nConnection: close\r\n",
        body.len()
    );
    if let Some((name, value)) = extra_header {
        let _ = write!(headers, "{name}: {value}\r\n");
    }
    headers.push_str("\r\n");
    stream.write_all(headers.as_bytes())?;
    stream.write_all(&body)?;
    stream.flush()?;
    Ok(())
}

fn parse_form_value(body: &str, key: &str) -> Option<String> {
    for pair in body.split('&') {
        let mut parts = pair.splitn(2, '=');
        let current_key = parts.next()?;
        let current_value = parts.next().unwrap_or("");
        if current_key == key {
            return Some(url_decode(current_value));
        }
    }
    None
}

fn url_decode(input: &str) -> String {
    let mut out = String::new();
    let mut bytes = input.as_bytes().iter().copied();
    while let Some(byte) = bytes.next() {
        match byte {
            b'+' => out.push(' '),
            b'%' => {
                let hi = bytes.next().unwrap_or(b'0');
                let lo = bytes.next().unwrap_or(b'0');
                let value = hex_value(hi) * 16 + hex_value(lo);
                out.push(value as char);
            }
            _ => out.push(byte as char),
        }
    }
    out
}

fn hex_value(byte: u8) -> u8 {
    match byte {
        b'0'..=b'9' => byte - b'0',
        b'a'..=b'f' => 10 + byte - b'a',
        b'A'..=b'F' => 10 + byte - b'A',
        _ => 0,
    }
}

fn escape_html(input: &str) -> String {
    input
        .replace('&', "&amp;")
        .replace('<', "&lt;")
        .replace('>', "&gt;")
        .replace('\"', "&quot;")
        .replace('\'', "&#39;")
}

fn open_in_browser(url: &str) -> Result<()> {
    #[cfg(target_os = "windows")]
    {
        Command::new("cmd").args(["/C", "start", "", url]).spawn()?;
        return Ok(());
    }
    #[cfg(target_os = "macos")]
    {
        Command::new("open").arg(url).spawn()?;
        return Ok(());
    }
    #[cfg(not(any(target_os = "windows", target_os = "macos")))]
    {
        Command::new("xdg-open").arg(url).spawn()?;
        return Ok(());
    }
}
