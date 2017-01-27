
use fast::output::{StdoutWriter, Writer};
use fast::render_status;

#[derive(Debug)]
pub enum Error {
    Todo,
}

pub fn run() -> Result<(), Error> {
    StdoutWriter::default().write("Hello world");
    StdoutWriter::default().write(&render_status("hello <time> {world}", "dump").unwrap());
    StdoutWriter::default().write(&render_status("hello <time> {world}", "ansi").unwrap());
    StdoutWriter::default().write(&render_status("<gradient|50|ff0000|00ffff>", "ansi").unwrap());
    StdoutWriter::default().write(&render_status("<cpu|00ff88|ff3322>", "ansi").unwrap());
    Err(Error::Todo)
}

