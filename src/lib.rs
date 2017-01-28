extern crate stats_once;
extern crate time;
extern crate color;
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
    RendererNotFound,
}

pub type Result<T> = ::std::result::Result<T, Error>;

/// Parse `source` as fastup markup language, expand all widgets, then
/// renders it into a `String` by the specific renderer named as `renderer_name`.
pub fn render_status(source: &str, renderer_name: &str) -> Result<String> {
    let doc = parse(&source).map_err(Error::ParseFailed)?;
    let doc = widgets::expand(doc);
    let renderer = renderers::make(renderer_name).ok_or(Error::RendererNotFound)?;
    Ok(renderer.render(&doc))
}

#[cfg(test)]
mod tests {
    use super::*;

    const TEST_SOURCE: &'static str = r"[ffffff:(000000: hello [cceeff: world(0000ff: yes) or {\[n\]o}] <time|hello|world> up)]";

    #[test]
    fn dump() {
        println!("{}", render_status(TEST_SOURCE, "dump").unwrap());
    }

    #[test]
    fn html() {
        println!("{}", render_status(TEST_SOURCE, "html").unwrap());
    }

    #[test]
    fn ansi() {
        println!("{}", render_status(TEST_SOURCE, "ansi").unwrap());
    }

    #[test]
    #[should_panic]
    fn renderer_not_found() {
        println!("{}", render_status(TEST_SOURCE, "not exists").unwrap());
    }
}

