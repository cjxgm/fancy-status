use fastup::{Document, Node};
use fastup::Node::*;

#[derive(Debug, Copy, Clone, Default)]
pub struct Renderer;

impl super::Renderer for Renderer {
    fn render(&self, doc: &Document) -> String {
        dump_from_document(doc, "")
    }
}

fn dump_from_document(doc: &Document, indent: &str) -> String {
    doc.0
        .iter()
        .map(|node| dump_from_node(node, indent))
        .collect::<Vec<_>>()
        .join(&format!("\n{}", indent))
}

const INDENTATION: &'static str = "    ";

fn dump_from_document_followed(doc: &Document, indent: &str) -> String {
    assert!(doc.0.len() > 0);

    if doc.0.len() == 1 {
        " ".to_string() + &dump_from_document(doc, indent)
    } else {
        let indent = format!("{}{}", INDENTATION, indent);
        format!(":\n{}", indent) + &dump_from_document(doc, &indent)
    }
}

fn dump_from_node(node: &Node, indent: &str) -> String {
    match *node {
        Text(ref text) => format!("{:?}", text),
        Foreground(color, ref doc) => {
            format!("({}){}",
                    color.clamp_to_888(),
                    dump_from_document_followed(doc, indent))
        }
        Background(color, ref doc) => {
            format!("[{}]{}",
                    color.clamp_to_888(),
                    dump_from_document_followed(doc, indent))
        }
        Bold(ref doc) => format!("{{}}{}", dump_from_document_followed(doc, indent)),
        Widget(..) => unreachable!(),
    }
}

