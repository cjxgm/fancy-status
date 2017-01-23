use fastup::Document;

pub type Result<T> = ::std::result::Result<T, String>;

pub trait Renderer {
    fn render(&self, doc: &Document) -> Result<String>;
}

