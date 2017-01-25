use fastup::{Document, Node};
use fastup::Node::*;

#[derive(Debug, Copy, Clone, Default)]
pub struct Renderer;

impl super::Renderer for Renderer {
    fn render(&self, doc: &Document) -> String {
        html_from_document(doc)
    }
}

fn html_from_document(doc: &Document) -> String {
    doc.0.iter().map(html_from_node).collect()
}

fn html_from_node(node: &Node) -> String {
    match *node {
        Text(ref text) => escape_html(text),
        Foreground(color, ref doc) => {
            format!("<span style=\"color: #{}\">{}</span>",
                    color.clamp_to_888(),
                    html_from_document(doc))
        }
        Background(color, ref doc) => {
            format!("<span style=\"background-color: #{}\">{}</span>",
                    color.clamp_to_888(),
                    html_from_document(doc))
        }
        Bold(ref doc) => format!("<b>{}</b>", html_from_document(doc)),
        Widget(..) => unreachable!(),
    }
}

fn escape_html(text: &str) -> String {
    text.chars().map(escape_html_char).collect()
}

fn escape_html_char(ch: char) -> String {
    match ch {
        '\n' => "<br>".into(),
        ' ' => "&nbsp;".into(),
        '>' => "&gt;".into(),
        '<' => "&lt;".into(),
        c => c.to_string(),
    }
}

