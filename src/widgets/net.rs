use color::Colorf32;
use fastup::{self, Node, parse_for_first_node, parse_color};
use stats_once::{NET_DOWNLOAD_SPEED, NET_UPLOAD_SPEED};
use super::error_node;
use std::mem::swap;

#[derive(Debug, Copy, Clone, Default)]
pub struct Widget;

impl super::Widget for Widget {
    fn expand(&self, args: Vec<String>) -> Node {
        match args.len() {
            4 | 6 => node_from_args(args),
            8 => node_from_args(args),     // debug only
            _ => error_node("(ccdd44: net) takes 4 or 6 arguments \\(speed in KiB\\): color-cold\\|color-hot\\|lo-speed\\|hi-speed{\\|lo-speed-upload\\|hi-speed-upload}"),
        }
    }
}

fn node_from_args(args: Vec<String>) -> Node {
    try_node_from_args(args).unwrap_or_else(|e| error_node(&e))
}

fn try_node_from_args(args: Vec<String>) -> Result<Node, String> {
    assert!((args.len() == 4 || args.len() == 6) || args.len() == 8);

    let cold = parse_color(&args[0]).map_err(escape_fastup)?;
    let hot = parse_color(&args[1]).map_err(escape_fastup)?;

    let parse_speed = |i| -> Result<Option<u64>, String> {
        if args.len() <= i || args[i].trim().len() == 0 {
            Ok(None)
        } else {
            let speed: f64 = args[i].parse().map_err(escape_fastup)?;
            if speed < 0.0 {
                Err(format!("speed cannot be a negative number, but got {}", speed))
            } else {
                Ok(Some((speed * 1024.0) as u64))
            }
        }
    };

    let lo_speed_down = parse_speed(2)?.unwrap();
    let hi_speed_down = parse_speed(3)?.unwrap();
    let lo_speed_up = parse_speed(4)?.unwrap_or(lo_speed_down);
    let hi_speed_up = parse_speed(5)?.unwrap_or(hi_speed_down);
    let speed_down = parse_speed(6)?.unwrap_or(*NET_DOWNLOAD_SPEED);
    let speed_up   = parse_speed(7)?.unwrap_or(*NET_UPLOAD_SPEED);

    Ok(
        net_node(
            cold.into(),
            hot.into(),
            lo_speed_down,
            hi_speed_down,
            lo_speed_up,
            hi_speed_up,
            speed_down,
            speed_up,
        )
    )
}

fn net_node(
    cold: Colorf32,
    hot: Colorf32,
    lo_speed_down: u64,
    hi_speed_down: u64,
    lo_speed_up: u64,
    hi_speed_up: u64,
    speed_down: u64,
    speed_up: u64,
) -> Node
{
    const BG_LIGHTNESS: f32 = 0.15;
    const FG_LIGHTNESS: f32 = 0.4;
    const IDLE_LIGHTNESS: f32 = 0.2;
    const FAST_LIGHTNESS_BOOST: f32 = 0.3;
    const PROGRESS_INDICATORS: &'static str = "▁▂▃▄▅▆▇";

    let ratio_of = |x, lo, hi| nan_to_zero(unmix(x as f32, (lo as f32, hi as f32)));
    let ratio_down = ratio_of(speed_down, lo_speed_down, hi_speed_down);
    let ratio_up = ratio_of(speed_up, lo_speed_up, hi_speed_up);

    let (fast_speed, fast_ratio, fast_boost, boost_down, boost_up) = {
        if speed_down == 0 && speed_up == 0 {
            (0, 0.0, 0.0, 0.0, 0.0)
        } else if (ratio_up - ratio_down).abs() < 1e-5 {
            (speed_down, ratio_down, FAST_LIGHTNESS_BOOST, FAST_LIGHTNESS_BOOST, 0.0)
        } else if ratio_up > ratio_down {
            (speed_up, ratio_up, FAST_LIGHTNESS_BOOST, 0.0, FAST_LIGHTNESS_BOOST)
        } else {
            (speed_down, ratio_down, FAST_LIGHTNESS_BOOST, FAST_LIGHTNESS_BOOST, 0.0)
        }
    };
    let fast_ratio = fast_ratio.max(0.0).min(1.0);
    let ratio_down = ratio_down.max(0.0).min(1.0);
    let ratio_up   = ratio_up  .max(0.0).min(1.0);

    let bg_cold = cold.set_lightness(BG_LIGHTNESS);
    let bg_hot = hot.set_lightness(BG_LIGHTNESS);
    let bg = bg_cold.mix(bg_hot, fast_ratio).clamp_to_888();

    let idle_for = |ratio: f32, boost: f32| {
        let idle_cold = cold.set_lightness(IDLE_LIGHTNESS + boost);
        let idle_hot = hot.set_lightness(IDLE_LIGHTNESS + boost);
        idle_cold.mix(idle_hot, ratio).clamp_to_888()
    };

    let fg_for = |ratio: f32, boost: f32| {
        if ratio == 0.0 {
            idle_for(ratio, boost)
        } else {
            let fg_cold = cold.set_lightness(FG_LIGHTNESS + boost);
            let fg_hot = hot.set_lightness(FG_LIGHTNESS + boost);
            fg_cold.mix(fg_hot, ratio).clamp_to_888()
        }
    };

    let indicator_for = |ratio: f32, reverse: bool| {
        let len = PROGRESS_INDICATORS.chars().count() as f32;
        let mut i = (len * ratio).floor();
        if reverse { i = len - 1.0 - i }
        let i = i.min(len - 1.0).max(0.0) as usize;
        PROGRESS_INDICATORS.chars().nth(i).unwrap()
    };

    let styled_indicator_for = |ratio: f32, boost: f32, upside_down: bool| {
        let mut fg = fg_for(ratio, boost);
        let mut bg = bg;
        if upside_down { swap(&mut fg, &mut bg) }
        let indicator = indicator_for(ratio, upside_down);
        format!("[{}:({}:{})]", bg, fg, indicator)
    };

    let indicator_down = styled_indicator_for(ratio_down, boost_down, true);
    let indicator_up   = styled_indicator_for(ratio_up, boost_up, false);
    let fast_text = {
        let fg_fast = fg_for(fast_ratio, fast_boost);
        let text = format_byte_size(fast_speed);
        let text = format!("{:>5}", text);
        let text = escape_fastup(text);
        format!("({}:{})", fg_fast, text)
    };

    let node = format!("[{}: \\ {}{}{} ]", bg, indicator_up, indicator_down, fast_text);
    parse_for_first_node(&node).unwrap()
}

fn escape_fastup<T>(input: T) -> String
    where T: ToString
{
    fastup::escape_for_text(&input.to_string())
}

/// Format byte size into short and human readable form.
///
/// Shortness is the most important considering factor here.
/// The largest unit is 'G'.
//
//   0B
//  10B
// 100B
// 999B
// 0.9K
// 9.0K
//  10K
// 100K
// ...
//
// NOTE: There are NO spaces in front of the result.
fn format_byte_size(size: u64) -> String {
    let (size, unit) = find_byte_size_unit(size);
    if unit == 'B' {
        format!("{}{}", size.floor(), unit)
    } else {
        if size < 10.0 {
            format!("{:.1}{}", (size * 10.0).floor() / 10.0, unit)
        } else {
            format!("{}{}", size.floor(), unit)
        }
    }
}

fn find_byte_size_unit(size: u64) -> (f64, char) {
    const UNITS: &'static str = "BKM";

    let mut size = size as f64;
    for unit in UNITS.chars() {
        if size > 999.9 {
            size /= 1024.0;
        } else {
            return (size, unit);
        }
    }

    (size, 'G')
}

fn unmix(x: f32, range: (f32, f32)) -> f32 {
    let (a, b) = range;
    (x - a) / (b - a)
}

fn nan_to_zero(x: f32) -> f32 {
    if x.is_nan() { 0.0 } else { x }
}

