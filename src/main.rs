//! # Entry point to the application.
//!
//! Gather and parse command line arguments and configuration files, pass
//! them into the application and do some post-exit clean up.
//!
//! ## Example Fastup Expressions
//!
//! ```fastup
//! hello <time> {world}
//! <gradient|50|ff0000|00ffff>
//! <cpu|00ff88|ff3322>
//! ```

#[macro_use]
extern crate clap;
#[macro_use]
extern crate fancy_status as fast;

mod app;

use std::process::exit;
use app::{Configuration, run};

fn main() {
    let config = parse_args();
    if let Err(err) = run(&config) {
        errln!("{}", err);
        exit(1);
    }
}

fn parse_args() -> Configuration {
    let args = clap_app! {
        @app (app_from_crate!())
        (@arg EXPRESSION: default_value("<time>") "Evaluates Fastup EXPRESSION if no <FILE> is given")
        (@arg FILE: -f --file +takes_value "Reads Fastup EXPRESSION from FILE and evaluates it")
        (@arg format: -o --output +takes_value default_value("ansi") possible_values(&["ansi", "tmux", "html", "dump"]) "Sets output format")
    }.get_matches();

    Configuration {
        expression: args.value_of("EXPRESSION").unwrap().into(), // TODO: read file
        renderer_name: args.value_of("format").unwrap().into(),
    }
}

