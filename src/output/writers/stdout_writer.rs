
use super::Writer;

#[derive(Default)]
pub struct StdoutWriter;

impl Writer for StdoutWriter {
    fn write(&self, what: &str) {
        print!("{}", what);
    }
}

