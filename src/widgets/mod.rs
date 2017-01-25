use fastup::{Document, Node, parse};
use fastup::Node::{Text, Foreground, Background, Bold};

/// Expand widgets to nodes until there are no widgets in the node tree
/// any more.
///
/// It is guranteed that all `Node`s in the returned `Document`
/// contains no `Node::Widget(..)`.
pub fn expand(doc: Document) -> Document {
    Document(doc.0.into_iter().map(expand_node).collect())
}

fn expand_node(node: Node) -> Node {
    match node {
        Text(..) => node,
        Foreground(color, doc) => Foreground(color, expand(doc)),
        Background(color, doc) => Background(color, expand(doc)),
        Bold(doc) => Bold(expand(doc)),
        Node::Widget(..) => {
            expand_node(error_node("unimplemented"))
        }
    }
}

/// Create a `Node` as formatted error message `err`.
/// The `err` should be a valid fastup markup text.
pub fn error_node(err: &str) -> Node {
    parse(&format!("[ff0000: (ffffff: {{\\<ERROR: {}\\>}})]", err))
        .unwrap()
        .0
        .into_iter()
        .next()
        .unwrap()
}

factory! {
    pub trait Widget {
        fn expand(&self, args: Vec<String>) -> Node;
    }

    /// Create a widget by `name`.
    make {
    }
}

