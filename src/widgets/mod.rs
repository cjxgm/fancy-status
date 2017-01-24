pub mod widget;

use fastup::{Document, Node, parse};
use fastup::Node::*;

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
        Widget(..) => {
            expand_node(parse("[ff0000: (ffffff: {\\<STUB: unimplemented\\>})]")
                .unwrap()
                .0
                .into_iter()
                .next()
                .unwrap())
        }
    }
}

