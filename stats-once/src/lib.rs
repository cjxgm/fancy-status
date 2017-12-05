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
pub mod mem;
pub mod net;
pub use cpu::{TOTAL as CPU_USAGE_TOTAL, PER_CORE as CPU_USAGE_PER_CORE};
pub use mem::{TOTAL as MEM_TOTAL, USED as MEM_USED};
pub use net::{IN_SPEED as NET_DOWNLOAD_SPEED, OUT_SPEED as NET_UPLOAD_SPEED};

