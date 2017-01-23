
use fast::output::{StdoutWriter, Writer};

#[derive(Debug)]
pub enum Error {
    Todo,
}

pub fn run() -> Result<(), Error> {
    StdoutWriter::default().write("Hello world");
    Err(Error::Todo)
}

