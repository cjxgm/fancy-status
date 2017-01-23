use super::renderer;
use fastup::Document;

#[derive(Debug, Copy, Clone, Default)]
pub struct Renderer;

impl renderer::Renderer for Renderer {
    fn render(&self, doc: &Document) -> String {
        format!("{:?}", doc)
    }
}

