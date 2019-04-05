use crate::system::{SYSTEM, STATS_HOLD_DURATION};
use sysinfo::SystemExt;
use sysinfo::NetworkExt;

lazy_static! {
    /// ```rust
    /// # use stats_once::net::*;
    /// assert!(*IN_SPEED >= 0);
    /// ```
    pub static ref IN_SPEED: u64 = {
        SYSTEM.get_network().get_income() * 1000 / STATS_HOLD_DURATION
    };

    /// ```rust
    /// # use stats_once::net::*;
    /// assert!(*OUT_SPEED >= 0);
    /// ```
    pub static ref OUT_SPEED: u64 = {
        SYSTEM.get_network().get_outcome() * 1000 / STATS_HOLD_DURATION
    };
}

