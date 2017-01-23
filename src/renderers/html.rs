use super::renderer::{self, Result};
use fastup::Document;

#[derive(Debug, Copy, Clone, Default)]
pub struct Renderer;

impl renderer::Renderer for Renderer {
    fn render(&self, doc: &Document) -> Result<String> {
        Ok("".into())
    }
}

