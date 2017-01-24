//! `Renderer` trait for renderers to implement.

use fastup::Document;

/// Renders `Document` from the fastup parser into a `String` for output.
///
/// A `Document` is made up of a vector of `Node`s. It is guranteed that
/// there will be no `Node::Widget(..)` in the entire tree of `Document`s
/// or `Node`s.
///
/// When you are pattern matching on a `Node`, you should have an arm of
/// `Node::Widget(..) => unreachable!()`.
pub trait Renderer {
    fn render(&self, doc: &Document) -> String;
}

