extern crate fastup;
use fastup::parse;

#[macro_use]
pub mod utils;
pub mod output;
pub mod renderers;
pub mod widgets;

#[derive(Debug)]
pub enum Error {
    ParseFailed(String),
    RenderFailed(String),
    RendererNotFound,
}

pub type Result<T> = ::std::result::Result<T, Error>;

/// Parse `source` as fastup markup language, expand all widgets, then
/// renders it into a `String` by the specific renderer named as `renderer_name`.
pub fn render_status(source: &str, renderer_name: &str) -> Result<String> {
    let doc = parse(&source).map_err(Error::ParseFailed)?;
    let doc = widgets::expand(doc);
    let renderer = renderers::make(renderer_name).ok_or(Error::RendererNotFound)?;
    renderer.render(&doc).map_err(Error::RenderFailed)
}

