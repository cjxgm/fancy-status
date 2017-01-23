use fastup::Node;

pub trait Widget {
    fn expand(&self, name: String, args: Vec<String>) -> Node;
}

