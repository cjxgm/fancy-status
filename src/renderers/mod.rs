pub mod renderer;
pub mod dump;
pub mod html;
pub mod ansi;
pub mod tmux;

use self::renderer::Renderer;

pub fn make(name: &str) -> Option<Box<Renderer>> {
    match name {
        "dump" => Some(Box::new(dump::Renderer::default())),
        "html" => Some(Box::new(html::Renderer::default())),
        "ansi" => Some(Box::new(ansi::Renderer::default())),
        "tmux" => Some(Box::new(tmux::Renderer::default())),
        _ => None,
    }
}

