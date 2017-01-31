use color::Colorf32;
use fastup::{self, Node, parse_for_first_node, parse_color};
use stats_once::{MEM_TOTAL, MEM_USED};
use super::error_node;
use std::ops::{Add, Sub, Mul, Div};

#[derive(Debug, Copy, Clone, Default)]
pub struct Widget;

impl super::Widget for Widget {
    fn expand(&self, args: Vec<String>) -> Node {
        match args.len() {
            2 => node_from_args(args),
            _ => error_node("(ccdd44: mem) takes 2 arguments: color-cold\\|color-hot"),
        }
    }
}

fn node_from_args(args: Vec<String>) -> Node {
    try_node_from_args(args).unwrap_or_else(|e| error_node(&e))
}

fn try_node_from_args(args: Vec<String>) -> Result<Node, String> {
    assert_eq!(args.len(), 2);

    let cold = parse_color(&args[0]).map_err(escape_fastup)?;
    let hot = parse_color(&args[1]).map_err(escape_fastup)?;

    Ok(mem_node(cold.into(), hot.into()))
}

fn mem_node(cold: Colorf32, hot: Colorf32) -> Node {
    const NONE_LIGHTNESS: f32 = 0.15;
    const FULL_LIGHTNESS: f32 = 0.7;
    const TEXT_LIGHTNESS_BOOST_NONE: f32 = 0.15;
    const TEXT_LIGHTNESS_BOOST_FULL: f32 = 0.25;

    let mem_used = *MEM_USED;
    let mem_total = *MEM_TOTAL;
    let mem_usage = (mem_used as f32) / (mem_total as f32);

    let none_cold = cold.set_lightness(NONE_LIGHTNESS);
    let none_hot = hot.set_lightness(NONE_LIGHTNESS);
    let none = none_cold.mix(none_hot, mem_usage);

    let full_lightness = remap(mem_usage, 0.0, 1.0, NONE_LIGHTNESS, FULL_LIGHTNESS);
    let full_cold = cold.set_lightness(full_lightness);
    let full_hot = hot.set_lightness(full_lightness);
    let full = full_cold.mix(full_hot, mem_usage);

    let text_lightness_boost = remap(mem_usage, 0.0, 1.0, TEXT_LIGHTNESS_BOOST_NONE, TEXT_LIGHTNESS_BOOST_FULL);
    let text_lightness = full_lightness + text_lightness_boost;
    let text_cold = cold.set_lightness(text_lightness);
    let text_hot = hot.set_lightness(text_lightness);
    let text = text_cold.mix(text_hot, mem_usage);

    let percent = (100.0 * mem_usage) as i32;
    let usage = format!(" {:>4}M{:>3}% ", mem_used, percent);
    let width = usage.len() as i32;

    let (fully_filled_cells, partial_fill_rate) = quantize(mem_usage, width);
    let partial = none.mix(full, partial_fill_rate);

    let none = none.clamp_to_888();
    let full = full.clamp_to_888();
    let text = text.clamp_to_888();
    let partial = partial.clamp_to_888();

    let (full_cells, partial_cell, none_cells) = zipper_split(&usage, fully_filled_cells as usize);
    let full_cells = format!("[{}:{}]", full, escape_fastup(full_cells));
    let partial_cell = format!("[{}:{}]", partial, escape_fastup(partial_cell));
    let none_cells = format!("[{}:{}]", none, escape_fastup(none_cells));
    let usage = format!("{}{}{}", full_cells, partial_cell, none_cells);

    let node = format!("({}:{})", text, usage);
    parse_for_first_node(&node).unwrap()
}

fn escape_fastup<T>(input: T) -> String
    where T: ToString
{
    fastup::escape_for_text(&input.to_string())
}

/// Linearly remap `x` from inclusive range [`xa`, `xb`] to
/// inclusive range [`ra`, `rb`]. `x` must fall inside [`xa`, `xb`],
/// or panic will be issued.
fn remap<T>(x: T, xa: T, xb: T, ra: T, rb: T) -> T
    where T: PartialOrd + Add<Output = T> + Sub<Output = T> + Mul<Output = T> + Div<Output = T> + Copy
{
    assert!(xa <= x && x <= xb);
    (x - xa) * (rb - ra) / (xb - xa) + ra
}

/// Returns (quantized, remaining)
fn quantize(x: f32, steps: i32) -> (i32, f32) {
    let x = x * (steps as f32);
    let quantized = x.floor();
    let remaining = x - quantized;
    let quantized = quantized as i32;
    (quantized, remaining)
}

fn byte_index_from_char_index(s: &str, nchar: usize) -> usize {
    s.chars().take(nchar).map(|ch| ch.len_utf8()).sum()
}

fn split_at_char_index(s: &str, at: usize) -> (&str, &str) {
    let at = byte_index_from_char_index(s, at);
    s.split_at(at)
}

/// ```text
/// zipper_split("hello world", 3)
///                  ^--------- at = 3
///
/// Returns ("hel", "l", "o world")
/// ```
fn zipper_split(s: &str, at: usize) -> (&str, &str, &str) {
    let (first, rest) = split_at_char_index(s, at);
    let (elem, last) = split_at_char_index(rest, 1);
    (first, elem, last)
}

