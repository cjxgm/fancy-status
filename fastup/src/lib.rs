//! # fastup: FAncy STatus markUP language
//!
//! ```fastup
//! hello [cceeff: world(0000ff: yes) or {\[n\]o}] <date|hello|world> up
//! ```
//!
//! becomes
//!
//! <span style="font-family: monospace">
//! hello&nbsp;<span style="background-color: #cceeff">world<span style="color: #0000ff">yes</span>&nbsp;or&nbsp;<b>[n]o</b></span>&nbsp;<span style="background-color: #ff0000"><span style="color: #ffffff"><b>&lt;Error:&nbsp;date&nbsp;accepts&nbsp;no&nbsp;argument&gt;</b></span></span>&nbsp;up
//! </span>

extern crate color;
mod parser;
pub use parser::*;

#[cfg(test)]
mod tests {
    use super::*;
    use Node::*;

    fn html_from_document(doc: Document) -> String {
        doc.0.into_iter().map(html_from_node).collect()
    }

    fn html_from_node(node: Node) -> String {
        match node {
            Text(text) => escape_html(&text),
            Foreground(color, doc) => format!("<span style=\"color: #{}\">{}</span>", color.clamp_to_888(), html_from_document(doc)),
            Background(color, doc) => format!("<span style=\"background-color: #{}\">{}</span>", color.clamp_to_888(), html_from_document(doc)),
            Bold(doc) => format!("<b>{}</b>", html_from_document(doc)),
            Widget(name, args) => unreachable!(),
        }
    }

    fn escape_html(text: &str) -> String {
        text.chars().map(|ch| -> String {
            match ch {
                '\n' => "<br>".into(),
                ' '  => "&nbsp;".into(),
                '>'  => "&gt;".into(),
                '<'  => "&lt;".into(),
                c    => c.to_string(),
            }
        }).collect()
    }

    fn expand_widgets(doc: Document) -> Document {
        doc.0.into_iter().map(|node| -> Node {
            match node {
                Text(_) => node,
                Foreground(color, doc) => Foreground(color, expand_widgets(doc)),
                Background(color, doc) => Background(color, expand_widgets(doc)),
                Bold(doc) => Bold(expand_widgets(doc)),
                Widget(..) => expand_widget(node),
            }
        }).collect::<Vec<Node>>().into()
    }

    fn expand_widget(widget: Node) -> Node {
        match widget {
            Widget(name, _) => parse(&format!("[ff0000: (ffffff: {{\\<Error: {} accepts no argument\\>}})]", name)).unwrap().0.into_iter().next().unwrap(),
            _ => unreachable!(),
        }
    }

    const TEST_SOURCE: &'static str = r"hello [cceeff: world(0000ff: yes) or {\[n\]o}] <date|hello|world> up";

    #[test]
    fn generate_html() {
        let doc = parse(TEST_SOURCE).unwrap();
        let doc = expand_widgets(doc);
        let html = html_from_document(doc);
        let correct = r#"hello&nbsp;<span style="background-color: #cceeff">world<span style="color: #0000ff">yes</span>&nbsp;or&nbsp;<b>[n]o</b></span>&nbsp;<span style="background-color: #ff0000"><span style="color: #ffffff"><b>&lt;Error:&nbsp;date&nbsp;accepts&nbsp;no&nbsp;argument&gt;</b></span></span>&nbsp;up"#;
        assert_eq!(html, correct);
    }
}

