use std::collections::BTreeMap;
use std::fmt::Write as _;
use std::fs;
use std::io::{BufRead, BufReader, Read, Write};
use std::net::{TcpListener, TcpStream};
use std::path::PathBuf;
use std::process::Command;

use serde_json::{json, to_string_pretty, Map, Value};

use crate::contract::{default_contract_path, default_wfrog_path, find_repo_root, load_front_panel_from_frog_source_path};
use crate::diagnostics::{Result, RuntimeError};
use crate::execute::execute_reference_contract_case;
use crate::native_kernel::{NativeBoolKernelBridge, NativeEnumKernelBridge, NativeKernelBridge, NativeStringKernelBridge};
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
             .numeric-skin{{position:absolute;inset:0;width:100%;height:100%;display:block;}}\
             .numeric-skin svg{{width:100%;height:100%;display:block;}}\
             .missing-skin{{background:#e5e7eb;border:1px solid #9ca3af;border-radius:6px;}}\
             .numeric-label-overlay{{position:absolute;transform:translateY(-50%);font-size:12px;line-height:1;white-space:nowrap;pointer-events:none;}}\
             .numeric-value-overlay{{position:absolute;box-sizing:border-box;font-family:Consolas,Segoe UI Mono,monospace;font-size:11px;font-weight:700;line-height:1;border:0;background:transparent;}}\
             .numeric-control-editor{{padding:0 4px;border-radius:0;outline:0;background:transparent;appearance:textfield;-moz-appearance:textfield;}}\
             .numeric-control-editor::-webkit-outer-spin-button,.numeric-control-editor::-webkit-inner-spin-button{{appearance:none;margin:0;}}\
             .numeric-control-editor:focus{{outline:0;background:transparent;}}\
             .numeric-indicator-value{{display:flex;align-items:center;padding:0 4px;pointer-events:none;}}\
             .numeric-step-overlay{{position:absolute;box-sizing:border-box;padding:0;border:1px solid var(--frog-numeric-step-border);border-radius:0;background:var(--frog-numeric-step-fill);color:transparent;cursor:pointer;display:flex;align-items:center;justify-content:center;}}\
             .numeric-step-overlay:focus{{outline:0;}}\
             .numeric-step-overlay:active{{background:var(--frog-numeric-step-fill-pressed);border-color:var(--frog-numeric-step-border-pressed);}}\
             .numeric-step-overlay::before{{content:'';display:block;width:0;height:0;}}\
             .numeric-increment::before{{border-left:5px solid transparent;border-right:5px solid transparent;border-bottom:6px solid var(--frog-numeric-step-symbol);}}\
             .numeric-decrement::before{{border-left:5px solid transparent;border-right:5px solid transparent;border-top:6px solid var(--frog-numeric-step-symbol);}}\
             .numeric-increment:active::before{{border-bottom-color:var(--frog-numeric-step-symbol-pressed);}}\
             .numeric-decrement:active::before{{border-top-color:var(--frog-numeric-step-symbol-pressed);}}\
             .actions{{margin-top:16px;display:flex;gap:12px;align-items:center;}}\
             button{{padding:8px 14px;border:0;border-radius:6px;cursor:pointer;background:#0f62fe;color:#ffffff;font-weight:600;}}\
             .numeric-step-overlay{{padding:0;border-radius:0;color:transparent;font-weight:400;}}\
             .diagnostic{{margin:12px 0;padding:10px 12px;border-radius:6px;}}\
             .diagnostic.error{{background:#fff1f2;color:#9f1239;border:1px solid #fecdd3;}}\
             summary{{cursor:pointer;margin-top:16px;font-weight:600;}}\
             pre{{white-space:pre-wrap;word-break:break-word;background:#0b1020;color:#dbeafe;padding:12px;border-radius:8px;font-size:12px;}}\
             </style><script>\
             document.addEventListener('click',function(event){{\
             const button=event.target.closest('.numeric-step-overlay');\
             if(!button)return;\
             const input=document.getElementById(button.dataset.target);\
             if(!input||input.disabled)return;\
             const step=Number(button.dataset.step||'1');\
             const min=Number(input.min||'0');\
             const max=Number(input.max||'65535');\
             const next=Math.min(max,Math.max(min,Number(input.value||'0')+step));\
             input.value=String(next);\
             input.dispatchEvent(new Event('input',{{bubbles:true}}));\
             }});\
             </script></head><body>\
             <h1>{title}</h1>\
             <p class='meta'>Example 05 - .frog front panel + Default Numeric .wfrog realization assets + Rust runtime</p>\
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
             </form>\
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
    caption_x: f64,
    caption_y: f64,
    value_face_x: f64,
    value_face_y: f64,
    value_face_width: f64,
    value_face_height: f64,
    increment_up_x: f64,
    increment_up_y: f64,
    increment_up_width: f64,
    increment_up_height: f64,
    increment_down_x: f64,
    increment_down_y: f64,
    increment_down_width: f64,
    increment_down_height: f64,
}

impl Default for NumericSvgGeometry {
    fn default() -> Self {
        Self {
            view_width: 380.0,
            view_height: 150.0,
            caption_x: 16.0,
            caption_y: 46.0,
            value_face_x: 22.0,
            value_face_y: 82.0,
            value_face_width: 214.0,
            value_face_height: 28.0,
            increment_up_x: 246.0,
            increment_up_y: 82.0,
            increment_up_width: 30.0,
            increment_up_height: 13.0,
            increment_down_x: 246.0,
            increment_down_y: 97.0,
            increment_down_width: 30.0,
            increment_down_height: 13.0,
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

fn css_percent_compact(value: f64) -> String {
    let mut formatted = format!("{value:.6}");
    if formatted.contains('.') {
        while formatted.ends_with('0') {
            formatted.pop();
        }
        if formatted.ends_with('.') {
            formatted.pop();
        }
    }
    format!("{formatted}%")
}

fn pct(value: f64, total: f64) -> f64 {
    if total <= 0.0 {
        0.0
    } else {
        (value / total) * 100.0
    }
}

fn safe_css_color(value: &str, fallback: &str) -> String {
    if value == "transparent" {
        return value.to_string();
    }
    let bytes = value.as_bytes();
    let valid_len = bytes.len() == 7 || bytes.len() == 9;
    let valid = valid_len && bytes.first() == Some(&b'#') && bytes[1..].iter().all(|byte| byte.is_ascii_hexdigit());
    if valid {
        value.to_string()
    } else {
        fallback.to_string()
    }
}

fn safe_css_length(value: &str, fallback: &str) -> String {
    let suffix = "px";
    if !value.ends_with(suffix) {
        return fallback.to_string();
    }
    let number = &value[..value.len() - suffix.len()];
    if !number.is_empty() && number.parse::<f64>().is_ok() {
        value.to_string()
    } else {
        fallback.to_string()
    }
}

fn safe_css_font_weight(value: &str, fallback: &str) -> String {
    if matches!(value, "normal" | "bold" | "lighter" | "bolder") {
        return value.to_string();
    }
    if value.len() == 3
        && value.as_bytes().iter().all(u8::is_ascii_digit)
        && value.parse::<u16>().is_ok_and(|weight| (100..=900).contains(&weight) && weight % 100 == 0)
    {
        value.to_string()
    } else {
        fallback.to_string()
    }
}

fn safe_css_font_style(value: &str, fallback: &str) -> String {
    if matches!(value, "normal" | "italic" | "oblique") {
        value.to_string()
    } else {
        fallback.to_string()
    }
}

fn safe_css_font_family(value: &str, fallback: &str) -> String {
    if !value.is_empty()
        && value
            .chars()
            .all(|ch| ch.is_ascii_alphanumeric() || matches!(ch, ' ' | ',' | '.' | '_' | '-'))
    {
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

fn property_number(properties: &Map<String, Value>, key: &str, fallback: f64) -> f64 {
    properties.get(key).and_then(Value::as_f64).unwrap_or(fallback)
}

fn property_u16(properties: &Map<String, Value>, key: &str, fallback: u16) -> u16 {
    properties
        .get(key)
        .and_then(Value::as_u64)
        .and_then(|value| u16::try_from(value).ok())
        .unwrap_or(fallback)
}

fn property_step(properties: &Map<String, Value>, key: &str, fallback: u16) -> u16 {
    let value = property_u16(properties, key, fallback);
    if value == 0 { fallback } else { value }
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

fn svg_child_rect_attribute(svg: &str, group_id: &str, attribute: &str) -> Option<String> {
    let marker = format!("id=\"{group_id}\"");
    let id_pos = svg.find(&marker)?;
    let group_end = svg[id_pos..].find("</g>")? + id_pos;
    let rect_start = svg[id_pos..group_end].find("<rect")? + id_pos;
    let rect_end = svg[rect_start..group_end].find('>')? + rect_start;
    let tag = &svg[rect_start..rect_end];
    let attr_marker = format!("{attribute}=\"");
    let attr_start = tag.find(&attr_marker)? + attr_marker.len();
    let attr_end = tag[attr_start..].find('"')? + attr_start;
    Some(tag[attr_start..attr_end].to_string())
}

fn svg_child_rect_attribute_f64(svg: &str, group_id: &str, attribute: &str, fallback: f64) -> f64 {
    svg_child_rect_attribute(svg, group_id, attribute)
        .and_then(|value| value.parse::<f64>().ok())
        .unwrap_or(fallback)
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

    geometry.caption_x = svg_attribute_f64(&svg, "caption_text", "x", geometry.caption_x);
    geometry.caption_y = svg_attribute_f64(&svg, "caption_text", "y", geometry.caption_y);
    geometry.value_face_x = svg_attribute_f64(&svg, "value_face", "x", geometry.value_face_x);
    geometry.value_face_y = svg_attribute_f64(&svg, "value_face", "y", geometry.value_face_y);
    geometry.value_face_width = svg_attribute_f64(&svg, "value_face", "width", geometry.value_face_width);
    geometry.value_face_height = svg_attribute_f64(&svg, "value_face", "height", geometry.value_face_height);
    geometry.increment_up_x = svg_child_rect_attribute_f64(&svg, "increment_up", "x", geometry.increment_up_x);
    geometry.increment_up_y = svg_child_rect_attribute_f64(&svg, "increment_up", "y", geometry.increment_up_y);
    geometry.increment_up_width = svg_child_rect_attribute_f64(&svg, "increment_up", "width", geometry.increment_up_width);
    geometry.increment_up_height = svg_child_rect_attribute_f64(&svg, "increment_up", "height", geometry.increment_up_height);
    geometry.increment_down_x = svg_child_rect_attribute_f64(&svg, "increment_down", "x", geometry.increment_down_x);
    geometry.increment_down_y = svg_child_rect_attribute_f64(&svg, "increment_down", "y", geometry.increment_down_y);
    geometry.increment_down_width = svg_child_rect_attribute_f64(&svg, "increment_down", "width", geometry.increment_down_width);
    geometry.increment_down_height = svg_child_rect_attribute_f64(&svg, "increment_down", "height", geometry.increment_down_height);
    geometry
}

fn svg_anchor_style(x: f64, y: f64, geometry: NumericSvgGeometry) -> String {
    format!(
        "left:{};top:{};",
        css_percent(pct(x, geometry.view_width)),
        css_percent(pct(y, geometry.view_height))
    )
}

fn caption_transform_for_align(align: &str) -> &'static str {
    match align {
        "center" => "translate(-50%,-50%)",
        "right" | "end" => "translate(-100%,-50%)",
        _ => "translateY(-50%)",
    }
}

fn caption_text_align(align: &str) -> &'static str {
    match align {
        "center" => "center",
        "right" | "end" => "right",
        _ => "left",
    }
}

fn caption_anchor_style(properties: &Map<String, Value>, geometry: NumericSvgGeometry) -> String {
    let x = property_number(properties, "caption.anchor.x", geometry.caption_x);
    let y = property_number(properties, "caption.anchor.y", geometry.caption_y);
    let align = property_string(properties, "caption.align.horizontal", "left");
    let mut style = svg_anchor_style(x, y, geometry);
    let _ = write!(style, "transform:{};text-align:{};", caption_transform_for_align(&align), caption_text_align(&align));
    if !property_bool(properties, "caption.visible", true) {
        style.push_str("display:none;");
    }
    style
}

fn runtime_caption_anchor_style(runtime: &Value, geometry: NumericSvgGeometry) -> String {
    let x = runtime["caption.anchor.x"].as_f64().unwrap_or(geometry.caption_x);
    let y = runtime["caption.anchor.y"].as_f64().unwrap_or(geometry.caption_y);
    let align = runtime_string(runtime, "caption.align.horizontal", "left");
    let mut style = svg_anchor_style(x, y, geometry);
    let _ = write!(style, "transform:{};text-align:{};", caption_transform_for_align(&align), caption_text_align(&align));
    if !runtime_bool(runtime, "caption.visible", true) {
        style.push_str("display:none;");
    }
    style
}

fn runtime_caption_anchor_vars(runtime: &Value, geometry: NumericSvgGeometry, prefix: &str) -> String {
    let x = runtime["caption.anchor.x"].as_f64().unwrap_or(geometry.caption_x);
    let y = runtime["caption.anchor.y"].as_f64().unwrap_or(geometry.caption_y);
    format!(
        "--{}-caption-left:{};--{}-caption-top:{};",
        prefix,
        css_percent_compact(pct(x, geometry.view_width)),
        prefix,
        css_percent_compact(pct(y, geometry.view_height))
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

fn render_numeric_skin(widget: &WidgetState, is_control: bool, value_face_color: &str) -> String {
    let Some(path) = &widget.asset_path else {
        return "<div class='numeric-skin missing-skin'></div>".to_string();
    };
    let Ok(svg) = fs::read_to_string(path) else {
        return "<div class='numeric-skin missing-skin'></div>".to_string();
    };
    let unit_display = if property_bool(&widget.properties, "unit_label.visible", false) { "inline" } else { "none" };
    let radix_display = if property_bool(&widget.properties, "display.radix_visible", false) { "inline" } else { "none" };
    let spinner_display = if is_control && property_bool(&widget.properties, "display.increment_buttons_visible", true) { "inline" } else { "none" };
    let frame_fill = safe_css_color(&property_string(&widget.properties, "style.frame.fill_color", "#ffffff"), "#ffffff");
    let frame_stroke = safe_css_color(&property_string(&widget.properties, "style.frame.border_color", "#000000"), "#000000");
    let frame_stroke_width = safe_css_length(&property_string(&widget.properties, "style.frame.border_width", "2px"), "2px");
    let value_face_fill = safe_css_color(&property_string(&widget.properties, "style.value_face.fill_color", value_face_color), value_face_color);
    let value_face_stroke = safe_css_color(&property_string(&widget.properties, "style.value_face.border_color", "transparent"), "transparent");
    let value_face_stroke_width = safe_css_length(&property_string(&widget.properties, "style.value_face.border_width", "0px"), "0px");
    let step_fill = safe_css_color(&property_string(&widget.properties, "style.increment_button.fill_color.normal", value_face_color), value_face_color);
    let step_symbol = safe_css_color(&property_string(&widget.properties, "style.increment_button.symbol_color.normal", "#ffffff"), "#ffffff");
    format!(
        "<div class='numeric-skin' aria-hidden='true' style='--frog-numeric-caption-display:none;--frog-numeric-text-display:none;--frog-numeric-frame-fill:{};--frog-numeric-frame-stroke:{};--frog-numeric-frame-stroke-width:{};--frog-numeric-unit-display:{unit_display};--frog-numeric-radix-display:{radix_display};--frog-numeric-spinner-display:{spinner_display};--frog-numeric-value-face-fill:{};--frog-numeric-value-face-stroke:{};--frog-numeric-value-face-stroke-width:{};--frog-numeric-spinner-fill:{};--frog-numeric-spinner-stroke:{};'>{svg}</div>",
        escape_html(&frame_fill),
        escape_html(&frame_stroke),
        escape_html(&frame_stroke_width),
        escape_html(&value_face_fill),
        escape_html(&value_face_stroke),
        escape_html(&value_face_stroke_width),
        escape_html(&step_fill),
        escape_html(&step_symbol)
    )
}

fn numeric_step_button_state_style(widget: &WidgetState) -> String {
    let normal_fill = safe_css_color(&property_string(&widget.properties, "style.increment_button.fill_color.normal", "#5B9BD5"), "#5B9BD5");
    let pressed_fill = safe_css_color(&property_string(&widget.properties, "style.increment_button.fill_color.pressed", "#2B4F7B"), "#2B4F7B");
    let normal_border = safe_css_color(&property_string(&widget.properties, "style.increment_button.border_color.normal", "transparent"), "transparent");
    let pressed_border = safe_css_color(&property_string(&widget.properties, "style.increment_button.border_color.pressed", &normal_border), &normal_border);
    let normal_symbol = safe_css_color(&property_string(&widget.properties, "style.increment_button.symbol_color.normal", "#ffffff"), "#ffffff");
    let pressed_symbol = safe_css_color(&property_string(&widget.properties, "style.increment_button.symbol_color.pressed", &normal_symbol), &normal_symbol);
    format!(
        "--frog-numeric-step-fill:{};--frog-numeric-step-fill-pressed:{};--frog-numeric-step-border:{};--frog-numeric-step-border-pressed:{};--frog-numeric-step-symbol:{};--frog-numeric-step-symbol-pressed:{};",
        escape_html(&normal_fill),
        escape_html(&pressed_fill),
        escape_html(&normal_border),
        escape_html(&pressed_border),
        escape_html(&normal_symbol),
        escape_html(&pressed_symbol)
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
    let label = property_string(
        &widget.properties,
        "caption.text",
        &property_string(&widget.properties, "label", &widget.widget_id),
    );
    let value_face_color = safe_css_color(&property_string(&widget.properties, "foreground_color", "#ffffff"), "#ffffff");
    let label_color = safe_css_color(&property_string(&widget.properties, "label_color", "#111827"), "#111827");
    let label_weight = safe_css_font_weight(&property_string(&widget.properties, "style.caption.font_weight", "400"), "400");
    let route = widget.asset_id.as_ref().map(|id| format!("/asset/{id}")).unwrap_or_default();
    let minimum = property_u16(&widget.properties, "data_entry.minimum", 0);
    let maximum = property_u16(&widget.properties, "data_entry.maximum", 65535);
    let step = property_step(&widget.properties, "data_entry.increment_step", 1);

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

    html.push_str(&render_numeric_skin(widget, is_control, &value_face_color));

    let label_style = caption_anchor_style(&widget.properties, geometry);
    let value_style = svg_box_style(
        geometry.value_face_x,
        geometry.value_face_y,
        geometry.value_face_width,
        geometry.value_face_height,
        geometry,
    );
    let _ = write!(
        html,
        "<span class='numeric-label-overlay' data-frog-part='caption' data-svg-anchor='caption.anchor' style='{}color:{};font-weight:{};'>{}</span>",
        label_style,
        escape_html(&label_color),
        escape_html(&label_weight),
        escape_html(&label)
    );

    if is_control {
        let _ = write!(
            html,
            "<input id='{}_value' name='input_value' type='number' min='{}' max='{}' step='{}' class='numeric-value-overlay numeric-control-editor' data-frog-part='text_value' data-svg-anchor='text_value.center' style='{}color:#111827;' value='{}'{} />",
            escape_html(&widget.widget_id),
            minimum,
            maximum,
            step,
            value_style,
            value,
            if property_bool(&widget.properties, "enabled", true) { "" } else { " disabled" }
        );
        if property_bool(&widget.properties, "display.increment_buttons_visible", true) {
            let step_state_style = numeric_step_button_state_style(widget);
            let _ = write!(
                html,
                "<button type='button' class='numeric-step-overlay numeric-increment' data-target='{}_value' data-step='{}' data-frog-part='increment_up' data-frog-method='increment' data-frog-button-state-law='normal-pressed' aria-label='Increment {}' style='{}{}'></button>",
                escape_html(&widget.widget_id),
                step,
                escape_html(&label),
                svg_box_style(geometry.increment_up_x, geometry.increment_up_y, geometry.increment_up_width, geometry.increment_up_height, geometry),
                step_state_style
            );
            let _ = write!(
                html,
                "<button type='button' class='numeric-step-overlay numeric-decrement' data-target='{}_value' data-step='-{}' data-frog-part='increment_down' data-frog-method='decrement' data-frog-button-state-law='normal-pressed' aria-label='Decrement {}' style='{}{}'></button>",
                escape_html(&widget.widget_id),
                step,
                escape_html(&label),
                svg_box_style(geometry.increment_down_x, geometry.increment_down_y, geometry.increment_down_width, geometry.increment_down_height, geometry),
                step_state_style
            );
        }
    } else {
        let _ = write!(
            html,
            "<output class='numeric-value-overlay numeric-indicator-value' data-frog-part='text_value' data-svg-anchor='text_value.center' style='{}color:#111827;'>{}</output>",
            value_style,
            value
        );
    }

    html.push_str("</section>");
    html
}

pub struct BooleanBrowserUiRuntime {
    pub contract: Value,
    pub wfrog: Value,
    pub panel: Value,
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
        let contract: Value = serde_json::from_str(&fs::read_to_string(&contract_path)?)?;
        let wfrog: Value = serde_json::from_str(&fs::read_to_string(&wfrog_path)?)?;
        let panel = source_front_panel_value(&contract_path, &contract)?;
        let mut asset_map = BTreeMap::new();
        if let Some(assets) = wfrog["svg_assets"].as_array() {
            for asset in assets {
                if let (Some(asset_id), Some(path)) = (asset["asset_id"].as_str(), asset["path"].as_str()) {
                    asset_map.insert(asset_id.to_string(), wfrog_path.parent().unwrap_or_else(|| std::path::Path::new("")).join(path));
                }
            }
        }
        let current_value = panel["widgets"]
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
            panel,
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
             .boolean-caption-overlay{{position:absolute;left:0;top:0;transform:translateY(-50%);text-align:left;font-size:14px;font-weight:600;line-height:1;color:#1f2933;white-space:nowrap;pointer-events:none;z-index:3;}}\
             .boolean-state-face{{position:absolute;left:var(--boolean-inner-left);top:var(--boolean-inner-top);width:var(--boolean-inner-width);height:var(--boolean-inner-height);border:2px solid var(--boolean-inner-border);border-radius:7px;background:var(--boolean-fill);box-shadow:inset 0 1px 0 rgba(255,255,255,.6),0 1px 2px rgba(15,23,42,.16);transition:background var(--boolean-transition),border-color var(--boolean-transition),box-shadow var(--boolean-transition),transform var(--boolean-transition);z-index:1;}}\
             .boolean-widget[data-realization-variant='circular'] .boolean-state-face{{border-radius:50%;}}\
             .boolean-widget[data-frog-frame-visible='false'] .boolean-state-face{{box-shadow:none;}}\
             .boolean-control:hover .boolean-state-face{{background:var(--boolean-hover-fill);border-color:var(--boolean-hover-inner-border);box-shadow:inset 0 1px 0 rgba(255,255,255,.72),0 2px 5px rgba(15,23,42,.18);}}\
             .boolean-control[data-frog-frame-visible='false']:hover .boolean-state-face{{box-shadow:none;}}\
             .boolean-control:active .boolean-state-face{{background:var(--boolean-pressed-fill);border-color:var(--boolean-pressed-inner-border);box-shadow:inset 0 2px 4px rgba(15,23,42,.22);transform:translateY(var(--boolean-pressed-inset));}}\
             .boolean-control[data-frog-frame-visible='false']:active .boolean-state-face{{box-shadow:none;}}\
             .boolean-control:focus-visible .boolean-state-face{{outline:var(--boolean-focus-width) solid var(--boolean-focus-color);}}\
             .boolean-state-overlay{{position:absolute;left:0;right:0;top:49px;transform:translateY(-50%);text-align:center;font-size:18px;font-weight:700;line-height:1;color:var(--boolean-text);pointer-events:none;z-index:4;}}\
             .actions{{margin-top:16px;display:flex;gap:12px;align-items:center;}}\
             .state-link{{font-size:16px;}}\
             .diagnostic{{margin:12px 0;padding:10px 12px;border-radius:6px;}}\
             .diagnostic.error{{background:#fff1f2;color:#9f1239;border:1px solid #fecdd3;}}\
             </style></head><body>\
             <h1>{title}</h1>\
             <p class='meta'>Example 06 - .frog front panel + Default Boolean .wfrog realization assets + Rust runtime</p>\
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

pub struct StringBrowserUiRuntime {
    pub contract: Value,
    pub wfrog: Value,
    pub panel: Value,
    pub asset_map: BTreeMap<String, PathBuf>,
    pub current_text: String,
    pub last_result: String,
    pub last_error: Option<String>,
    pub native_kernel_bridge: Option<NativeStringKernelBridge>,
}

impl StringBrowserUiRuntime {
    pub fn with_native_kernel_bridge(
        contract_path: PathBuf,
        wfrog_path: PathBuf,
        native_kernel_bridge: Option<NativeStringKernelBridge>,
    ) -> Result<Self> {
        let contract: Value = serde_json::from_str(&fs::read_to_string(&contract_path)?)?;
        let wfrog: Value = serde_json::from_str(&fs::read_to_string(&wfrog_path)?)?;
        let panel = source_front_panel_value(&contract_path, &contract)?;
        let mut asset_map = BTreeMap::new();
        if let Some(assets) = wfrog["svg_assets"].as_array() {
            for asset in assets {
                if let (Some(asset_id), Some(path)) = (asset["asset_id"].as_str(), asset["path"].as_str()) {
                    asset_map.insert(asset_id.to_string(), wfrog_path.parent().unwrap_or_else(|| std::path::Path::new("")).join(path));
                }
            }
        }
        let current_text = panel["widgets"]
            .as_array()
            .and_then(|widgets| {
                widgets.iter().find_map(|widget| {
                    if widget["instance_id"].as_str() == Some("str_input") {
                        widget["props"]["value"].as_str().map(ToString::to_string)
                    } else {
                        None
                    }
                })
            })
            .unwrap_or_else(|| "hello world".to_string());

        Ok(Self {
            contract,
            wfrog,
            panel,
            asset_map,
            last_result: current_text.clone(),
            current_text,
            last_error: None,
            native_kernel_bridge,
        })
    }

    pub fn run_once(&mut self, input_text: String) -> Result<Value> {
        self.current_text = input_text;
        if let Some(bridge) = &self.native_kernel_bridge {
            if bridge.manifest().source_lowered_unit != "Examples/07_string_value_roundtrip/main.lowering.json" {
                return Err(RuntimeError::Message("Unexpected native string kernel source lowered unit.".to_string()));
            }
            let result = bridge.run(&self.current_text);
            if !result.ok {
                self.last_error = Some(bridge.manifest().diagnostic(result.error_code));
                return Err(RuntimeError::Message(bridge.manifest().diagnostic(result.error_code)));
            }
            self.last_result = result.result;
        } else {
            if self.current_text.as_bytes().len() > 256 {
                return Err(RuntimeError::Message("input_text must remain within 256 UTF-8 bytes.".to_string()));
            }
            self.last_result = self.current_text.clone();
        }
        self.last_error = None;
        Ok(self.execution_artifact())
    }

    pub fn execution_artifact(&self) -> Value {
        let panel = &self.panel;
        let widgets = panel["widgets"].as_array().cloned().unwrap_or_default();
        let widget_by_id = |id: &str| widgets.iter().find(|widget| widget["instance_id"].as_str() == Some(id)).cloned().unwrap_or(Value::Null);
        let make_runtime = |widget: &Value, value: &str| {
            let props = &widget["props"];
            let visual = &widget["visual"];
            let mut runtime = json!({
                "value": value,
                "label.text": props["label.text"].clone(),
                "caption.text": props["caption.text"].clone(),
                "asset_ref": visual["asset_ref"].clone(),
                "realization.variant": props["realization.variant"].clone()
            });
            if let Some(runtime_object) = runtime.as_object_mut() {
                for member in [
                    "caption.visible",
                    "caption.anchor.x",
                    "caption.anchor.y",
                    "caption.align.horizontal",
                    "caption.style.text_color",
                    "style.frame.fill_color",
                    "style.frame.border_color",
                    "style.frame.border_width",
                    "style.text_region.fill_color",
                    "style.text_region.fill_color.hover",
                    "style.text_region.border_color",
                    "style.text_region.border_color.hover",
                    "style.text_region.border_width",
                    "style.text_region.border_width.hover",
                    "style.text.color",
                    "style.text.font_size",
                    "style.text.font_weight",
                    "placeholder.text",
                    "placeholder.visible",
                    "interaction.enabled",
                    "interaction.read_only",
                ] {
                    if !props[member].is_null() {
                        runtime_object.insert(member.to_string(), props[member].clone());
                    }
                }
            }
            runtime
        };
        let input = widget_by_id("str_input");
        let result = widget_by_id("str_result");
        json!({
            "artifact_kind": "frog_runtime_execution_result",
            "artifact_governance_ref": {"path": "Versioning/Readme.md"},
            "status": "ok",
            "contract_ref": {"unit_ids": ["main"], "backend_family": self.contract["backend_family"].clone(), "source_ref": self.contract["source_ref"].clone()},
            "execution_summary": {"mode": "string_value_roundtrip", "executed_unit": "main", "operation": "copy", "input_text": self.current_text, "result_text": self.last_result},
            "outputs": {"public": {"result_text": self.last_result}, "ui": {"str_input": self.current_text, "str_result": self.last_result}},
            "ui_runtime": {
                "panel": {"panel_id": panel["panel_id"].clone(), "title": panel["title"].clone(), "class_ref": panel["class_ref"].clone(), "layout": panel["layout"].clone()},
                "widgets": [
                    {"widget_id": "str_input", "class_ref": input["class_ref"].clone(), "role": "control", "layout": input["layout"].clone(), "runtime": make_runtime(&input, &self.current_text)},
                    {"widget_id": "str_result", "class_ref": result["class_ref"].clone(), "role": "indicator", "layout": result["layout"].clone(), "runtime": make_runtime(&result, &self.last_result)}
                ]
            },
            "diagnostics": []
        })
    }

    pub fn render_html(&self) -> String {
        let snapshot = self.execution_artifact();
        let panel = &snapshot["ui_runtime"]["panel"];
        let widgets = snapshot["ui_runtime"]["widgets"].as_array().unwrap();
        let panel_width = panel["layout"]["width"].as_i64().unwrap_or(560);
        let panel_height = panel["layout"]["height"].as_i64().unwrap_or(170);
        let uses_native_kernel = self.native_kernel_bridge.is_some();
        let mut diagnostics = String::new();
        if let Some(message) = &self.last_error {
            let _ = write!(diagnostics, "<div class='diagnostic error'>{}</div>", escape_html(message));
        }
        let rendered_widgets = widgets
            .iter()
            .map(|widget| {
                let asset_id = widget["runtime"]["asset_ref"]
                    .as_str()
                    .and_then(|value| value.strip_prefix("asset:"))
                    .unwrap_or("");
                render_string_widget(widget, self.asset_map.get(asset_id))
            })
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
             .string-widget{{font-family:Segoe UI,Arial,sans-serif;}}\
             .string-skin{{position:absolute;inset:0;width:100%;height:100%;display:block;}}\
             .string-skin svg{{width:100%;height:100%;display:block;--frog-string-label-display:inherit;--frog-string-caption-display:inherit;--frog-string-placeholder-display:inherit;--frog-string-frame-fill:inherit;--frog-string-frame-stroke:inherit;--frog-string-frame-stroke-width:inherit;--frog-string-text-region-fill:inherit;--frog-string-text-region-stroke:inherit;--frog-string-text-region-stroke-width:inherit;--frog-string-text-fill:inherit;--frog-string-text-font-size:inherit;--frog-string-text-font-weight:inherit;}}\
             .string-skin #label_text,.string-skin #caption_text,.string-skin #placeholder,.string-skin #text_value{{display:none;}}\
             .string-control:hover .string-skin svg{{--frog-string-text-region-fill:var(--frog-string-text-region-fill-hover);--frog-string-text-region-stroke:var(--frog-string-text-region-stroke-hover);--frog-string-text-region-stroke-width:var(--frog-string-text-region-stroke-width-hover);}}\
             .string-caption-overlay{{position:absolute;transform:translateY(-50%);font-size:14px;font-weight:600;line-height:1;white-space:nowrap;pointer-events:none;}}\
             .string-value-overlay{{position:absolute;box-sizing:border-box;font-family:Segoe UI,Arial,sans-serif;line-height:1.2;border:0;background:transparent;}}\
             .string-control-editor{{padding:0 8px;outline:0;}}\
             .string-control-editor:focus{{outline:0;}}\
             .string-indicator-value{{display:flex;align-items:center;padding:0 8px;pointer-events:none;}}\
             .actions{{margin-top:16px;display:flex;gap:12px;align-items:center;}}\
             button{{padding:8px 14px;border:0;border-radius:6px;cursor:pointer;background:#0f62fe;color:#ffffff;font-weight:600;}}\
             .diagnostic{{margin:12px 0;padding:10px 12px;border-radius:6px;}}\
             .diagnostic.error{{background:#fff1f2;color:#9f1239;border:1px solid #fecdd3;}}\
             </style></head><body>\
             <h1>{title}</h1>\
             <p class='meta'>Example 07 - .frog front panel + Default String .wfrog realization assets + Rust runtime</p>\
             <dl class='runtime-facts' aria-label='Runtime facts'>\
             <div><dt>Runtime</dt><dd>Rust reference runtime</dd></div>\
             <div><dt>Execution</dt><dd>{execution_path}</dd></div>\
             <div><dt>Compiler backend</dt><dd>{compiler_backend}</dd></div>\
             </dl>{diagnostics}\
             <form method='post' action='/run'>\
             <div class='front-panel' data-panel-id='{panel_id}' data-coordinate-space='panel_pixels' data-runtime-language='rust' data-compiler-backend='{compiler_backend_id}' data-execution-path='{execution_path_id}' style='width:{panel_width}px;height:{panel_height}px;'>\
             {rendered_widgets}</div>\
             <div class='actions'><button type='submit'>Run Example 07</button><a class='state-link' href='/state.json'>state.json</a></div></form>\
             </body></html>",
            title = escape_html(panel["title"].as_str().unwrap_or("FROG")),
            diagnostics = diagnostics,
            execution_path = if uses_native_kernel { "native kernel bridge" } else { "string contract executor" },
            compiler_backend = if uses_native_kernel { "LLVM native string kernel artifact" } else { "none for Example 07" },
            compiler_backend_id = if uses_native_kernel { "llvm" } else { "none" },
            execution_path_id = if uses_native_kernel { "native_kernel_bridge" } else { "rust_string_contract_executor" },
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
                let _ = write_response(&mut stream, "500 Internal Server Error", "text/plain; charset=utf-8", format!("{error}").into_bytes(), None);
            }
        }
        Ok(())
    }

    fn handle_connection(&mut self, stream: &mut TcpStream) -> Result<()> {
        let request = read_request(stream)?;
        if request.method == "GET" && request.path == "/" {
            return write_response(stream, "200 OK", "text/html; charset=utf-8", self.render_html().into_bytes(), None);
        }
        if request.method == "GET" && request.path == "/state.json" {
            let payload = to_string_pretty(&self.execution_artifact()).unwrap().into_bytes();
            return write_response(stream, "200 OK", "application/json; charset=utf-8", payload, None);
        }
        if request.method == "GET" && request.path.starts_with("/asset/") {
            let asset_id = request.path.trim_start_matches("/asset/");
            if let Some(path) = self.asset_map.get(asset_id) {
                if path.exists() {
                    return write_response(stream, "200 OK", "image/svg+xml", fs::read(path)?, None);
                }
            }
            return write_response(stream, "404 Not Found", "text/plain; charset=utf-8", b"missing asset".to_vec(), None);
        }
        if request.method == "POST" && request.path == "/run" {
            let body = String::from_utf8_lossy(&request.body);
            let value = parse_form_value(&body, "input_text").unwrap_or_else(|| "hello world".to_string());
            if let Err(error) = self.run_once(value) {
                self.last_error = Some(error.to_string());
            }
            return write_response(stream, "303 See Other", "text/plain; charset=utf-8", Vec::new(), Some(("Location", "/".to_string())));
        }
        write_response(stream, "404 Not Found", "text/plain; charset=utf-8", b"not found".to_vec(), None)
    }
}

pub struct PathBrowserUiRuntime {
    pub contract: Value,
    pub wfrog: Value,
    pub panel: Value,
    pub asset_map: BTreeMap<String, PathBuf>,
    pub values: BTreeMap<String, String>,
    pub last_result: String,
    pub last_error: Option<String>,
    pub native_kernel_bridge: Option<NativeStringKernelBridge>,
}

impl PathBrowserUiRuntime {
    pub fn with_native_kernel_bridge(
        contract_path: PathBuf,
        wfrog_path: PathBuf,
        native_kernel_bridge: Option<NativeStringKernelBridge>,
    ) -> Result<Self> {
        let contract: Value = serde_json::from_str(&fs::read_to_string(&contract_path)?)?;
        if contract["source_ref"]["example_id"].as_str() != Some("09_path_value_roundtrip") {
            return Err(RuntimeError::Message("PathBrowserUiRuntime expects Example 09.".to_string()));
        }
        if contract["units"][0]["kind"].as_str() != Some("path_value_roundtrip_ui_unit") {
            return Err(RuntimeError::Message("PathBrowserUiRuntime expects path_value_roundtrip_ui_unit.".to_string()));
        }
        let wfrog: Value = serde_json::from_str(&fs::read_to_string(&wfrog_path)?)?;
        let panel = source_front_panel_value(&contract_path, &contract)?;
        let mut asset_map = BTreeMap::new();
        if let Some(assets) = wfrog["svg_assets"].as_array() {
            for asset in assets {
                if let (Some(asset_id), Some(path)) = (asset["asset_id"].as_str(), asset["path"].as_str()) {
                    asset_map.insert(asset_id.to_string(), wfrog_path.parent().unwrap_or_else(|| std::path::Path::new("")).join(path));
                }
            }
        }
        let mut values = BTreeMap::new();
        for widget in path_panel_widgets(&panel) {
            let widget_id = widget["instance_id"].as_str().unwrap_or("").to_string();
            if widget_id.is_empty() {
                continue;
            }
            let asset_ref = widget["visual"]["asset_ref"].as_str().unwrap_or("");
            let asset_id = asset_ref.strip_prefix("asset:").unwrap_or("");
            if asset_id.is_empty() || !asset_map.get(asset_id).is_some_and(|path| path.exists()) {
                return Err(RuntimeError::Message(format!("Path widget {widget_id} asset path must exist.")));
            }
            values.insert(widget_id, widget["props"]["value"].as_str().unwrap_or("").to_string());
        }
        if !values.contains_key("path_input") || !values.contains_key("path_result") {
            return Err(RuntimeError::Message("Example 09 panel must contain path_input and path_result.".to_string()));
        }
        let mut runtime = Self {
            contract,
            wfrog,
            panel,
            asset_map,
            values,
            last_result: String::new(),
            last_error: None,
            native_kernel_bridge,
        };
        runtime.run_all(BTreeMap::new())?;
        Ok(runtime)
    }

    pub fn run_once(&mut self, input_path: String) -> Result<Value> {
        let mut values = BTreeMap::new();
        values.insert("input_path".to_string(), input_path);
        self.run_all(values)
    }

    pub fn run_all(&mut self, control_values: BTreeMap<String, String>) -> Result<Value> {
        for (input_id, value) in control_values {
            self.set_input_value(&input_id, &value)?;
        }
        for (input_id, output_id) in self.path_execution_pairs() {
            let input = self.input_value(&input_id);
            let result = if let Some(bridge) = &self.native_kernel_bridge {
                if bridge.manifest().source_lowered_unit != "Examples/09_path_value_roundtrip/main.lowering.json" {
                    return Err(RuntimeError::Message("Unexpected native path kernel source lowered unit.".to_string()));
                }
                let result = bridge.run(&input);
                if !result.ok {
                    let diagnostic = bridge.manifest().diagnostic(result.error_code);
                    self.last_error = Some(diagnostic.clone());
                    return Err(RuntimeError::Message(diagnostic));
                }
                result.result
            } else {
                if input.as_bytes().len() > 256 {
                    return Err(RuntimeError::Message(format!("{input_id} must remain within 256 UTF-8 bytes.")));
                }
                input
            };
            self.publish_output(&output_id, &result);
            if output_id == "result_path" {
                self.last_result = result;
            }
        }
        self.last_error = None;
        Ok(self.execution_artifact())
    }

    fn set_input_value(&mut self, input_id: &str, value: &str) -> Result<()> {
        if value.as_bytes().len() > 256 {
            return Err(RuntimeError::Message(format!("{input_id} must remain within 256 UTF-8 bytes.")));
        }
        for widget in path_panel_widgets(&self.panel) {
            let widget_id = widget["instance_id"].as_str().unwrap_or("");
            if path_input_id(&self.contract, &widget) == input_id {
                self.values.insert(widget_id.to_string(), value.to_string());
            }
        }
        Ok(())
    }

    fn input_value(&self, input_id: &str) -> String {
        for widget in path_panel_widgets(&self.panel) {
            let widget_id = widget["instance_id"].as_str().unwrap_or("");
            if path_input_id(&self.contract, &widget) == input_id {
                return self.values.get(widget_id).cloned().unwrap_or_default();
            }
        }
        String::new()
    }

    fn publish_output(&mut self, output_id: &str, value: &str) {
        for widget in path_panel_widgets(&self.panel) {
            let widget_id = widget["instance_id"].as_str().unwrap_or("");
            if path_output_id(&self.contract, &widget) == output_id {
                self.values.insert(widget_id.to_string(), value.to_string());
            }
        }
    }

    fn path_execution_pairs(&self) -> Vec<(String, String)> {
        let mut pairs = Vec::new();
        for widget in path_panel_widgets(&self.panel) {
            if widget["class_ref"].as_str() != Some("frog.widgets.path_control") {
                continue;
            }
            let input_id = path_input_id(&self.contract, &widget);
            if input_id.is_empty() {
                continue;
            }
            let output_id = widget["props"]["binding.output_id"]
                .as_str()
                .map(ToString::to_string)
                .unwrap_or_else(|| if input_id == "input_path" { "result_path".to_string() } else { String::new() });
            if !output_id.is_empty() {
                pairs.push((input_id, output_id));
            }
        }
        pairs
    }

    fn runtime_for(&self, widget: &Value) -> Value {
        let props = &widget["props"];
        let visual = &widget["visual"];
        let widget_id = widget["instance_id"].as_str().unwrap_or("");
        let mut runtime = json!({
            "value": self.values.get(widget_id).cloned().unwrap_or_default(),
            "path.display_value": self.values.get(widget_id).cloned().unwrap_or_default(),
            "path.kind": props["path.kind"].clone(),
            "path.validation_state": props["path.validation_state"].clone(),
            "label.text": props["label.text"].clone(),
            "caption.text": props["caption.text"].clone(),
            "asset_ref": visual["asset_ref"].clone(),
            "realization.variant": props["realization.variant"].clone(),
        });
        if let Some(runtime_object) = runtime.as_object_mut() {
            let input_id = path_input_id(&self.contract, widget);
            let output_id = path_output_id(&self.contract, widget);
            if !input_id.is_empty() && widget["class_ref"].as_str() == Some("frog.widgets.path_control") {
                runtime_object.insert("binding.public_input_id".to_string(), Value::String(input_id));
            }
            if !output_id.is_empty() && widget["class_ref"].as_str() == Some("frog.widgets.path_indicator") {
                runtime_object.insert("binding.public_output_id".to_string(), Value::String(output_id));
            }
            for member in [
                "caption.visible",
                "caption.anchor.x",
                "caption.anchor.y",
                "caption.align.horizontal",
                "caption.style.text_color",
                "display.icon_visible",
                "display.validation_marker_visible",
                "display.text_overflow_visible",
                "browse.enabled",
                "browse.button_visible",
                "style.frame.fill_color",
                "style.frame.border_color",
                "style.frame.border_width",
                "style.path_face.fill_color",
                "style.path_face.fill_color.hover",
                "style.path_face.border_color",
                "style.path_face.border_color.hover",
                "style.path_face.border_width",
                "style.path_display.color",
                "style.path_display.font_size",
                "style.path_display.font_weight",
                "style.path_display.padding_inline",
                "style.path_display.baseline_offset",
                "style.path_display.line_height",
                "style.path_icon.fill_color",
                "style.path_icon.front_fill_color",
                "style.path_icon.stroke_color",
                "style.path_icon.highlight_color",
                "style.browse_button.fill_color",
                "style.browse_button.fill_color.hover",
                "style.browse_button.border_color",
                "style.browse_button.border_color.hover",
                "style.browse_button.border_width",
                "style.browse_button.text_color",
                "style.browse_button.text_font_size",
                "binding.preview_input_id",
                "binding.preview_output_id",
                "binding.input_id",
                "binding.output_id",
                "interaction.enabled",
                "interaction.read_only",
            ] {
                if !props[member].is_null() {
                    runtime_object.insert(member.to_string(), props[member].clone());
                }
            }
        }
        runtime
    }

    pub fn execution_artifact(&self) -> Value {
        let mut widgets = Vec::new();
        let mut ui_outputs = Map::new();
        for widget in path_panel_widgets(&self.panel) {
            let widget_id = widget["instance_id"].as_str().unwrap_or("");
            let role = if widget["class_ref"].as_str() == Some("frog.widgets.path_control") { "control" } else { "indicator" };
            ui_outputs.insert(widget_id.to_string(), Value::String(self.values.get(widget_id).cloned().unwrap_or_default()));
            widgets.push(json!({
                "widget_id": widget_id,
                "class_ref": widget["class_ref"].clone(),
                "role": role,
                "layout": widget["layout"].clone(),
                "runtime": self.runtime_for(&widget)
            }));
        }
        json!({
            "artifact_kind": "frog_runtime_execution_result",
            "artifact_governance_ref": {"path": "Versioning/Readme.md"},
            "status": "ok",
            "contract_ref": {"unit_ids": ["main"], "backend_family": self.contract["backend_family"].clone(), "source_ref": self.contract["source_ref"].clone()},
            "execution_summary": {"mode": "path_value_roundtrip", "executed_unit": "main", "operation": "copy", "input_path": self.input_value("input_path"), "result_path": self.last_result},
            "outputs": {"public": {"result_path": self.last_result}, "ui": Value::Object(ui_outputs)},
            "ui_runtime": {
                "panel": {"panel_id": self.panel["panel_id"].clone(), "title": self.panel["title"].clone(), "class_ref": self.panel["class_ref"].clone(), "layout": self.panel["layout"].clone()},
                "widgets": widgets
            },
            "diagnostics": []
        })
    }

    pub fn render_html(&self) -> String {
        let snapshot = self.execution_artifact();
        let panel = &snapshot["ui_runtime"]["panel"];
        let widgets = snapshot["ui_runtime"]["widgets"].as_array().unwrap();
        let panel_width = panel["layout"]["width"].as_i64().unwrap_or(700);
        let panel_height = panel["layout"]["height"].as_i64().unwrap_or(300);
        let uses_native_kernel = self.native_kernel_bridge.is_some();
        let mut diagnostics = String::new();
        if let Some(message) = &self.last_error {
            let _ = write!(diagnostics, "<div class='diagnostic error'>{}</div>", escape_html(message));
        }
        let rendered_widgets = widgets
            .iter()
            .map(|widget| {
                let asset_id = widget["runtime"]["asset_ref"]
                    .as_str()
                    .and_then(|value| value.strip_prefix("asset:"))
                    .unwrap_or("");
                render_path_widget(widget, self.asset_map.get(asset_id))
            })
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
             .path-widget{{font-family:Segoe UI,Arial,sans-serif;}}\
             .path-skin{{position:absolute;inset:0;width:100%;height:100%;display:block;}}\
             .path-skin svg{{width:100%;height:100%;display:block;}}\
             .path-skin #label_text,.path-skin #caption_text,.path-skin #path_display{{display:none;}}\
             .path-caption-overlay{{position:absolute;transform:translateY(-50%);font-size:14px;font-weight:600;line-height:1;white-space:nowrap;pointer-events:none;}}\
             .path-value-overlay{{position:absolute;box-sizing:border-box;font-family:Segoe UI,Arial,sans-serif;line-height:1;border:0;background:transparent;margin:0;}}\
             .path-control-editor{{outline:0;appearance:none;-webkit-appearance:none;}}\
             .path-control-editor:focus{{outline:0;}}\
             .path-indicator-value{{display:flex;align-items:center;pointer-events:none;overflow:hidden;white-space:nowrap;text-overflow:ellipsis;}}\
             .path-file-picker{{position:absolute;width:1px;height:1px;opacity:0;pointer-events:none;}}\
             .path-browse-overlay{{position:absolute;box-sizing:border-box;cursor:pointer;background:transparent;border:0;}}\
             .path-control:hover .path-skin #path_face{{fill:var(--frog-path-face-fill-hover) !important;stroke:var(--frog-path-face-stroke-hover) !important;}}\
             .path-control:hover .path-skin #browse_button{{fill:var(--frog-path-button-fill-hover) !important;}}\
             .actions{{margin-top:16px;display:flex;gap:12px;align-items:center;}}\
             button{{padding:8px 14px;border:0;border-radius:6px;cursor:pointer;background:#0f62fe;color:#ffffff;font-weight:600;}}\
             .diagnostic{{margin:12px 0;padding:10px 12px;border-radius:6px;}}\
             .diagnostic.error{{background:#fff1f2;color:#9f1239;border:1px solid #fecdd3;}}\
             </style><script>\
             function frogPathPicked(input,targetId){{const target=document.getElementById(targetId);if(!target){{return;}}if(input.files&&input.files.length>0){{target.value=input.files[0].name;target.dispatchEvent(new Event('input',{{bubbles:true}}));target.dispatchEvent(new Event('change',{{bubbles:true}}));}}}}\
             </script></head><body>\
             <h1>{title}</h1>\
             <p class='meta'>Example 09 - .frog front panel + Default Path .wfrog realization assets + Rust runtime</p>\
             <dl class='runtime-facts' aria-label='Runtime facts'>\
             <div><dt>Runtime</dt><dd>Rust reference runtime</dd></div>\
             <div><dt>Execution</dt><dd>{execution_path}</dd></div>\
             <div><dt>Compiler backend</dt><dd>{compiler_backend}</dd></div>\
             </dl>{diagnostics}\
             <form method='post' action='/run'>\
             <div class='front-panel' data-panel-id='{panel_id}' data-coordinate-space='panel_pixels' data-runtime-language='rust' data-compiler-backend='{compiler_backend_id}' data-execution-path='{execution_path_id}' style='width:{panel_width}px;height:{panel_height}px;'>\
             {rendered_widgets}</div>\
             <div class='actions'><button type='submit'>Run Example 09</button><a class='state-link' href='/state.json'>state.json</a></div></form>\
             </body></html>",
            title = escape_html(panel["title"].as_str().unwrap_or("FROG")),
            diagnostics = diagnostics,
            execution_path = if uses_native_kernel { "native kernel bridge" } else { "path contract executor" },
            compiler_backend = if uses_native_kernel { "LLVM native path kernel artifact" } else { "none for Example 09" },
            compiler_backend_id = if uses_native_kernel { "llvm" } else { "none" },
            execution_path_id = if uses_native_kernel { "native_kernel_bridge" } else { "rust_path_contract_executor" },
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
                let _ = write_response(&mut stream, "500 Internal Server Error", "text/plain; charset=utf-8", format!("{error}").into_bytes(), None);
            }
        }
        Ok(())
    }

    fn handle_connection(&mut self, stream: &mut TcpStream) -> Result<()> {
        let request = read_request(stream)?;
        if request.method == "GET" && request.path == "/" {
            return write_response(stream, "200 OK", "text/html; charset=utf-8", self.render_html().into_bytes(), None);
        }
        if request.method == "GET" && request.path == "/state.json" {
            let payload = to_string_pretty(&self.execution_artifact()).unwrap().into_bytes();
            return write_response(stream, "200 OK", "application/json; charset=utf-8", payload, None);
        }
        if request.method == "GET" && request.path.starts_with("/asset/") {
            let asset_id = request.path.trim_start_matches("/asset/");
            if let Some(path) = self.asset_map.get(asset_id) {
                if path.exists() {
                    return write_response(stream, "200 OK", "image/svg+xml", fs::read(path)?, None);
                }
            }
            return write_response(stream, "404 Not Found", "text/plain; charset=utf-8", b"missing asset".to_vec(), None);
        }
        if request.method == "POST" && request.path == "/run" {
            let body = String::from_utf8_lossy(&request.body);
            let values = parse_path_form_values(&body);
            if let Err(error) = self.run_all(values) {
                self.last_error = Some(error.to_string());
            }
            return write_response(stream, "303 See Other", "text/plain; charset=utf-8", Vec::new(), Some(("Location", "/".to_string())));
        }
        write_response(stream, "404 Not Found", "text/plain; charset=utf-8", b"not found".to_vec(), None)
    }
}

fn path_panel_widgets(panel: &Value) -> Vec<Value> {
    panel["widgets"]
        .as_array()
        .cloned()
        .unwrap_or_default()
        .into_iter()
        .filter(|widget| matches!(widget["class_ref"].as_str(), Some("frog.widgets.path_control" | "frog.widgets.path_indicator")))
        .collect()
}

fn path_contract_binding(contract: &Value, widget_id: &str, member: &str) -> Option<String> {
    contract["units"][0]["ui_bindings"]["widgets"]
        .as_array()
        .and_then(|widgets| {
            widgets.iter().find_map(|widget| {
                if widget["widget_id"].as_str() == Some(widget_id) {
                    widget["binding"][member].as_str().map(ToString::to_string)
                } else {
                    None
                }
            })
        })
}

fn path_input_id(contract: &Value, widget: &Value) -> String {
    let widget_id = widget["instance_id"].as_str().unwrap_or("");
    path_contract_binding(contract, widget_id, "public_input_id")
        .or_else(|| widget["props"]["binding.public_input_id"].as_str().map(ToString::to_string))
        .or_else(|| widget["props"]["binding.preview_input_id"].as_str().map(ToString::to_string))
        .unwrap_or_default()
}

fn path_output_id(contract: &Value, widget: &Value) -> String {
    let widget_id = widget["instance_id"].as_str().unwrap_or("");
    path_contract_binding(contract, widget_id, "public_output_id")
        .or_else(|| widget["props"]["binding.public_output_id"].as_str().map(ToString::to_string))
        .or_else(|| widget["props"]["binding.preview_output_id"].as_str().map(ToString::to_string))
        .unwrap_or_default()
}

fn parse_path_form_values(body: &str) -> BTreeMap<String, String> {
    let mut result = BTreeMap::new();
    for key in ["input_path", "input_path_no_icon"] {
        if let Some(value) = parse_form_value(body, key) {
            result.insert(key.to_string(), value);
        }
    }
    result
}

fn load_path_svg_geometry(asset_path: Option<&PathBuf>) -> NumericSvgGeometry {
    let mut geometry = NumericSvgGeometry {
        view_width: 520.0,
        view_height: 150.0,
        caption_x: 16.0,
        caption_y: 46.0,
        value_face_x: 22.0,
        value_face_y: 78.0,
        value_face_width: 390.0,
        value_face_height: 36.0,
        increment_up_x: 424.0,
        increment_up_y: 78.0,
        increment_up_width: 34.0,
        increment_up_height: 36.0,
        increment_down_x: 56.0,
        increment_down_y: 96.0,
        increment_down_width: 0.0,
        increment_down_height: 0.0,
    };
    let Some(path) = asset_path else { return geometry; };
    let Ok(svg) = fs::read_to_string(path) else { return geometry; };
    if let Some(start) = svg.find("viewBox=\"") {
        let value_start = start + "viewBox=\"".len();
        if let Some(value_end) = svg[value_start..].find('"') {
            let parts: Vec<&str> = svg[value_start..value_start + value_end].split_whitespace().collect();
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
    geometry.caption_x = svg_attribute_f64(&svg, "caption_text", "x", geometry.caption_x);
    geometry.caption_y = svg_attribute_f64(&svg, "caption_text", "y", geometry.caption_y);
    geometry.value_face_x = svg_attribute_f64(&svg, "path_face", "x", geometry.value_face_x);
    geometry.value_face_y = svg_attribute_f64(&svg, "path_face", "y", geometry.value_face_y);
    geometry.value_face_width = svg_attribute_f64(&svg, "path_face", "width", geometry.value_face_width);
    geometry.value_face_height = svg_attribute_f64(&svg, "path_face", "height", geometry.value_face_height);
    geometry.increment_down_x = svg_attribute_f64(&svg, "path_display", "x", geometry.increment_down_x);
    geometry.increment_down_y = svg_attribute_f64(&svg, "path_display", "y", geometry.increment_down_y);
    geometry.increment_up_x = svg_attribute_f64(&svg, "browse_button", "x", geometry.increment_up_x);
    geometry.increment_up_y = svg_attribute_f64(&svg, "browse_button", "y", geometry.increment_up_y);
    geometry.increment_up_width = svg_attribute_f64(&svg, "browse_button", "width", geometry.increment_up_width);
    geometry.increment_up_height = svg_attribute_f64(&svg, "browse_button", "height", geometry.increment_up_height);
    geometry
}

fn render_path_skin(asset_path: Option<&PathBuf>, runtime: &Value) -> String {
    let Some(path) = asset_path else {
        return "<div class='path-skin missing-skin'></div>".to_string();
    };
    let Ok(svg) = fs::read_to_string(path) else {
        return "<div class='path-skin missing-skin'></div>".to_string();
    };
    let face_fill = safe_css_color(&runtime_string(runtime, "style.path_face.fill_color", "#ffffff"), "#ffffff");
    let face_stroke = safe_css_color(&runtime_string(runtime, "style.path_face.border_color", "#64748b"), "#64748b");
    let button_fill = safe_css_color(&runtime_string(runtime, "style.browse_button.fill_color", "#f8fafc"), "#f8fafc");
    let button_stroke = safe_css_color(&runtime_string(runtime, "style.browse_button.border_color", "#64748b"), "#64748b");
    format!(
        "<div class='path-skin' aria-hidden='true' style='--frog-path-label-display:none;--frog-path-caption-display:none;--frog-path-frame-fill:{};--frog-path-frame-stroke:{};--frog-path-frame-stroke-width:{};--frog-path-face-fill:{};--frog-path-face-stroke:{};--frog-path-face-stroke-width:{};--frog-path-face-fill-hover:{};--frog-path-face-stroke-hover:{};--frog-path-text-fill:{};--frog-path-text-font-size:{};--frog-path-text-font-weight:{};--frog-path-button-fill:{};--frog-path-button-fill-hover:{};--frog-path-button-stroke:{};--frog-path-button-stroke-hover:{};--frog-path-button-stroke-width:{};--frog-path-button-text-fill:{};--frog-path-button-text-font-size:{};--frog-path-icon-display:{};--frog-path-icon-fill:{};--frog-path-icon-front-fill:{};--frog-path-icon-stroke:{};--frog-path-icon-highlight:{};--frog-path-browse-display:{};--frog-path-validation-display:{};--frog-path-overflow-display:{};--frog-path-focus-display:none;'>{}</div>",
        escape_html(&safe_css_color(&runtime_string(runtime, "style.frame.fill_color", "transparent"), "transparent")),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.frame.border_color", "transparent"), "transparent")),
        escape_html(&safe_css_length(&runtime_string(runtime, "style.frame.border_width", "0px"), "0px")),
        escape_html(&face_fill),
        escape_html(&face_stroke),
        escape_html(&safe_css_length(&runtime_string(runtime, "style.path_face.border_width", "2px"), "2px")),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.path_face.fill_color.hover", &face_fill), &face_fill)),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.path_face.border_color.hover", &face_stroke), &face_stroke)),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.path_display.color", "#111827"), "#111827")),
        escape_html(&safe_css_length(&runtime_string(runtime, "style.path_display.font_size", "15px"), "15px")),
        escape_html(&safe_css_font_weight(&runtime_string(runtime, "style.path_display.font_weight", "400"), "400")),
        escape_html(&button_fill),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.browse_button.fill_color.hover", &button_fill), &button_fill)),
        escape_html(&button_stroke),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.browse_button.border_color.hover", &button_stroke), &button_stroke)),
        escape_html(&safe_css_length(&runtime_string(runtime, "style.browse_button.border_width", "1px"), "1px")),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.browse_button.text_color", "#111827"), "#111827")),
        escape_html(&safe_css_length(&runtime_string(runtime, "style.browse_button.text_font_size", "13px"), "13px")),
        if runtime_bool(runtime, "display.icon_visible", true) { "inline" } else { "none" },
        escape_html(&safe_css_color(&runtime_string(runtime, "style.path_icon.fill_color", "#facc15"), "#facc15")),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.path_icon.front_fill_color", "#fde68a"), "#fde68a")),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.path_icon.stroke_color", "#b45309"), "#b45309")),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.path_icon.highlight_color", "#fff7cc"), "#fff7cc")),
        if runtime_bool(runtime, "browse.button_visible", false) { "inline" } else { "none" },
        if runtime_bool(runtime, "display.validation_marker_visible", false) { "inline" } else { "none" },
        if runtime_bool(runtime, "display.text_overflow_visible", false) { "inline" } else { "none" },
        svg
    )
}

fn render_path_widget(widget: &Value, asset_path: Option<&PathBuf>) -> String {
    let runtime = &widget["runtime"];
    let layout = &widget["layout"];
    let geometry = load_path_svg_geometry(asset_path);
    let is_control = widget["role"].as_str() == Some("control");
    let widget_id = widget["widget_id"].as_str().unwrap_or("path_input");
    let caption = runtime_string(runtime, "caption.text", widget_id);
    let value = runtime_string(runtime, "value", "");
    let asset_route = runtime["asset_ref"]
        .as_str()
        .and_then(|value| value.strip_prefix("asset:"))
        .map(|id| format!("/asset/{id}"))
        .unwrap_or_default();
    let text_x = if runtime_bool(runtime, "display.icon_visible", true) { geometry.increment_down_x.max(geometry.value_face_x) } else { geometry.value_face_x };
    let text_width = (geometry.value_face_width - (text_x - geometry.value_face_x)).max(0.0);
    let value_style = svg_box_style(text_x, geometry.value_face_y, text_width, geometry.value_face_height, geometry);
    let browse_style = svg_box_style(geometry.increment_up_x, geometry.increment_up_y, geometry.increment_up_width, geometry.increment_up_height, geometry);
    let input_id = runtime_string(
        runtime,
        "binding.public_input_id",
        &runtime_string(runtime, "binding.preview_input_id", &format!("{widget_id}_value")),
    );
    let text_line_height = runtime_string(
        runtime,
        "style.path_display.line_height",
        &format!("{}px", ((geometry.value_face_height / geometry.view_height) * layout["height"].as_f64().unwrap_or(120.0)).round()),
    );
    let text_baseline = safe_css_length(&runtime_string(runtime, "style.path_display.baseline_offset", "0px"), "0px");
    let mut body = String::new();
    body.push_str(&render_path_skin(asset_path, runtime));
    let _ = write!(
        body,
        "<span class='path-caption-overlay' data-frog-part='caption' data-svg-anchor='caption.anchor' style='{}color:{};'>{}</span>",
        runtime_caption_anchor_style(runtime, geometry),
        escape_html(&safe_css_color(&runtime_string(runtime, "caption.style.text_color", "#111827"), "#111827")),
        escape_html(&caption)
    );
    if is_control {
        let _ = write!(
            body,
            "<input id='{widget_id}_value' name='{input_id}' type='text' class='path-value-overlay path-control-editor' data-frog-part='path_display' data-svg-anchor='path_display.left_center' data-frog-input-id='{input_id}' style='{value_style}color:{};font-size:{};font-weight:{};padding:0 {};line-height:{};transform:translateY({});' value='{}'{}>",
            escape_html(&safe_css_color(&runtime_string(runtime, "style.path_display.color", "#111827"), "#111827")),
            escape_html(&safe_css_length(&runtime_string(runtime, "style.path_display.font_size", "15px"), "15px")),
            escape_html(&safe_css_font_weight(&runtime_string(runtime, "style.path_display.font_weight", "400"), "400")),
            escape_html(&safe_css_length(&runtime_string(runtime, "style.path_display.padding_inline", "8px"), "8px")),
            escape_html(&safe_css_length(&text_line_height, &text_line_height)),
            escape_html(&text_baseline),
            escape_html(&value),
            if runtime_bool(runtime, "interaction.enabled", true) { "" } else { " disabled" }
        );
        let _ = write!(
            body,
            "<input id='{widget_id}_file_picker' type='file' class='path-file-picker' tabindex='-1' aria-hidden='true' onchange=\"frogPathPicked(this,'{widget_id}_value')\">"
        );
        if runtime_bool(runtime, "browse.button_visible", is_control) {
            let _ = write!(
                body,
                "<label for='{widget_id}_file_picker' class='path-browse-overlay' data-frog-part='browse_button' aria-label='Browse {}' style='{browse_style}'></label>",
                escape_html(&caption)
            );
        }
    } else {
        let _ = write!(
            body,
            "<output class='path-value-overlay path-indicator-value' data-frog-part='path_display' data-svg-anchor='path_display.left_center' style='{value_style}color:{};font-size:{};font-weight:{};padding:0 {};line-height:{};transform:translateY({});'>{}</output>",
            escape_html(&safe_css_color(&runtime_string(runtime, "style.path_display.color", "#111827"), "#111827")),
            escape_html(&safe_css_length(&runtime_string(runtime, "style.path_display.font_size", "15px"), "15px")),
            escape_html(&safe_css_font_weight(&runtime_string(runtime, "style.path_display.font_weight", "400"), "400")),
            escape_html(&safe_css_length(&runtime_string(runtime, "style.path_display.padding_inline", "8px"), "8px")),
            escape_html(&safe_css_length(&text_line_height, &text_line_height)),
            escape_html(&text_baseline),
            escape_html(&value)
        );
    }
    format!(
        "<section class='frog-widget path-widget {}' data-widget-id='{}' data-class-ref='{}' data-role='{}' data-frog-visual-law='wfrog-realization-state-map' data-frog-browse-visible='{}' data-asset-route='{}' style='position:absolute;left:{}px;top:{}px;width:{}px;height:{}px;--frog-path-button-fill:{};--frog-path-button-fill-hover:{};'>{}</section>",
        if is_control { "path-control" } else { "path-indicator" },
        escape_html(widget_id),
        escape_html(widget["class_ref"].as_str().unwrap_or("")),
        escape_html(widget["role"].as_str().unwrap_or("")),
        if runtime_bool(runtime, "browse.button_visible", is_control) { "true" } else { "false" },
        escape_html(&asset_route),
        layout["x"].as_i64().unwrap_or(0),
        layout["y"].as_i64().unwrap_or(0),
        layout["width"].as_i64().unwrap_or(300),
        layout["height"].as_i64().unwrap_or(120),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.browse_button.fill_color", "#f8fafc"), "#f8fafc")),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.browse_button.fill_color.hover", "#e5eef9"), "#e5eef9")),
        body
    )
}

#[derive(Clone, Debug)]
struct EnumUiItem {
    id: String,
    text: String,
    numeric_value: u16,
    enabled: bool,
}

fn enum_items_from_widget(widget: &Value) -> Result<Vec<EnumUiItem>> {
    let widget_id = widget["instance_id"].as_str().or_else(|| widget["widget_id"].as_str()).unwrap_or("enum");
    let props = &widget["props"];
    let runtime = &widget["runtime"];
    let items = props["items"]
        .as_array()
        .or_else(|| runtime["items"].as_array())
        .ok_or_else(|| RuntimeError::Message(format!("Enum widget {widget_id} must define items in front-panel instance properties.")))?;
    if items.is_empty() {
        return Err(RuntimeError::Message(format!("Enum widget {widget_id} must define at least one item.")));
    }
    let mut result = Vec::new();
    let mut seen_ids = std::collections::BTreeSet::new();
    let mut seen_numbers = std::collections::BTreeSet::new();
    for item in items {
        let id = item["id"].as_str().unwrap_or("").to_string();
        let text = item["text"].as_str().unwrap_or("").to_string();
        let raw = item["numeric_value"].as_u64().ok_or_else(|| RuntimeError::Message("Enum item must publish numeric_value.".to_string()))?;
        if id.is_empty() || text.is_empty() || raw > 65535 {
            return Err(RuntimeError::Message("Enum item must publish id, text, and a u16 numeric_value.".to_string()));
        }
        if !seen_ids.insert(id.clone()) {
            return Err(RuntimeError::Message(format!("Duplicate enum item id: {id}")));
        }
        if !seen_numbers.insert(raw as u16) {
            return Err(RuntimeError::Message("Duplicate enum item numeric_value.".to_string()));
        }
        result.push(EnumUiItem {
            id,
            text,
            numeric_value: raw as u16,
            enabled: item["enabled"].as_bool().unwrap_or(true),
        });
    }
    Ok(result)
}

fn enum_item_by_id(items: &[EnumUiItem], id: &str, label: &str) -> Result<EnumUiItem> {
    items
        .iter()
        .find(|item| item.id == id)
        .cloned()
        .ok_or_else(|| RuntimeError::Message(format!("{label} must resolve to a declared enum item.")))
}

fn enum_item_by_numeric_value(items: &[EnumUiItem], numeric_value: u16, label: &str) -> Result<EnumUiItem> {
    items
        .iter()
        .find(|item| item.numeric_value == numeric_value)
        .cloned()
        .ok_or_else(|| RuntimeError::Message(format!("{label} must resolve to a declared enum item.")))
}

pub struct EnumBrowserUiRuntime {
    pub contract: Value,
    pub wfrog: Value,
    pub panel: Value,
    pub asset_map: BTreeMap<String, PathBuf>,
    pub current_mode: String,
    pub last_result: String,
    pub last_error: Option<String>,
    pub native_kernel_bridge: Option<NativeEnumKernelBridge>,
}

impl EnumBrowserUiRuntime {
    pub fn with_native_kernel_bridge(
        contract_path: PathBuf,
        wfrog_path: PathBuf,
        native_kernel_bridge: Option<NativeEnumKernelBridge>,
    ) -> Result<Self> {
        let contract: Value = serde_json::from_str(&fs::read_to_string(&contract_path)?)?;
        if contract["source_ref"]["example_id"].as_str() != Some("08_enum_value_roundtrip") {
            return Err(RuntimeError::Message("EnumBrowserUiRuntime expects Example 08.".to_string()));
        }
        let wfrog: Value = serde_json::from_str(&fs::read_to_string(&wfrog_path)?)?;
        let panel = source_front_panel_value(&contract_path, &contract)?;
        let mut asset_map = BTreeMap::new();
        if let Some(assets) = wfrog["svg_assets"].as_array() {
            for asset in assets {
                if let (Some(asset_id), Some(path)) = (asset["asset_id"].as_str(), asset["path"].as_str()) {
                    asset_map.insert(asset_id.to_string(), wfrog_path.parent().unwrap_or_else(|| std::path::Path::new("")).join(path));
                }
            }
        }
        let input_widget = panel["widgets"]
            .as_array()
            .and_then(|widgets| widgets.iter().find(|widget| widget["instance_id"].as_str() == Some("mode_input")))
            .ok_or_else(|| RuntimeError::Message("Example 08 panel must contain mode_input.".to_string()))?;
        let items = enum_items_from_widget(input_widget)?;
        let current_mode = input_widget["props"]["value"]
            .as_str()
            .unwrap_or(&items[0].id)
            .to_string();
        enum_item_by_id(&items, &current_mode, "mode_value")?;
        Ok(Self {
            contract,
            wfrog,
            panel,
            asset_map,
            last_result: current_mode.clone(),
            current_mode,
            last_error: None,
            native_kernel_bridge,
        })
    }

    pub fn run_once(&mut self, mode_value: String) -> Result<Value> {
        self.current_mode = mode_value;
        let input_widget = self.widget_by_id("mode_input")?;
        let items = enum_items_from_widget(&input_widget)?;
        let input_item = enum_item_by_id(&items, &self.current_mode, "mode_value")?;
        if !input_item.enabled {
            return Err(RuntimeError::Message("mode_value must resolve to an enabled enum item.".to_string()));
        }
        if let Some(bridge) = &self.native_kernel_bridge {
            if bridge.manifest().source_lowered_unit != "Examples/08_enum_value_roundtrip/main.lowering.json" {
                return Err(RuntimeError::Message("Unexpected native enum kernel source lowered unit.".to_string()));
            }
            let result = bridge.run(input_item.numeric_value);
            if !result.ok {
                let diagnostic = bridge.manifest().diagnostic(result.error_code);
                self.last_error = Some(diagnostic.clone());
                return Err(RuntimeError::Message(diagnostic));
            }
            let result_widget = self.widget_by_id("mode_result")?;
            let result_items = enum_items_from_widget(&result_widget)?;
            self.last_result = enum_item_by_numeric_value(&result_items, result.result_numeric_value, "result_mode")?.id;
        } else {
            self.last_result = input_item.id;
        }
        self.last_error = None;
        Ok(self.execution_artifact())
    }

    fn widget_by_id(&self, id: &str) -> Result<Value> {
        self.panel["widgets"]
            .as_array()
            .and_then(|widgets| widgets.iter().find(|widget| widget["instance_id"].as_str() == Some(id)))
            .cloned()
            .ok_or_else(|| RuntimeError::Message(format!("Example 08 panel must contain {id}.")))
    }

    fn runtime_for(&self, widget: &Value, value: &str) -> Value {
        let props = &widget["props"];
        let visual = &widget["visual"];
        let items = enum_items_from_widget(widget).unwrap_or_default();
        let selected = enum_item_by_id(&items, value, "enum.value").unwrap_or_else(|_| items[0].clone());
        let mut runtime = json!({
            "value": selected.id,
            "selected.text": selected.text,
            "selected.numeric_value": selected.numeric_value,
            "label.text": props["label.text"].clone(),
            "caption.text": props["caption.text"].clone(),
            "items": items.iter().map(|item| json!({"id": item.id, "text": item.text, "numeric_value": item.numeric_value, "enabled": item.enabled})).collect::<Vec<Value>>(),
            "asset_ref": visual["asset_ref"].clone(),
            "realization.variant": props["realization.variant"].clone()
        });
        if let Some(runtime_object) = runtime.as_object_mut() {
            for member in [
                "enum.domain_id",
                "caption.visible",
                "caption.anchor.x",
                "caption.anchor.y",
                "caption.align.horizontal",
                "caption.style.text_color",
                "display.digital_display_visible",
                "display.increment_buttons_visible",
                "display.selector_visible",
                "display.text_overflow_visible",
                "style.scale.reference_width",
                "style.scale.reference_height",
                "style.frame.fill_color",
                "style.frame.border_color",
                "style.frame.border_width",
                "style.value_face.fill_color",
                "style.value_face.fill_color.hover",
                "style.value_face.border_color",
                "style.value_face.border_color.hover",
                "style.value_face.border_width",
                "style.value_display.color",
                "style.value_display.font_size",
                "style.value_display.font_size_mode",
                "style.value_display.font_weight",
                "style.value_display.vertical_offset",
                "style.value_display.vertical_offset_mode",
                "style.value_display.padding_inline",
                "style.value_display.padding_inline_mode",
                "style.selector_face.fill_color",
                "style.selector_face.fill_color.hover",
                "style.selector_face.border_color",
                "style.selector_face.border_color.hover",
                "style.selector_face.border_width",
                "style.selector_face.border_width_mode",
                "style.selector_face.border_radius",
                "style.selector_face.border_radius_mode",
                "style.selector_face.symbol_color",
                "style.selector_face.symbol_color.hover",
                "style.selector_face.symbol_width",
                "style.selector_face.symbol_height",
                "style.selector_face.symbol_size_mode",
                "style.dropdown.fill_color",
                "style.dropdown.border_color",
                "style.dropdown.border_width",
                "style.dropdown.border_width_mode",
                "style.dropdown.option.fill_color",
                "style.dropdown.option.text_color",
                "style.dropdown.option.hover_fill_color",
                "style.dropdown.option.hover_text_color",
                "style.dropdown.option.selected_fill_color",
                "style.dropdown.option.selected_text_color",
                "style.dropdown.option.font_family",
                "style.dropdown.option.font_size",
                "style.dropdown.option.font_size_mode",
                "style.dropdown.option.font_weight",
                "style.dropdown.option.font_style",
                "style.dropdown.option.padding_inline",
                "style.dropdown.option.padding_inline_mode",
                "style.dropdown.option.height",
                "style.dropdown.option.height_mode",
                "interaction.enabled",
                "interaction.read_only",
            ] {
                if !props[member].is_null() {
                    runtime_object.insert(member.to_string(), props[member].clone());
                }
            }
        }
        runtime
    }

    pub fn execution_artifact(&self) -> Value {
        let input = self.widget_by_id("mode_input").unwrap_or(Value::Null);
        let result = self.widget_by_id("mode_result").unwrap_or(Value::Null);
        let input_items = enum_items_from_widget(&input).unwrap_or_default();
        let result_items = enum_items_from_widget(&result).unwrap_or_default();
        let input_item = enum_item_by_id(&input_items, &self.current_mode, "mode_value").unwrap_or_else(|_| input_items[0].clone());
        let output_item = enum_item_by_id(&result_items, &self.last_result, "result_mode").unwrap_or_else(|_| result_items[0].clone());
        json!({
            "artifact_kind": "frog_runtime_execution_result",
            "artifact_governance_ref": {"path": "Versioning/Readme.md"},
            "status": "ok",
            "contract_ref": {"unit_ids": ["main"], "backend_family": self.contract["backend_family"].clone(), "source_ref": self.contract["source_ref"].clone()},
            "execution_summary": {
                "mode": "enum_value_roundtrip",
                "executed_unit": "main",
                "operation": "copy",
                "input_mode": input_item.id,
                "input_text": input_item.text,
                "input_numeric_value": input_item.numeric_value,
                "result_mode": output_item.id,
                "result_text": output_item.text,
                "result_numeric_value": output_item.numeric_value
            },
            "outputs": {"public": {"result_mode": output_item.id}, "ui": {"mode_input": input_item.id, "mode_result": output_item.id}},
            "ui_runtime": {
                "panel": {"panel_id": self.panel["panel_id"].clone(), "title": self.panel["title"].clone(), "class_ref": self.panel["class_ref"].clone(), "layout": self.panel["layout"].clone()},
                "widgets": [
                    {"widget_id": "mode_input", "class_ref": input["class_ref"].clone(), "role": "control", "layout": input["layout"].clone(), "runtime": self.runtime_for(&input, &input_item.id)},
                    {"widget_id": "mode_result", "class_ref": result["class_ref"].clone(), "role": "indicator", "layout": result["layout"].clone(), "runtime": self.runtime_for(&result, &output_item.id)}
                ]
            },
            "diagnostics": []
        })
    }

    pub fn render_html(&self) -> String {
        let snapshot = self.execution_artifact();
        let panel = &snapshot["ui_runtime"]["panel"];
        let widgets = snapshot["ui_runtime"]["widgets"].as_array().unwrap();
        let panel_width = panel["layout"]["width"].as_i64().unwrap_or(620);
        let panel_height = panel["layout"]["height"].as_i64().unwrap_or(180);
        let uses_native_kernel = self.native_kernel_bridge.is_some();
        let mut diagnostics = String::new();
        if let Some(message) = &self.last_error {
            let _ = write!(diagnostics, "<div class='diagnostic error'>{}</div>", escape_html(message));
        }
        let rendered_widgets = widgets
            .iter()
            .map(|widget| {
                let asset_id = widget["runtime"]["asset_ref"]
                    .as_str()
                    .and_then(|value| value.strip_prefix("asset:"))
                    .unwrap_or("");
                render_enum_widget(widget, self.asset_map.get(asset_id))
            })
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
             .front-panel{{position:relative;background:#ffffff;border-radius:10px;box-shadow:0 4px 14px rgba(15,23,42,0.08);overflow:visible;}}\
             .frog-widget{{position:absolute;box-sizing:border-box;}}\
             .enum-widget{{font-family:Segoe UI,Arial,sans-serif;overflow:visible;}}\
             .enum-skin{{position:absolute;inset:0;width:100%;height:100%;display:block;}}\
             .enum-skin svg{{width:100%;height:100%;display:block;}}\
             .enum-skin #label_text,.enum-skin #caption_text,.enum-skin #value_display{{display:none;}}\
             .enum-caption-overlay{{position:absolute;transform:translateY(-50%);font-size:14px;font-weight:600;line-height:1;white-space:nowrap;pointer-events:none;}}\
             .enum-value-display-overlay{{position:absolute;box-sizing:border-box;display:flex;align-items:center;padding:0 var(--frog-enum-text-padding-inline);font-family:Segoe UI,Arial,sans-serif;line-height:normal;transform:translateY(var(--frog-enum-text-vertical-offset));z-index:3;}}\
             .enum-widget .enum-display-button{{border:0;background:transparent;text-align:left;justify-content:flex-start;appearance:none;cursor:pointer;}}\
             .enum-widget .enum-display-button:focus,.enum-widget .enum-display-button:focus-visible,.enum-widget .enum-display-button:active{{outline:0;box-shadow:none;}}\
             .enum-select-state{{display:none;}}\
             .enum-selector-overlay{{position:absolute;box-sizing:border-box;display:flex;align-items:center;justify-content:center;border-style:solid;border-width:var(--frog-enum-selector-stroke-width);border-radius:var(--frog-enum-selector-radius);}}\
             .enum-selector-overlay::after{{content:'';width:0;height:0;border-left:calc(var(--frog-enum-selector-symbol-width) / 2) solid transparent;border-right:calc(var(--frog-enum-selector-symbol-width) / 2) solid transparent;border-top:var(--frog-enum-selector-symbol-height) solid currentColor;}}\
             .enum-control:has(.enum-display-button:hover) .enum-skin #value_face,.enum-control:has(.enum-dropdown:not([hidden])) .enum-skin #value_face{{fill:var(--frog-enum-value-hover-fill) !important;}}\
             .enum-indicator-value{{position:absolute;box-sizing:border-box;display:flex;align-items:center;padding:0 var(--frog-enum-text-padding-inline);pointer-events:none;line-height:normal;transform:translateY(var(--frog-enum-text-vertical-offset));}}\
             .enum-dropdown{{position:absolute;box-sizing:border-box;z-index:30;background:var(--frog-enum-dropdown-fill);border:var(--frog-enum-dropdown-border-width) solid var(--frog-enum-dropdown-border);}}\
             .enum-dropdown[hidden]{{display:none;}}\
             .enum-dropdown-option{{width:100%;min-height:var(--frog-enum-dropdown-option-height);display:flex;align-items:center;justify-content:flex-start;padding:0 var(--frog-enum-dropdown-option-padding-inline);border:0;border-radius:0;background:var(--frog-enum-dropdown-option-fill);color:var(--frog-enum-dropdown-option-text);font-family:var(--frog-enum-dropdown-option-font-family);font-size:var(--frog-enum-dropdown-option-font-size);font-weight:var(--frog-enum-dropdown-option-font-weight);font-style:var(--frog-enum-dropdown-option-font-style);text-align:left;cursor:pointer;}}\
             .enum-dropdown-option:hover,.enum-dropdown-option:focus{{background:var(--frog-enum-dropdown-option-hover-fill);color:var(--frog-enum-dropdown-option-hover-text);outline:0;}}\
             .enum-dropdown-option[aria-selected='true']{{background:var(--frog-enum-dropdown-option-selected-fill);color:var(--frog-enum-dropdown-option-selected-text);}}\
             .enum-widget .enum-selector-button{{padding:0;border-style:solid;border-width:var(--frog-enum-selector-stroke-width);border-radius:var(--frog-enum-selector-radius);font-weight:400;cursor:pointer;appearance:none;z-index:4;background:var(--frog-enum-selector-fill);border-color:var(--frog-enum-selector-stroke);color:var(--frog-enum-selector-symbol);}}\
             .enum-widget .enum-selector-button:hover{{background:var(--frog-enum-selector-hover-fill);border-color:var(--frog-enum-selector-hover-stroke);color:var(--frog-enum-selector-hover-symbol);}}\
             .enum-widget .enum-selector-button:focus,.enum-widget .enum-selector-button:focus-visible,.enum-widget .enum-selector-button:active{{outline:0;box-shadow:none;}}\
             .actions{{margin-top:16px;display:flex;gap:12px;align-items:center;}}\
             button{{padding:8px 14px;border:0;border-radius:6px;cursor:pointer;background:#0f62fe;color:#ffffff;font-weight:600;}}\
             .diagnostic{{margin:12px 0;padding:10px 12px;border-radius:6px;}}\
             .diagnostic.error{{background:#fff1f2;color:#9f1239;border:1px solid #fecdd3;}}\
             </style><script>\
             function frogCloseEnumDropdown(menuId,displayId){{const menu=document.getElementById(menuId);const display=document.getElementById(displayId);if(menu){{menu.hidden=true;}}if(display){{display.setAttribute('aria-expanded','false');}}}}\
             function frogCloseOtherEnumDropdowns(menuId){{document.querySelectorAll('.enum-dropdown').forEach(function(m){{if(m.id!==menuId){{m.hidden=true;}}}});document.querySelectorAll('.enum-display-button,.enum-selector-button').forEach(function(b){{if(b.getAttribute('aria-controls')!==menuId){{b.setAttribute('aria-expanded','false');}}}});}}\
             function frogToggleEnumDropdown(menuId,displayId){{const menu=document.getElementById(menuId);const display=document.getElementById(displayId);if(!menu){{return;}}frogCloseOtherEnumDropdowns(menuId);menu.hidden=!menu.hidden;if(display){{display.setAttribute('aria-expanded',menu.hidden?'false':'true');}}}}\
             function frogUpdateEnumDisplay(select,displayId){{const d=document.getElementById(displayId);if(!d){{return;}}const o=select.options[select.selectedIndex];if(o){{d.textContent=o.textContent;}}const menu=document.getElementById(select.id.replace('_value','_dropdown'));if(menu){{menu.querySelectorAll('.enum-dropdown-option').forEach(function(option){{option.setAttribute('aria-selected',option.getAttribute('data-enum-value')===select.value?'true':'false');}});}}}}\
             function frogSelectEnumOption(option,selectId,displayId,menuId){{const s=document.getElementById(selectId);const d=document.getElementById(displayId);if(!s||!option){{return;}}const value=option.getAttribute('data-enum-value');s.value=value;if(d){{d.textContent=option.textContent;}}frogUpdateEnumDisplay(s,displayId);frogCloseEnumDropdown(menuId,displayId);s.dispatchEvent(new Event('input',{{bubbles:true}}));s.dispatchEvent(new Event('change',{{bubbles:true}}));}}\
             document.addEventListener('click',function(event){{if(!event.target.closest('.enum-widget')){{document.querySelectorAll('.enum-dropdown').forEach(function(m){{m.hidden=true;}});document.querySelectorAll('.enum-display-button,.enum-selector-button').forEach(function(b){{b.setAttribute('aria-expanded','false');}});}}}});\
             </script></head><body>\
             <h1>{title}</h1>\
             <p class='meta'>Example 08 - .frog front panel + Default Enum .wfrog realization assets + Rust runtime</p>\
             <dl class='runtime-facts' aria-label='Runtime facts'>\
             <div><dt>Runtime</dt><dd>Rust reference runtime</dd></div>\
             <div><dt>Execution</dt><dd>{execution_path}</dd></div>\
             <div><dt>Compiler backend</dt><dd>{compiler_backend}</dd></div>\
             </dl>{diagnostics}\
             <form method='post' action='/run'>\
             <div class='front-panel' data-panel-id='{panel_id}' data-coordinate-space='panel_pixels' data-runtime-language='rust' data-compiler-backend='{compiler_backend_id}' data-execution-path='{execution_path_id}' style='width:{panel_width}px;height:{panel_height}px;'>\
             {rendered_widgets}</div>\
             <div class='actions'><button type='submit'>Run Example 08</button><a class='state-link' href='/state.json'>state.json</a></div></form>\
             </body></html>",
            title = escape_html(panel["title"].as_str().unwrap_or("FROG")),
            diagnostics = diagnostics,
            execution_path = if uses_native_kernel { "native kernel bridge" } else { "enum contract executor" },
            compiler_backend = if uses_native_kernel { "LLVM native enum kernel artifact" } else { "none for Example 08" },
            compiler_backend_id = if uses_native_kernel { "llvm" } else { "none" },
            execution_path_id = if uses_native_kernel { "native_kernel_bridge" } else { "rust_enum_contract_executor" },
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
                let _ = write_response(&mut stream, "500 Internal Server Error", "text/plain; charset=utf-8", format!("{error}").into_bytes(), None);
            }
        }
        Ok(())
    }

    fn handle_connection(&mut self, stream: &mut TcpStream) -> Result<()> {
        let request = read_request(stream)?;
        if request.method == "GET" && request.path == "/" {
            return write_response(stream, "200 OK", "text/html; charset=utf-8", self.render_html().into_bytes(), None);
        }
        if request.method == "GET" && request.path == "/state.json" {
            let payload = to_string_pretty(&self.execution_artifact()).unwrap().into_bytes();
            return write_response(stream, "200 OK", "application/json; charset=utf-8", payload, None);
        }
        if request.method == "GET" && request.path.starts_with("/asset/") {
            let asset_id = request.path.trim_start_matches("/asset/");
            if let Some(path) = self.asset_map.get(asset_id) {
                if path.exists() {
                    return write_response(stream, "200 OK", "image/svg+xml", fs::read(path)?, None);
                }
            }
            return write_response(stream, "404 Not Found", "text/plain; charset=utf-8", b"missing asset".to_vec(), None);
        }
        if request.method == "POST" && request.path == "/run" {
            let body = String::from_utf8_lossy(&request.body);
            let value = parse_form_value(&body, "mode_value").unwrap_or_else(|| "run".to_string());
            if let Err(error) = self.run_once(value) {
                self.last_error = Some(error.to_string());
            }
            return write_response(stream, "303 See Other", "text/plain; charset=utf-8", Vec::new(), Some(("Location", "/".to_string())));
        }
        write_response(stream, "404 Not Found", "text/plain; charset=utf-8", b"not found".to_vec(), None)
    }
}

fn load_string_svg_geometry(asset_path: Option<&PathBuf>) -> NumericSvgGeometry {
    let mut geometry = NumericSvgGeometry {
        view_width: 420.0,
        view_height: 190.0,
        caption_x: 16.0,
        caption_y: 46.0,
        value_face_x: 28.0,
        value_face_y: 88.0,
        value_face_width: 364.0,
        value_face_height: 56.0,
        ..NumericSvgGeometry::default()
    };
    let Some(path) = asset_path else { return geometry; };
    let Ok(svg) = fs::read_to_string(path) else { return geometry; };
    if let Some(start) = svg.find("viewBox=\"") {
        let value_start = start + "viewBox=\"".len();
        if let Some(value_end) = svg[value_start..].find('"') {
            let parts: Vec<&str> = svg[value_start..value_start + value_end].split_whitespace().collect();
            if parts.len() == 4 {
                if let (Ok(width), Ok(height)) = (parts[2].parse::<f64>(), parts[3].parse::<f64>()) {
                    geometry.view_width = width;
                    geometry.view_height = height;
                }
            }
        }
    }
    geometry.caption_x = svg_attribute_f64(&svg, "caption_text", "x", geometry.caption_x);
    geometry.caption_y = svg_attribute_f64(&svg, "caption_text", "y", geometry.caption_y);
    geometry.value_face_x = svg_attribute_f64(&svg, "text_region", "x", geometry.value_face_x);
    geometry.value_face_y = svg_attribute_f64(&svg, "text_region", "y", geometry.value_face_y);
    geometry.value_face_width = svg_attribute_f64(&svg, "text_region", "width", geometry.value_face_width);
    geometry.value_face_height = svg_attribute_f64(&svg, "text_region", "height", geometry.value_face_height);
    geometry
}

fn render_string_widget(widget: &Value, asset_path: Option<&PathBuf>) -> String {
    let runtime = &widget["runtime"];
    let layout = &widget["layout"];
    let geometry = load_string_svg_geometry(asset_path);
    let asset_ref = runtime["asset_ref"].as_str().unwrap_or("");
    let asset_route = asset_ref.strip_prefix("asset:").map(|id| format!("/asset/{id}")).unwrap_or_default();
    let value = runtime_string(runtime, "value", "");
    let caption = runtime_string(runtime, "caption.text", widget["widget_id"].as_str().unwrap_or(""));
    let is_control = widget["role"].as_str() == Some("control");
    let string_skin = if let Some(path) = asset_path {
        if path.exists() {
            let svg = fs::read_to_string(path).unwrap_or_default();
            let region_fill = safe_css_color(&runtime_string(runtime, "style.text_region.fill_color", "#ffffff"), "#ffffff");
            let region_stroke = safe_css_color(&runtime_string(runtime, "style.text_region.border_color", "#64748b"), "#64748b");
            let region_stroke_width = safe_css_length(&runtime_string(runtime, "style.text_region.border_width", "2px"), "2px");
            let region_hover_fill = safe_css_color(&runtime_string(runtime, "style.text_region.fill_color.hover", &region_fill), &region_fill);
            let region_hover_stroke =
                safe_css_color(&runtime_string(runtime, "style.text_region.border_color.hover", &region_stroke), &region_stroke);
            let region_hover_stroke_width = safe_css_length(
                &runtime_string(runtime, "style.text_region.border_width.hover", &region_stroke_width),
                &region_stroke_width,
            );
            format!(
                "<div class='string-skin' aria-hidden='true' style='--frog-string-label-display:none;--frog-string-caption-display:none;--frog-string-placeholder-display:none;--frog-string-frame-fill:{};--frog-string-frame-stroke:{};--frog-string-frame-stroke-width:{};--frog-string-text-region-fill:{};--frog-string-text-region-stroke:{};--frog-string-text-region-stroke-width:{};--frog-string-text-region-fill-hover:{};--frog-string-text-region-stroke-hover:{};--frog-string-text-region-stroke-width-hover:{};--frog-string-text-fill:{};--frog-string-text-font-size:{};--frog-string-text-font-weight:{};'>{}</div>",
                escape_html(&safe_css_color(&runtime_string(runtime, "style.frame.fill_color", "transparent"), "transparent")),
                escape_html(&safe_css_color(&runtime_string(runtime, "style.frame.border_color", "transparent"), "transparent")),
                escape_html(&safe_css_length(&runtime_string(runtime, "style.frame.border_width", "0px"), "0px")),
                escape_html(&region_fill),
                escape_html(&region_stroke),
                escape_html(&region_stroke_width),
                escape_html(&region_hover_fill),
                escape_html(&region_hover_stroke),
                escape_html(&region_hover_stroke_width),
                escape_html(&safe_css_color(&runtime_string(runtime, "style.text.color", "#111827"), "#111827")),
                escape_html(&safe_css_length(&runtime_string(runtime, "style.text.font_size", "16px"), "16px")),
                escape_html(&safe_css_font_weight(&runtime_string(runtime, "style.text.font_weight", "400"), "400")),
                svg
            )
        } else {
            "<div class='string-skin missing-skin'></div>".to_string()
        }
    } else {
        "<div class='string-skin missing-skin'></div>".to_string()
    };
    let value_style = svg_box_style(
        geometry.value_face_x,
        geometry.value_face_y,
        geometry.value_face_width,
        geometry.value_face_height,
        geometry,
    );
    let caption_overlay = format!(
        "<span class='string-caption-overlay' data-frog-part='caption' data-svg-anchor='caption.anchor' style='{}color:{};'>{}</span>",
        runtime_caption_anchor_style(runtime, geometry),
        escape_html(&safe_css_color(&runtime_string(runtime, "caption.style.text_color", "#111827"), "#111827")),
        escape_html(&caption)
    );
    let value_overlay = if is_control {
        format!(
            "<input id='{}_value' name='input_text' type='text' class='string-value-overlay string-control-editor' data-frog-part='text_value' data-svg-anchor='text_region.left_center' style='{}color:{};font-size:{};font-weight:{};' value='{}'>",
            escape_html(widget["widget_id"].as_str().unwrap_or("str_input")),
            value_style,
            escape_html(&safe_css_color(&runtime_string(runtime, "style.text.color", "#111827"), "#111827")),
            escape_html(&safe_css_length(&runtime_string(runtime, "style.text.font_size", "16px"), "16px")),
            escape_html(&safe_css_font_weight(&runtime_string(runtime, "style.text.font_weight", "400"), "400")),
            escape_html(&value)
        )
    } else {
        format!(
            "<output class='string-value-overlay string-indicator-value' data-frog-part='text_value' data-svg-anchor='text_region.left_center' style='{}color:{};font-size:{};font-weight:{};'>{}</output>",
            value_style,
            escape_html(&safe_css_color(&runtime_string(runtime, "style.text.color", "#111827"), "#111827")),
            escape_html(&safe_css_length(&runtime_string(runtime, "style.text.font_size", "16px"), "16px")),
            escape_html(&safe_css_font_weight(&runtime_string(runtime, "style.text.font_weight", "400"), "400")),
            escape_html(&value)
        )
    };
    format!(
        "<section class='frog-widget string-widget {}' data-widget-id='{}' data-class-ref='{}' data-role='{}' data-frog-visual-law='wfrog-realization-state-map' data-asset-route='{}' style='position:absolute;left:{}px;top:{}px;width:{}px;height:{}px;'>{}{}{}</section>",
        if is_control { "string-control" } else { "string-indicator" },
        escape_html(widget["widget_id"].as_str().unwrap_or("")),
        escape_html(widget["class_ref"].as_str().unwrap_or("")),
        escape_html(widget["role"].as_str().unwrap_or("")),
        escape_html(&asset_route),
        layout["x"].as_i64().unwrap_or(0),
        layout["y"].as_i64().unwrap_or(0),
        layout["width"].as_i64().unwrap_or(240),
        layout["height"].as_i64().unwrap_or(110),
        string_skin,
        caption_overlay,
        value_overlay,
    )
}

#[derive(Clone, Copy)]
struct EnumSvgGeometry {
    view_width: f64,
    view_height: f64,
    caption_x: f64,
    caption_y: f64,
    value_face_x: f64,
    value_face_y: f64,
    value_face_width: f64,
    value_face_height: f64,
    selector_face_x: f64,
    selector_face_y: f64,
    selector_face_width: f64,
    selector_face_height: f64,
}

impl Default for EnumSvgGeometry {
    fn default() -> Self {
        Self {
            view_width: 380.0,
            view_height: 150.0,
            caption_x: 16.0,
            caption_y: 46.0,
            value_face_x: 22.0,
            value_face_y: 82.0,
            value_face_width: 214.0,
            value_face_height: 28.0,
            selector_face_x: 246.0,
            selector_face_y: 82.0,
            selector_face_width: 24.0,
            selector_face_height: 28.0,
        }
    }
}

fn enum_box_style(x: f64, y: f64, width: f64, height: f64, geometry: EnumSvgGeometry) -> String {
    format!(
        "left:{};top:{};width:{};height:{};",
        css_percent(pct(x, geometry.view_width)),
        css_percent(pct(y, geometry.view_height)),
        css_percent(pct(width, geometry.view_width)),
        css_percent(pct(height, geometry.view_height))
    )
}

fn enum_dropdown_style(x: f64, y: f64, width: f64, height: f64, geometry: EnumSvgGeometry) -> String {
    format!(
        "left:{};top:{};width:{};",
        css_percent(pct(x, geometry.view_width)),
        css_percent(pct(y + height, geometry.view_height)),
        css_percent(pct(width, geometry.view_width))
    )
}

fn enum_caption_anchor_style(runtime: &Value, geometry: EnumSvgGeometry) -> String {
    let x = runtime["caption.anchor.x"].as_f64().unwrap_or(geometry.caption_x);
    let y = runtime["caption.anchor.y"].as_f64().unwrap_or(geometry.caption_y);
    let align = runtime_string(runtime, "caption.align.horizontal", "left");
    let mut style = format!(
        "left:{};top:{};",
        css_percent(pct(x, geometry.view_width)),
        css_percent(pct(y, geometry.view_height))
    );
    let _ = write!(style, "transform:{};text-align:{};", caption_transform_for_align(&align), caption_text_align(&align));
    if !runtime_bool(runtime, "caption.visible", true) {
        style.push_str("display:none;");
    }
    style
}

fn load_enum_svg_geometry(asset_path: Option<&PathBuf>) -> EnumSvgGeometry {
    let mut geometry = EnumSvgGeometry::default();
    let Some(path) = asset_path else { return geometry; };
    let Ok(svg) = fs::read_to_string(path) else { return geometry; };
    if let Some(start) = svg.find("viewBox=\"") {
        let value_start = start + "viewBox=\"".len();
        if let Some(value_end) = svg[value_start..].find('"') {
            let parts: Vec<&str> = svg[value_start..value_start + value_end].split_whitespace().collect();
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
    geometry.caption_x = svg_attribute_f64(&svg, "caption_text", "x", geometry.caption_x);
    geometry.caption_y = svg_attribute_f64(&svg, "caption_text", "y", geometry.caption_y);
    geometry.value_face_x = svg_attribute_f64(&svg, "value_face", "x", geometry.value_face_x);
    geometry.value_face_y = svg_attribute_f64(&svg, "value_face", "y", geometry.value_face_y);
    geometry.value_face_width = svg_attribute_f64(&svg, "value_face", "width", geometry.value_face_width);
    geometry.value_face_height = svg_attribute_f64(&svg, "value_face", "height", geometry.value_face_height);
    geometry.selector_face_x = svg_attribute_f64(&svg, "selector_face", "x", geometry.selector_face_x);
    geometry.selector_face_y = svg_attribute_f64(&svg, "selector_face", "y", geometry.selector_face_y);
    geometry.selector_face_width = svg_attribute_f64(&svg, "selector_face", "width", geometry.selector_face_width);
    geometry.selector_face_height = svg_attribute_f64(&svg, "selector_face", "height", geometry.selector_face_height);
    geometry
}

fn render_enum_skin(asset_path: Option<&PathBuf>, runtime: &Value) -> String {
    let Some(path) = asset_path else {
        return "<div class='enum-skin missing-skin'></div>".to_string();
    };
    let Ok(svg) = fs::read_to_string(path) else {
        return "<div class='enum-skin missing-skin'></div>".to_string();
    };
    let frame_stroke = safe_css_color(&runtime_string(runtime, "style.frame.border_color", "transparent"), "transparent");
    let frame_stroke_width = safe_css_length(&runtime_string(runtime, "style.frame.border_width", "0px"), "0px");
    let frame_visible = frame_stroke != "transparent" && frame_stroke_width != "0px";
    format!(
        "<div class='enum-skin' aria-hidden='true' style='--frog-enum-label-display:none;--frog-enum-caption-display:none;--frog-enum-value-display:none;--frog-enum-frame-display:{};--frog-enum-frame-fill:{};--frog-enum-frame-stroke:{};--frog-enum-frame-stroke-width:{};--frog-enum-value-face-fill:{};--frog-enum-value-face-stroke:{};--frog-enum-value-face-stroke-width:{};--frog-enum-selector-display:{};--frog-enum-selector-fill:{};--frog-enum-selector-stroke:{};--frog-enum-selector-stroke-width:{};--frog-enum-selector-symbol:{};--frog-enum-increment-display:none;--frog-enum-digital-display:none;--frog-enum-overflow-display:none;'>{}</div>",
        if frame_visible { "inline" } else { "none" },
        escape_html(&safe_css_color(&runtime_string(runtime, "style.frame.fill_color", "transparent"), "transparent")),
        escape_html(&frame_stroke),
        escape_html(&frame_stroke_width),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.value_face.fill_color", "#ffffff"), "#ffffff")),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.value_face.border_color", "#64748b"), "#64748b")),
        escape_html(&safe_css_length(&runtime_string(runtime, "style.value_face.border_width", "2px"), "2px")),
        if runtime_bool(runtime, "display.selector_visible", true) { "inline" } else { "none" },
        escape_html(&safe_css_color(&runtime_string(runtime, "style.selector_face.fill_color", "#f1f5f9"), "#f1f5f9")),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.selector_face.border_color", "#64748b"), "#64748b")),
        escape_html(&safe_css_length(&runtime_string(runtime, "style.selector_face.border_width", "1px"), "1px")),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.selector_face.symbol_color", "#111827"), "#111827")),
        svg
    )
}

fn render_enum_widget(widget: &Value, asset_path: Option<&PathBuf>) -> String {
    let runtime = &widget["runtime"];
    let layout = &widget["layout"];
    let geometry = load_enum_svg_geometry(asset_path);
    let items = enum_items_from_widget(widget).unwrap_or_default();
    let selected_id = runtime_string(runtime, "value", "");
    let selected = enum_item_by_id(&items, &selected_id, "enum.value").unwrap_or_else(|_| items[0].clone());
    let is_control = widget["role"].as_str() == Some("control");
    let selector_visible = runtime_bool(runtime, "display.selector_visible", is_control);
    let value_style = enum_box_style(
        geometry.value_face_x,
        geometry.value_face_y,
        geometry.value_face_width,
        geometry.value_face_height,
        geometry,
    );
    let selector_style = enum_box_style(
        geometry.selector_face_x,
        geometry.selector_face_y,
        geometry.selector_face_width,
        geometry.selector_face_height,
        geometry,
    );
    let dropdown_style = enum_dropdown_style(
        geometry.value_face_x,
        geometry.value_face_y,
        geometry.value_face_width,
        geometry.value_face_height,
        geometry,
    );
    let text_color = safe_css_color(&runtime_string(runtime, "style.value_display.color", "#111827"), "#111827");
    let text_size = safe_css_length(&runtime_string(runtime, "style.value_display.font_size", "16px"), "16px");
    let text_weight = safe_css_font_weight(&runtime_string(runtime, "style.value_display.font_weight", "400"), "400");
    let text_offset = safe_css_length(&runtime_string(runtime, "style.value_display.vertical_offset", "0px"), "0px");
    let text_padding = safe_css_length(&runtime_string(runtime, "style.value_display.padding_inline", "8px"), "8px");
    let selector_fill = safe_css_color(&runtime_string(runtime, "style.selector_face.fill_color", "#f1f5f9"), "#f1f5f9");
    let selector_stroke = safe_css_color(&runtime_string(runtime, "style.selector_face.border_color", "#64748b"), "#64748b");
    let selector_symbol = safe_css_color(&runtime_string(runtime, "style.selector_face.symbol_color", "#111827"), "#111827");
    let widget_id = widget["widget_id"].as_str().unwrap_or("mode_input");
    let caption = runtime_string(runtime, "caption.text", widget_id);
    let asset_route = runtime["asset_ref"]
        .as_str()
        .and_then(|value| value.strip_prefix("asset:"))
        .map(|id| format!("/asset/{id}"))
        .unwrap_or_default();
    let mut body = String::new();
    body.push_str(&render_enum_skin(asset_path, runtime));
    let _ = write!(
        body,
        "<span class='enum-caption-overlay' data-frog-part='caption' data-svg-anchor='caption.anchor' style='{}color:{};'>{}</span>",
        enum_caption_anchor_style(runtime, geometry),
        escape_html(&safe_css_color(&runtime_string(runtime, "caption.style.text_color", "#111827"), "#111827")),
        escape_html(&caption)
    );
    if is_control {
        let value_id = format!("{widget_id}_value");
        let display_id = format!("{widget_id}_display");
        let dropdown_id = format!("{widget_id}_dropdown");
        let _ = write!(
            body,
            "<button id='{display_id}' type='button' class='enum-value-display-overlay enum-display-button' data-frog-part='value_display' data-svg-anchor='value_display.left_center' aria-haspopup='listbox' aria-expanded='false' aria-controls='{dropdown_id}' onclick=\"frogToggleEnumDropdown('{dropdown_id}','{display_id}')\" style='{value_style}color:{};font-size:{};font-weight:{};--frog-enum-text-vertical-offset:{};'>{}</button>",
            escape_html(&text_color),
            escape_html(&text_size),
            escape_html(&text_weight),
            escape_html(&text_offset),
            escape_html(&selected.text)
        );
        if selector_visible {
            let _ = write!(
                body,
                "<button type='button' class='enum-selector-overlay enum-selector-button' data-frog-part='selector_face' aria-label='Open {}' aria-haspopup='listbox' aria-expanded='false' aria-controls='{dropdown_id}' onclick=\"frogToggleEnumDropdown('{dropdown_id}','{display_id}')\" style='{selector_style}'></button>",
                escape_html(&caption)
            );
        }
        let _ = write!(body, "<select id='{value_id}' name='mode_value' class='enum-select-state' data-frog-part='value_state' aria-hidden='true' tabindex='-1' hidden>");
        for item in &items {
            let _ = write!(
                body,
                "<option value='{}'{}{}>{}</option>",
                escape_html(&item.id),
                if item.id == selected.id { " selected" } else { "" },
                if item.enabled { "" } else { " disabled" },
                escape_html(&item.text)
            );
        }
        let _ = write!(body, "</select><div id='{dropdown_id}' class='enum-dropdown' data-frog-part='dropdown' role='listbox' aria-label='{} options' hidden style='{dropdown_style}'>", escape_html(&caption));
        for item in &items {
            let _ = write!(
                body,
                "<button type='button' class='enum-dropdown-option' role='option' data-enum-value='{}' aria-selected='{}' onclick=\"frogSelectEnumOption(this,'{value_id}','{display_id}','{dropdown_id}')\"{}>{}</button>",
                escape_html(&item.id),
                if item.id == selected.id { "true" } else { "false" },
                if item.enabled { "" } else { " disabled" },
                escape_html(&item.text)
            );
        }
        body.push_str("</div>");
    } else {
        let _ = write!(
            body,
            "<output class='enum-value-overlay enum-indicator-value' data-frog-part='value_display' data-svg-anchor='value_display.left_center' style='{value_style}color:{};font-size:{};font-weight:{};--frog-enum-text-vertical-offset:{};'>{}</output>",
            escape_html(&text_color),
            escape_html(&text_size),
            escape_html(&text_weight),
            escape_html(&text_offset),
            escape_html(&selected.text)
        );
    }
    format!(
        "<section class='frog-widget enum-widget {}' data-widget-id='{}' data-class-ref='{}' data-role='{}' data-frog-visual-law='wfrog-realization-state-map' data-frog-selector-visible='{}' data-asset-route='{}' style='position:absolute;left:{}px;top:{}px;width:{}px;height:{}px;--frog-enum-selector-fill:{};--frog-enum-selector-stroke:{};--frog-enum-selector-stroke-width:{};--frog-enum-selector-radius:{};--frog-enum-selector-symbol:{};--frog-enum-selector-symbol-width:{};--frog-enum-selector-symbol-height:{};--frog-enum-selector-hover-fill:{};--frog-enum-selector-hover-stroke:{};--frog-enum-selector-hover-symbol:{};--frog-enum-value-hover-fill:{};--frog-enum-text-padding-inline:{};--frog-enum-dropdown-fill:{};--frog-enum-dropdown-border:{};--frog-enum-dropdown-border-width:{};--frog-enum-dropdown-option-fill:{};--frog-enum-dropdown-option-text:{};--frog-enum-dropdown-option-hover-fill:{};--frog-enum-dropdown-option-hover-text:{};--frog-enum-dropdown-option-selected-fill:{};--frog-enum-dropdown-option-selected-text:{};--frog-enum-dropdown-option-font-family:{};--frog-enum-dropdown-option-font-size:{};--frog-enum-dropdown-option-font-weight:{};--frog-enum-dropdown-option-font-style:{};--frog-enum-dropdown-option-padding-inline:{};--frog-enum-dropdown-option-height:{};'>{}</section>",
        if is_control { "enum-control" } else { "enum-indicator" },
        escape_html(widget_id),
        escape_html(widget["class_ref"].as_str().unwrap_or("")),
        escape_html(widget["role"].as_str().unwrap_or("")),
        if selector_visible { "true" } else { "false" },
        escape_html(&asset_route),
        layout["x"].as_i64().unwrap_or(0),
        layout["y"].as_i64().unwrap_or(0),
        layout["width"].as_i64().unwrap_or(260),
        layout["height"].as_i64().unwrap_or(110),
        escape_html(&selector_fill),
        escape_html(&selector_stroke),
        escape_html(&safe_css_length(&runtime_string(runtime, "style.selector_face.border_width", "1px"), "1px")),
        escape_html(&safe_css_length(&runtime_string(runtime, "style.selector_face.border_radius", "1px"), "1px")),
        escape_html(&selector_symbol),
        escape_html(&safe_css_length(&runtime_string(runtime, "style.selector_face.symbol_width", "10px"), "10px")),
        escape_html(&safe_css_length(&runtime_string(runtime, "style.selector_face.symbol_height", "7px"), "7px")),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.selector_face.fill_color.hover", &selector_fill), &selector_fill)),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.selector_face.border_color.hover", &selector_stroke), &selector_stroke)),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.selector_face.symbol_color.hover", &selector_symbol), &selector_symbol)),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.value_face.fill_color.hover", "transparent"), "transparent")),
        escape_html(&text_padding),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.dropdown.fill_color", "#ffffff"), "#ffffff")),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.dropdown.border_color", "#64748b"), "#64748b")),
        escape_html(&safe_css_length(&runtime_string(runtime, "style.dropdown.border_width", "1px"), "1px")),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.dropdown.option.fill_color", "#ffffff"), "#ffffff")),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.dropdown.option.text_color", "#111827"), "#111827")),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.dropdown.option.hover_fill_color", "#2563eb"), "#2563eb")),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.dropdown.option.hover_text_color", "#ffffff"), "#ffffff")),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.dropdown.option.selected_fill_color", "#2563eb"), "#2563eb")),
        escape_html(&safe_css_color(&runtime_string(runtime, "style.dropdown.option.selected_text_color", "#ffffff"), "#ffffff")),
        escape_html(&safe_css_font_family(&runtime_string(runtime, "style.dropdown.option.font_family", "Segoe UI,Arial,sans-serif"), "Segoe UI,Arial,sans-serif")),
        escape_html(&safe_css_length(&runtime_string(runtime, "style.dropdown.option.font_size", &text_size), &text_size)),
        escape_html(&safe_css_font_weight(&runtime_string(runtime, "style.dropdown.option.font_weight", "400"), "400")),
        escape_html(&safe_css_font_style(&runtime_string(runtime, "style.dropdown.option.font_style", "normal"), "normal")),
        escape_html(&safe_css_length(&runtime_string(runtime, "style.dropdown.option.padding_inline", &text_padding), &text_padding)),
        escape_html(&safe_css_length(&runtime_string(runtime, "style.dropdown.option.height", "28px"), "28px")),
        body
    )
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
    let focus_visible = runtime_bool(runtime, "style.focus_ring.visible", false);
    let caption_geometry = NumericSvgGeometry {
        view_width: 160.0,
        view_height: 80.0,
        caption_x: 8.0,
        caption_y: 15.0,
        ..NumericSvgGeometry::default()
    };

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
    let focus_color = safe_css_color(&runtime_string(runtime, "style.focus_ring.color", "#2563eb"), "#2563eb");
    let focus_width = if focus_visible { safe_css_length(&runtime_string(runtime, "style.focus_ring.width", "3px"), "3px") } else { "0px".to_string() };
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
         --boolean-text:{};--boolean-focus-color:{};--boolean-focus-width:{};--boolean-transition:{}ms {};--boolean-pressed-inset:{};{}",
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
        text_color,
        focus_color,
        focus_width,
        transition_ms,
        transition_timing,
        pressed_inset,
        runtime_caption_anchor_vars(runtime, caption_geometry, "boolean"),
    );
    let skin = format!(
        "<span class='boolean-state-face' data-frog-part='inner_face' aria-hidden='true'></span>\
         <img class='boolean-skin' src='{}' alt='' aria-hidden='true'>",
        escape_html(&asset_route)
    );
    let mut overlays = format!(
        "<span class='boolean-caption-overlay' data-frog-part='caption' data-svg-anchor='caption.anchor' style='{}'>{}</span>",
        runtime_caption_anchor_style(runtime, caption_geometry),
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

fn source_front_panel_value(contract_path: &PathBuf, contract: &Value) -> Result<Value> {
    let source_path = contract
        .get("source_ref")
        .and_then(Value::as_object)
        .and_then(|source_ref| source_ref.get("path"))
        .and_then(Value::as_str)
        .ok_or_else(|| RuntimeError::Message("contract.source_ref.path is required.".to_string()))?;
    let mut path = PathBuf::from(source_path);
    if !path.is_absolute() {
        path = find_repo_root(contract_path)?.join(path);
    }
    Ok(serde_json::to_value(load_front_panel_from_frog_source_path(&path)?)?)
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
