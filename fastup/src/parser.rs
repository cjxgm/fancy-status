use color::Colorf32;

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


mod grammar {
    include!(concat!(env!("OUT_DIR"), "/grammar.rs"));
}

pub fn parse(input: &str) -> Result<Document, String> {
    grammar::document(input).map_err(|e| e.to_string())
}

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn test() {
        println!("{:?}", parse("f{f1\\[  }234(123456: a<sd|h|>el\\)lo) \\("));
        println!("{:?}", parse("f{f1\\[\\}234(123456: a<sd|h|>el\\)lo) \\("));
    }
}

