
use fast::output::{StdoutWriter, Writer};
use fast::render_status;

#[derive(Debug)]
pub enum Error {
    Todo,
}

pub fn run() -> Result<(), Error> {
    StdoutWriter::default().write("Hello world");
    StdoutWriter::default().write(&render_status("hello <test> {world}", "dump").unwrap());
    StdoutWriter::default().write(&render_status("hello <test> {world}", "ansi").unwrap());
    Err(Error::Todo)
}

