use fastup::{Document, Node};
use fastup::Node::*;
use color::Color888;
use std::fmt;

#[derive(Debug, Copy, Clone, Default)]
pub struct Renderer;

impl super::Renderer for Renderer {
    fn render(&self, doc: &Document) -> String {
        let env = Environment::default();
        tmux_from_document(doc, &env) + "#[default]"
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
        let mut style = "#[default".to_string();
        if let Some(color) = env.foreground {
            style += &format!(",fg=#{}", color);
        }
        if let Some(color) = env.background {
            style += &format!(",bg=#{}", color);
        }
        if env.bold {
            style += &format!(",bold");
        }
        style += "]";
        style
    }
}

impl fmt::Display for Environment {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", String::from(self))
    }
}

fn tmux_from_document(doc: &Document, env: &Environment) -> String {
    doc.0.iter().map(|node| tmux_from_node(node, env)).collect()
}

fn tmux_from_node(node: &Node, env: &Environment) -> String {
    match *node {
        Text(ref text) => format!("{}{}", env, escape_tmux_format(text)),
        Foreground(color, ref doc) => {
            let env = Environment { foreground: Some(color.clamp_to_888()), ..*env };
            tmux_from_document(doc, &env)
        }
        Background(color, ref doc) => {
            let env = Environment { background: Some(color.clamp_to_888()), ..*env };
            tmux_from_document(doc, &env)
        }
        Bold(ref doc) => {
            let env = Environment { bold: true, ..*env };
            tmux_from_document(doc, &env)
        }
        Widget(..) => unreachable!(),
    }
}

fn escape_tmux_format(text: &str) -> String {
    text.replace("#", "###[]")
}

