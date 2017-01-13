
use utils::Color;
use typed_arena::Arena as TypedArena;
type NodeArena = TypedArena<Node>;

pub struct Tree {
    arena: NodeArena,
}

pub enum Node<'a> {
    Text(String),
    Bold(&'a Node),
    ColorFg(Color, &'a Node),
    ColorBg(Color, &'a Node),
}

impl Tree {
    fn new() -> Self {
        Tree { arena: NodeArena::new() }
    }

    fn node(&self, value: Node) -> &Node {
        self.arena.alloc(value)
    }
}

