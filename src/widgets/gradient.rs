use color::Colorf32;
use fastup::{self, Node, parse_for_first_node, parse_color};
use super::error_node;

#[derive(Debug, Copy, Clone, Default)]
pub struct Widget;

impl super::Widget for Widget {
    fn expand(&self, args: Vec<String>) -> Node {
        match args.len() {
            3 => node_from_args(args),
            4..=5 => node_from_args(args),   // debug only
            _ => error_node("(ccdd44: gradient) takes 3 arguments: width\\|color1\\|color2"),
        }
    }
}

fn node_from_args(args: Vec<String>) -> Node {
    try_node_from_args(args).unwrap_or_else(|e| error_node(&e))
}

fn try_node_from_args(args: Vec<String>) -> Result<Node, String> {
    assert!(args.len() == 3 || (4 <= args.len() && args.len() <= 5));

    let width = args[0].parse::<i32>().map_err(escape_fastup)?;
    if width < 2 {
        return Err("(ccdd44: gradient width) must \\>= 2".into());
    }

    let color1 = parse_color(&args[1]).map_err(escape_fastup)?;
    let color2 = parse_color(&args[2]).map_err(escape_fastup)?;
    let mut color1 = Colorf32::from(color1);
    let mut color2 = Colorf32::from(color2);

    let parse_lightness = |i| -> Result<Option<f32>, String> {
        if args.len() <= i || args[i].trim().len() == 0 {
            Ok(None)
        } else {
            let lightness = args[i].parse().map_err(escape_fastup)?;
            Ok(Some(lightness))
        }
    };
    if let Some(lightness) = parse_lightness(3)? {
        color1 = color1.set_lightness(lightness);
    }
    if let Some(lightness) = parse_lightness(4)? {
        color2 = color2.set_lightness(lightness);
    }

    Ok(gradient_node(width, color1, color2))
}

fn gradient_node(width: i32, color1: Colorf32, color2: Colorf32) -> Node {
    assert!(width >= 2);
    let gradient = (0..width)
        .map(|x| {
            let x = (x as f32) / ((width - 1) as f32);
            let mix1 = color1.mix(color2, x).clamp_to_888();
            let mix2 = color1.mix_fast(color2, x).clamp_to_888();
            format!("[{}: ({}: ▄)]", mix1, mix2)
        })
        .collect::<String>();
    let gradient = format!("(000000: {})", gradient);
    parse_for_first_node(&gradient).unwrap()
}

fn escape_fastup<T>(input: T) -> String
    where T: ToString
{
    fastup::escape_for_text(&input.to_string())
}

