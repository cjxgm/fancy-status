use system::SYSTEM;
use sysinfo::SystemExt;
use sysinfo::ProcessorExt;

lazy_static! {
    /// ```rust
    /// # use stats_once::cpu::*;
    /// assert!(0.0 <= *TOTAL && *TOTAL <= 1.0);
    /// // unstable feature awaits: assert!((0.0...1.0).contains(TOTAL));
    /// ```
    pub static ref TOTAL: f32 = {
        let p = SYSTEM.get_processor_list();
        let usage = p[0].get_cpu_usage();       // p[0] is total cpu usage
        normalize_float(usage)
    };

    /// ```rust
    /// # use stats_once::cpu::*;
    /// for &usage in PER_CORE.iter() {
    ///     assert!(0.0 <= usage && usage <= 1.0);
    ///     // unstable feature awaits: assert!((0.0...1.0).contains(usage));
    /// }
    /// ```
    pub static ref PER_CORE: Vec<f32> = {
        let p = SYSTEM.get_processor_list();
        p.iter()
            .skip(1)                            // p[1..] is individual cpu usage by core
            .map(|x| x.get_cpu_usage())
            .map(normalize_float)
            .collect()
    };
}

fn normalize_float(x: f32) -> f32 {
    match x {
        x if x < 0.0 => 0.0,
        x if x > 1.0 => 1.0,
        x if x.is_nan() => 0.0,
        _ => x,
    }
}

