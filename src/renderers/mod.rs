//! A `Renderer` trait with various implementations and a factory function.
//!
//! A renderer takes a `Document` which comes from fastup parser to produce
//! an output like HTML for browsers or ANSI Escape Sequence for terminals.
use fastup::Document;

factory! {
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

    /// A factory function to generate a Renderer by a string name.
    /// The `name` is the same as the renderer module's name.
    /// e.g. there is a `dump` module here, and you can `name` it `"dump"`.
    make {
        dump;
        html;
        ansi;
        tmux;
    }
}

