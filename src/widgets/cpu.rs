use color::Colorf32;
use fastup::{self, Node, parse_for_first_node, parse_color};
use sysinfo::System;
use super::error_node;
use std::time::Duration;
use std::thread::sleep;

#[derive(Debug, Copy, Clone, Default)]
pub struct Widget;

impl super::Widget for Widget {
    fn expand(&self, args: Vec<String>) -> Node {
        match args.len() {
            2 => node_from_args(args),
            _ => error_node("(ccdd44: cpu) takes 2 arguments: color-cold\\|color-hot"),
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

    Ok(cpu_node(cold.into(), hot.into()))
}

fn cpu_node(cold: Colorf32, hot: Colorf32) -> Node {
    const BG_LIGHTNESS: f32 = 0.15;
    const FG_LIGHTNESS: f32 = 0.7;
    const IDLE_LIGHTNESS: f32 = 0.3;
    const IDLE_THRESHOLD: f32 = 0.05;
    const PROGRESS_INDICATOR: &'static str = "▁▂▃▄▅▆▇";

    let cpu_total = *CPU_TOTAL;
    let cpu_usages = &CPU_USAGES;

    let bg_cold = cold.set_lightness(BG_LIGHTNESS);
    let bg_hot = hot.set_lightness(BG_LIGHTNESS);
    let bg = bg_cold.mix(bg_hot, cpu_total).clamp_to_888();

    let idle_cold = cold.set_lightness(IDLE_LIGHTNESS);
    let idle_hot = hot.set_lightness(IDLE_LIGHTNESS);
    let idle = idle_cold.mix(idle_hot, cpu_total).clamp_to_888();

    let fg_cold = cold.set_lightness(FG_LIGHTNESS);
    let fg_hot = hot.set_lightness(FG_LIGHTNESS);
    let make_fg_color = |usage: f32| {
        if usage <= IDLE_THRESHOLD {
            idle
        } else {
            let usage = (usage - IDLE_THRESHOLD) / (1.0 - IDLE_THRESHOLD);
            fg_cold.mix(fg_hot, usage).clamp_to_888()
        }
    };
    let make_cpu_indicator = |usage: f32| {
        let len = PROGRESS_INDICATOR.chars().count() as f32;
        let i = (len * usage).min(len - 1.0).max(0.0) as usize;
        PROGRESS_INDICATOR.chars().nth(i).unwrap()
    };

    let cpu_usages = cpu_usages.iter()
        .map(|&usage| format!("({}:{})", make_fg_color(usage), make_cpu_indicator(usage)))
        .collect::<String>();
    let total_usage = {
        let color = make_fg_color(cpu_total);
        let percent = (100.0 * cpu_total) as i32;
        let percent = format!("{:>3}%", percent);
        let percent = escape_fastup(percent);
        format!("({}:{})", color, percent)
    };

    let node = format!("[{}: \\ {}{} ]", bg, cpu_usages, total_usage);
    parse_for_first_node(&node).unwrap()
}

lazy_static! {
    static ref _SYSTEM: System = {
        let mut sys = System::new();
        sleep(Duration::from_millis(100));
        sys.refresh_system();
        sys
    };

    /// ```rust,ignore
    /// assert!(0.0 <= cpu_total && cpu_total <= 1.0);
    /// // unstable feature awaits: assert!((0.0...1.0).contains(cpu_total));
    /// ```
    static ref CPU_TOTAL: f32 = {
        let p = _SYSTEM.get_processor_list();
        let usage = p[0].get_cpu_usage();       // p[0] is total cpu usage
        normalize_float(usage)
    };

    /// ```rust,ignore
    /// for usage in cpu_usages {
    ///     assert!(0.0 <= usage && usage <= 1.0);
    ///     // unstable feature awaits: assert!((0.0...1.0).contains(usage));
    /// }
    /// ```
    static ref CPU_USAGES: Vec<f32> = {
        let p = _SYSTEM.get_processor_list();
        p.iter()
            .skip(1)                            // p[1..] is individual cpu usage by core
            .map(|x| x.get_cpu_usage())
            .map(normalize_float)
            .collect()
    };
}

fn escape_fastup<T>(input: T) -> String
    where T: ToString
{
    fastup::escape_for_text(&input.to_string())
}

fn normalize_float(x: f32) -> f32 {
    match x {
        x if x < 0.0 => 0.0,
        x if x > 1.0 => 1.0,
        x if x.is_nan() => 0.0,
        _ => x,
    }
}

