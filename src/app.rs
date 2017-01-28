
use fast::output::{StdoutWriter, Writer};
use fast::render_status;

pub type Result<T> = ::std::result::Result<T, String>;

#[derive(Debug)]
pub struct Configuration {
    pub expression: String,
    pub renderer_name: String,
}

pub fn run(config: &Configuration) -> Result<()> {
    let status = render_status(&config.expression, &config.renderer_name)?;
    StdoutWriter::default().write(&status);
    Ok(())
}

