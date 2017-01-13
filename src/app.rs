
#[derive(Debug)]
pub enum Error {
    Todo,
}

pub fn run() -> Result<(), Error> {
    Err(Error::Todo)
}

