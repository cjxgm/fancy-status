use fastup::{Document, Node};
use fastup::Node::*;
use color::{Color, Color888};
use std::fmt;

#[derive(Debug, Copy, Clone, Default)]
pub struct Renderer;

impl super::Renderer for Renderer {
    fn render(&self, doc: &Document) -> String {
        let env = Environment::default();
        ansi_from_document(doc, &env) + "\x1b[0m"
    }
}

#[derive(Debug, Copy, Clone, Default)]
struct Environment {
    foreground: Option<Color888>,
    background: Option<Color888>,
    bold: bool,
}

impl<'a> From<&'a Environment> for String {
    fn from(env: &'a Environment) -> Self {
        let mut style = "\x1b[0m".to_string();
        if let Some(Color(r, g, b)) = env.foreground {
            style += &format!("\x1b[38;2;{};{};{}m", r, g, b);
        }
        if let Some(Color(r, g, b)) = env.background {
            style += &format!("\x1b[48;2;{};{};{}m", r, g, b);
        }
        if env.bold {
            style += &format!("\x1b[1m");
        }
        style
    }
}

impl fmt::Display for Environment {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{}", String::from(self))
    }
}

fn ansi_from_document(doc: &Document, env: &Environment) -> String {
    doc.0.iter().map(|node| ansi_from_node(node, env)).collect()
}

fn ansi_from_node(node: &Node, env: &Environment) -> String {
    match *node {
        Text(ref text) => format!("{}{}", env, text),
        Foreground(color, ref doc) => {
            let env = Environment { foreground: Some(color.clamp_to_888()), ..*env };
            ansi_from_document(doc, &env)
        }
        Background(color, ref doc) => {
            let env = Environment { background: Some(color.clamp_to_888()), ..*env };
            ansi_from_document(doc, &env)
        }
        Bold(ref doc) => {
            let env = Environment { bold: true, ..*env };
            ansi_from_document(doc, &env)
        }
        Widget(..) => unreachable!(),
    }
}

