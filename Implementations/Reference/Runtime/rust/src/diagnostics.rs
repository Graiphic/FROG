use thiserror::Error;

#[derive(Debug, Error)]
pub enum RuntimeError {
    #[error("{0}")]
    Message(String),

    #[error("I/O error: {0}")]
    Io(#[from] std::io::Error),

    #[error("JSON error: {0}")]
    Json(#[from] serde_json::Error),

    #[error("Parse error: {0}")]
    ParseInt(#[from] std::num::ParseIntError),
}

pub type Result<T> = std::result::Result<T, RuntimeError>;

pub fn ensure(condition: bool, message: impl Into<String>) -> Result<()> {
    if condition {
        Ok(())
    } else {
        Err(RuntimeError::Message(message.into()))
    }
}
