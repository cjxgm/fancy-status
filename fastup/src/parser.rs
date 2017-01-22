use color::Colorf32;

/// Document is a sequence of nodes concatenated together.
#[derive(Debug)]
pub struct Document(pub Vec<Node>);

#[derive(Debug)]
pub enum Node {
    /// Pure text.
    Text(String),
    /// Set foreground, currently only color.
    Foreground(Colorf32, Document),
    /// Set background, currently only color.
    Background(Colorf32, Document),
    /// Bold the node.
    Bold(Document),
    /// `Widget(name, args)` calls the widget named `name` with `args`.
    Widget(String, Vec<String>),
}


mod grammar {
    include!(concat!(env!("OUT_DIR"), "/grammar.rs"));
}

#[cfg(test)]
mod test {
    use super::grammar;

    #[test]
    fn test() {
        println!("{:?}", grammar::document("f{f1\\[}234(123456: a<sd|h|>el\\)lo) \\("));
    }
}

