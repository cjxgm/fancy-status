use fastup::{Document, Node, parse_for_first_node};
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
        Node::Widget(name, args) => {
            let node = make(&name)
                .map(|widget| widget.expand(args))
                .unwrap_or_else(|| no_widget_error_node(&name));
            expand_node(node)
        }
    }
}

fn no_widget_error_node(widget_name: &str) -> Node {
    let err = format!("no such widget: (ccdd44: {})", widget_name);
    error_node(&err)
}

/// Create a `Node` as formatted error message `err`.
/// The `err` should be a valid fastup markup text.
pub fn error_node(err: &str) -> Node {
    let source = format!("[551100: (ff6666: \
                            \\ (44ccdd:\\<)ERROR: {}(44ccdd:\\>) )]",
                         err);
    parse_for_first_node(&source).unwrap()
}

factory! {
    pub trait Widget {
        fn expand(&self, args: Vec<String>) -> Node;
    }

    /// Create a widget by `name`.
    make {
        time;
        gradient;
        cpu;
        mem;
        net;
    }
}

