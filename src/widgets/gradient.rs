use color::Color888;
use fastup::{parse_for_first_node, Node, parse_color};
use super::error_node;

#[derive(Debug, Copy, Clone, Default)]
pub struct Widget;

impl super::Widget for Widget {
    fn expand(&self, args: Vec<String>) -> Node {
        match args.len() {
            3 => node_from_args(args),
            _ => error_node("(ccdd44: gradient) takes 3 arguments: width\\|color1\\|color2"),
        }
    }
}

fn node_from_args(args: Vec<String>) -> Node {
    try_node_from_args(args).unwrap_or_else(|e| error_node(&e))
}

fn try_node_from_args(args: Vec<String>) -> Result<Node, String> {
    assert_eq!(args.len(), 3);

    let width = args[0].parse::<i32>().map_err(|e| e.to_string())?;
    if width < 2 { return Err("(ccdd44: gradient width) must \\>= 2".into()) }
    let color1 = parse_color(&args[1])?;
    let color2 = parse_color(&args[2])?;

    Ok(gradient_node(width, color1, color2))
}

fn gradient_node(width: i32, color1: Color888, color2: Color888) -> Node {
    assert!(width >= 2);
    parse_for_first_node("(ff0000: TODO)").unwrap()
}

