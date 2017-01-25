use fastup::Document;

#[derive(Debug, Copy, Clone, Default)]
pub struct Renderer;

impl super::Renderer for Renderer {
    fn render(&self, doc: &Document) -> String {
        "".into()
    }
}

