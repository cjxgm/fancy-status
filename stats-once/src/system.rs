use sysinfo::{System, SystemExt};
use std::time::Duration;
use std::thread::sleep;

/// In milliseconds.
pub const STATS_HOLD_DURATION: u64 = 100;

lazy_static! {
    /// Gather stats after holding for `STATS_HOLD_DURATION` milliseconds.
    pub static ref SYSTEM: System = {
        let mut sys = System::new();
        sleep(Duration::from_millis(STATS_HOLD_DURATION));
        sys.refresh_system();
        sys
    };
}

