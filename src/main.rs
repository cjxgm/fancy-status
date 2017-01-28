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
use std::path::Path;
use std::fs::File;
use std::io::Read;
use app::{Configuration, run};

type Result<T> = ::std::result::Result<T, String>;

fn main() {
    if let Err(err) = try_main() {
        errln!("{}", err);
        exit(1);
    }
}

fn try_main() -> Result<()> {
    let config = parse_args()?;
    run(&config)
}

fn parse_args() -> Result<Configuration> {
    let args = clap_app! {
        @app (app_from_crate!())
        (@arg EXPRESSION: default_value("<time>") "Evaluates Fastup EXPRESSION if no <FILE> is given")
        (@arg FILE: -f --file +takes_value "Reads Fastup EXPRESSION from FILE and evaluates it")
        (@arg format: -o --output +takes_value default_value("ansi") possible_values(&["ansi", "tmux", "html", "dump"]) "Sets output format")
    }.get_matches();

    Ok(Configuration {
        expression: match args.value_of("FILE") {
            Some(path) => read_whole_file(path)?,
            None => args.value_of("EXPRESSION").unwrap().into(),    // `EXPRESSION` has default value, just unwrap
        },
        renderer_name: args.value_of("format").unwrap().into(),     // `format` has default value, just unwrap
    })
}

fn read_whole_file<T>(path: T) -> Result<String>
    where T: AsRef<Path>
{
    let mut content = String::new();
    {
        let mut file = File::open(path).map_err(|e| e.to_string())?;
        file.read_to_string(&mut content).map_err(|e| e.to_string())?;
    }
    Ok(content)
}

