use frog_reference_runtime_rust::cli::run_cli;

fn main() {
    if let Err(err) = run_cli() {
        eprintln!("{err}");
        std::process::exit(1);
    }
}
