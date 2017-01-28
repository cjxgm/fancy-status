//! # Output utilities

/// Similar to `println!(...)`, but write to `stderr` instead of `stdout`.
#[macro_export]
macro_rules! errln {
    ($($x:expr),*) => {
        use std::io::Write;
        writeln!(::std::io::stderr(), $($x,)*).unwrap();
    };
}

