use color::Colorf32;
use fastup::{self, Node, parse_for_first_node, parse_color};
use stats_once::{CPU_USAGE_TOTAL, CPU_USAGE_PER_CORE};
use super::error_node;

#[derive(Debug, Copy, Clone, Default)]
pub struct Widget;

impl super::Widget for Widget {
    fn expand(&self, args: Vec<String>) -> Node {
        match args.len() {
            2 => node_from_args(args),
            x if x > 2 => node_from_args(args),     // debug only
            _ => error_node("(ccdd44: cpu) takes 2 arguments: color-cold\\|color-hot"),
        }
    }
}

fn node_from_args(args: Vec<String>) -> Node {
    try_node_from_args(args).unwrap_or_else(|e| error_node(&e))
}

fn try_node_from_args(args: Vec<String>) -> Result<Node, String> {
    assert!(args.len() >= 2);

    let cold = parse_color(&args[0]).map_err(escape_fastup)?;
    let hot = parse_color(&args[1]).map_err(escape_fastup)?;
    if args.len() == 2 {
        Ok(cpu_node(cold.into(),
                    hot.into(),
                    *CPU_USAGE_TOTAL,
                    &CPU_USAGE_PER_CORE))
    } else {
        let usages = args.into_iter()
            .skip(2)
            .map(|arg| arg.parse::<f32>().map_err(escape_fastup))
            .fold(Ok::<_, String>(Vec::new()),
                  |usages, percent| match usages {
                      Ok(mut usages) => {
                          let percent = percent?;
                          usages.push(percent / 100.0);
                          Ok(usages)
                      }
                      Err(_) => usages,
                  })?;

        let total: f32 = usages.iter().sum();
        let len = usages.len() as f32;
        let total = total / len;

        Ok(cpu_node(cold.into(), hot.into(), total, &usages))
    }
}

fn cpu_node(cold: Colorf32, hot: Colorf32, cpu_total: f32, cpu_usages: &[f32]) -> Node {
    const BG_LIGHTNESS: f32 = 0.15;
    const FG_LIGHTNESS: f32 = 0.7;
    const IDLE_LIGHTNESS: f32 = 0.3;
    const IDLE_THRESHOLD: f32 = 0.05;
    const PROGRESS_INDICATORS: &'static str = "▁▂▃▄▅▆▇";

    let bg_cold = cold.set_lightness(BG_LIGHTNESS);
    let bg_hot = hot.set_lightness(BG_LIGHTNESS);
    let bg = bg_cold.mix(bg_hot, cpu_total).clamp_to_888();

    let idle_cold = cold.set_lightness(IDLE_LIGHTNESS);
    let idle_hot = hot.set_lightness(IDLE_LIGHTNESS);
    let idle = idle_cold.mix(idle_hot, cpu_total).clamp_to_888();

    let fg_cold = cold.set_lightness(FG_LIGHTNESS);
    let fg_hot = hot.set_lightness(FG_LIGHTNESS);
    let fg_for = |usage: f32| if usage <= IDLE_THRESHOLD {
        idle
    } else {
        let usage = (usage - IDLE_THRESHOLD) / (1.0 - IDLE_THRESHOLD);
        fg_cold.mix(fg_hot, usage).clamp_to_888()
    };

    let indicator_for = |usage: f32| {
        let len = PROGRESS_INDICATORS.chars().count() as f32;
        let i = (len * usage).min(len - 1.0).max(0.0) as usize;
        PROGRESS_INDICATORS.chars().nth(i).unwrap()
    };

    let cpu_usages = cpu_usages.iter()
        .map(|&usage| format!("({}:{})", fg_for(usage), indicator_for(usage)))
        .collect::<String>();
    let total_usage = {
        let color = fg_for(cpu_total);
        let percent = (100.0 * cpu_total) as i32;
        let percent = format!("{:>3}%", percent);
        let percent = escape_fastup(percent);
        format!("({}:{})", color, percent)
    };

    let node = format!("[{}: \\ {}{} ]", bg, cpu_usages, total_usage);
    parse_for_first_node(&node).unwrap()
}

fn escape_fastup<T>(input: T) -> String
    where T: ToString
{
    fastup::escape_for_text(&input.to_string())
}

