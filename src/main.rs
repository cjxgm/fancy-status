//! # Entry point to the application.
//!
//! Gather and parse command line arguments and configuration files, pass
//! them into the application and do some post-exit clean up.

mod app;
#[macro_use]
mod utils;

use utils::*;

fn main() {
    if let Err(e) = app::run() {
        errln!("Error: {:?}", e);
        ::std::process::exit(1);
    }
}

