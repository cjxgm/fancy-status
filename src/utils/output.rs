//! # Output utilities

pub use std::io::Write;

/// Similar to `println!(...)`, but write to `stderr` instead of `stdout`.
#[macro_export]
macro_rules! errln {
    ($($x:expr),*) => {
        writeln!(::std::io::stderr(), $($x,)*).unwrap();
    };
}

