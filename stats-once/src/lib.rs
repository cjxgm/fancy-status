//! # Global System Statistics
//!
//! Gather system statistics (stats) for only once at the first read of
//! any one of them. You may repeatedly read the stats but it won't be
//! updating after the first read of any one of them.

#[macro_use]
extern crate lazy_static;
extern crate sysinfo;

mod system;
pub mod cpu;
pub use cpu::{TOTAL as CPU_USAGE_TOTAL, PER_CORE as CPU_USAGE_PER_CORE};

