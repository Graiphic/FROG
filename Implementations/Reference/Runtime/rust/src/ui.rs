use std::fmt::Write as _;
use std::io::{BufRead, BufReader, Read, Write};
use std::net::{TcpListener, TcpStream};
use std::path::PathBuf;
use std::process::Command;

use serde_json::to_string_pretty;

use crate::contract::{default_contract_path, default_wfrog_path};
use crate::diagnostics::{Result, RuntimeError};
use crate::runtime::RuntimeCore;

pub struct BrowserUiRuntime {
    pub core: RuntimeCore,
    pub last_error: Option<String>,
}

impl BrowserUiRuntime {
    pub fn new(contract_path: Option<PathBuf>, wfrog_path: Option<PathBuf>) -> Result<Self> {
        let contract = contract_path.unwrap_or(default_contract_path()?);
        let wfrog = wfrog_path.unwrap_or(default_wfrog_path()?);
        let core = RuntimeCore::from_paths(contract, wfrog)?;
        Ok(Self {
            core,
            last_error: None,
        })
    }

    pub fn render_html(&self) -> String {
        let snapshot = self.core.execution_artifact();
        let widgets = snapshot["ui_runtime"]["widgets"].as_array().unwrap();
        let ctrl = widgets
            .iter()
            .find(|entry| entry["widget_id"] == "ctrl_input")
            .unwrap();
        let ind = widgets
            .iter()
            .find(|entry| entry["widget_id"] == "ind_result")
            .unwrap();

        let ctrl_runtime = &ctrl["runtime"];
        let ind_runtime = &ind["runtime"];
        let ctrl_asset = ctrl_runtime["asset_ref"].as_str().unwrap_or("");
        let ind_asset = ind_runtime["asset_ref"].as_str().unwrap_or("");
        let ctrl_asset_url = if ctrl_asset.is_empty() {
            String::new()
        } else {
            format!("/asset/{}", ctrl_asset.split_once(':').unwrap().1)
        };
        let ind_asset_url = if ind_asset.is_empty() {
            String::new()
        } else {
            format!("/asset/{}", ind_asset.split_once(':').unwrap().1)
        };

        let mut diagnostics = String::new();
        if let Some(message) = &self.last_error {
            let _ = write!(
                diagnostics,
                "<div class='diagnostic error'>{}</div>",
                escape_html(message)
            );
        }

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
             .panel{{width:460px;min-height:170px;background:#ffffff;border-radius:10px;padding:16px;box-shadow:0 4px 14px rgba(15,23,42,0.08);}}\
             .widgets{{display:flex;gap:24px;align-items:flex-start;}}\
             .widget{{width:180px;padding:12px;border-radius:8px;border:2px solid #cbd2d9;background:#fbfdff;}}\
             .widget img{{display:block;width:100%;height:32px;object-fit:contain;margin-bottom:8px;}}\
             .widget label{{display:block;margin-bottom:8px;font-size:12px;font-weight:600;text-transform:uppercase;letter-spacing:0.04em;}}\
             .widget input,.widget output{{display:block;width:100%;padding:8px 10px;box-sizing:border-box;border-radius:6px;border:1px solid #9aa5b1;font-size:16px;}}\
             .widget output{{background:#f8fff0;}}\
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
             <div><dt>Execution</dt><dd>contract executor</dd></div>\
             <div><dt>Compiler backend</dt><dd>none in runtime path</dd></div>\
             </dl>\
             {diagnostics}\
             <div class='panel'><form method='post' action='/run'>\
             <div class='widgets'>\
             <section class='widget' style='border-color:{ctrl_color}'>\
             <label>{ctrl_label}</label><img src='{ctrl_asset_url}' alt=''>\
             <input name='input_value' type='number' min='0' max='65535' value='{ctrl_value}' {ctrl_disabled}>\
             </section>\
             <section class='widget' style='border-color:{ind_color}'>\
             <label>{ind_label}</label><img src='{ind_asset_url}' alt=''>\
             <output>{ind_value}</output>\
             </section></div>\
             <div class='actions'><button type='submit'>Run Example 05</button><a href='/state.json'>state.json</a></div>\
             </form><details><summary>Current runtime snapshot</summary><pre>{snapshot_pretty}</pre></details></div>\
             </body></html>",
            title = escape_html(snapshot["ui_runtime"]["panel"]["title"].as_str().unwrap_or("FROG")),
            diagnostics = diagnostics,
            ctrl_color = escape_html(ctrl_runtime["foreground_color"].as_str().unwrap_or("#5B9BD5")),
            ctrl_label = escape_html(ctrl_runtime["label"].as_str().unwrap_or("Input")),
            ctrl_asset_url = escape_html(&ctrl_asset_url),
            ctrl_value = ctrl_runtime["value"].as_u64().unwrap_or(0),
            ctrl_disabled = if ctrl_runtime["enabled"].as_bool().unwrap_or(true) { "" } else { "disabled" },
            ind_color = escape_html(ind_runtime["foreground_color"].as_str().unwrap_or("#70AD47")),
            ind_label = escape_html(ind_runtime["label"].as_str().unwrap_or("Accumulated result")),
            ind_asset_url = escape_html(&ind_asset_url),
            ind_value = ind_runtime["value"].as_u64().unwrap_or(0),
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
            let payload = to_string_pretty(&self.core.execution_artifact()).unwrap().into_bytes();
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
                    if let Err(error) = self.core.execute(Some(parsed)) {
                        self.last_error = Some(error.to_string());
                    } else {
                        self.last_error = None;
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
