use fastup::Document;

pub trait Renderer {
    fn render(&self, doc: &Document) -> String;
}

