//! # fastup: FAncy STatus markUP language
//!
//! ```fastup
//! hello [cceeff: world(0000ff: yes) or {\[n\]o}] <date|hello|world> up
//! ```
//!
//! becomes
//!
//! <span style="font-family: monospace">
//! hello <span style="background-color: #cceeff">world<span style="color: #0000ff">yes</span> or <b>[n]o</b></span> <span style="background-color: #ff0000"><span style="color: #ffffff"><b>&lt;Error: date accepts no argument&gt;</b></span></span> up
//! </span>

extern crate color;
mod parser;
pub use parser::*;

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn generate_html() {
        let src = r"hello [cceeff: world(0000ff: yes) or {\[n\]o}] <date|hello|world> up";
        let doc = parse(src).unwrap();
        println!("{:?}", doc);
    }
}

