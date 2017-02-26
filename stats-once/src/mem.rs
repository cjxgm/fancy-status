use system::SYSTEM;
use sysinfo::SystemExt;

lazy_static! {
    /// ```rust
    /// # use stats_once::mem::*;
    /// assert!(*TOTAL >= 0);
    /// ```
    pub static ref TOTAL: i32 = {
        mib_from_kib(SYSTEM.get_total_memory())
    };

    /// ```rust
    /// # use stats_once::mem::*;
    /// assert!(*USED >= 0);
    /// ```
    pub static ref USED: i32 = {
        mib_from_kib(SYSTEM.get_used_memory())
    };
}

/// Convert byte size from KiB to MiB.
fn mib_from_kib(x: u64) -> i32 {
    (x / (1 << 10)) as i32
}

