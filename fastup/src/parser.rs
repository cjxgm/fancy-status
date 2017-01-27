use color::{Colorf32, Color888};

/// Document is a sequence of nodes concatenated together.
#[derive(Debug)]
pub struct Document(pub Vec<Node>);

#[derive(Debug)]
pub enum Node {
    /// Pure text.
    Text(String),
    /// Set foreground color for the child document.
    Foreground(Colorf32, Document),
    /// Set background color for the child document.
    Background(Colorf32, Document),
    /// Bold the child document.
    Bold(Document),
    /// `Widget(name, args)` calls the widget named `name` with `args`.
    Widget(String, Vec<String>),
}

impl From<Vec<Node>> for Document {
    fn from(nodes: Vec<Node>) -> Self {
        Document(nodes)
    }
}

mod grammar {
    include!(concat!(env!("OUT_DIR"), "/grammar.rs"));
}

/// Parse `input` string into a `Document`.
pub fn parse(input: &str) -> Result<Document, String> {
    grammar::document(input).map_err(|e| e.to_string())
}

/// Parse `input` string into a `Document`. A `Document` is a
/// sequence of `Node`s. Take the first `Node` and discard all
/// the rest.
pub fn parse_for_first_node(input: &str) -> Result<Node, String> {
    parse(input).and_then(|doc| {
        doc.0
            .into_iter()
            .next()
            .ok_or("not a single node".into())
    })
}

/// Parse `input` string into a `Color888`.
/// `input` should be in `rrggbb` format, all in lowercase.
pub fn parse_color(input: &str) -> Result<Color888, String> {
    grammar::color(input).map_err(|e| e.to_string())
}

/// Escape `input` so that it will only generate `Text` node,
/// removing magic in the `input` string.
pub fn escape_for_text(input: &str) -> String {
    input.chars().map(escape_char_for_text).collect()
}

fn escape_char_for_text(ch: char) -> String {
    match ch {
        '<' | '>' | '(' | ')' | '[' | ']' | '{' | '}' | '\\' | '|' => {
            let mut s = '\\'.to_string();
            s.push(ch);
            s
        }
        _ => ch.to_string(),
    }
}

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn test() {
        println!("{:?}", parse("f{f1\\[  }234(123456: a<sd|h|>el\\)lo) \\("));
        println!("{:?}", parse("f{f1\\[\\}234(123456: a<sd|h|>el\\)lo) \\("));
    }

    #[test]
    fn escape() {
        let escaped = escape_for_text("hello <([{\\|}])> world");
        let should_be = "hello \\<\\(\\[\\{\\\\\\|\\}\\]\\)\\> world";
        assert_eq!(escaped, should_be);
    }
}

