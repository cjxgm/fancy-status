//! # Entry point to the application.
//!
//! Gather and parse command line arguments and configuration files, pass
//! them into the application and do some post-exit clean up.

#[macro_use]
extern crate clap;
#[macro_use]
extern crate fancy_status as fast;

mod app;

use fast::utils::*;
use std::process;

fn main() {
    parse_args();
    if let Err(e) = app::run() {
        errln!("Error: {:?}", e);
        process::exit(1);
    }
}

fn parse_args() {
    let args = clap_app! {
        @app (app_from_crate!())
        (@arg EXPRESSION: -e --expression +takes_value "Evaluates EXPRESSION")
        (@arg FILE: "Reads EXPRESSION from FILE and evaluates it.")
        (@arg format: -f --format +takes_value default_value("ansi") possible_values(&["ansi", "tmux"]) "Sets output format")
        (@arg debug: -d ... "Sets the level of debugging information")
        (@arg verbose: -v ... "Sets the level of verbosity")
    }.get_matches();
}

