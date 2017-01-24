//! A `Renderer` trait with various implementations and a factory function.
//!
//! A renderer takes a `Document` which comes from fastup parser to produce
//! an output like HTML for browsers or ANSI Escape Sequence for terminals.
pub mod renderer;
pub mod dump;
pub mod html;
pub mod ansi;
pub mod tmux;

use self::renderer::Renderer;

/// A factory function to generate a Renderer by a string name.
/// The `name` is the same as the renderer module's name.
/// e.g. there is a `dump` module here, and you can `name` it `"dump"`.
pub fn make(name: &str) -> Option<Box<Renderer>> {
    match name {
        "dump" => Some(Box::new(dump::Renderer::default())),
        "html" => Some(Box::new(html::Renderer::default())),
        "ansi" => Some(Box::new(ansi::Renderer::default())),
        "tmux" => Some(Box::new(tmux::Renderer::default())),
        _ => None,
    }
}

