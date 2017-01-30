//! Convert byte size to human readable format, like `1024` to `1.0 KiB`.
//!
//! ## Implementation Details
//!
//! The conversion utilizes a hack:
//!
//! | `leading_zeros()` | Size Unit   |
//! |-------------------|-------------|
//! | `x if x > 53`     |   B (Octet) |
//! | `x if x > 43`     | KiB         |
//! | `x if x > 33`     | MiB         |
//! | `x if x > 23`     | GiB         |
//! | `x if x > 13`     | TiB         |
//! | `x if x > 3`      | PiB         |
//! | `_`               | EiB         |
use std::fmt;

/// Representing byte units, from B, KiB, MiB up to EiB.
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
pub enum Unit {
    B,
    KiB,
    MiB,
    GiB,
    TiB,
    PiB,
    EiB,
}

/// Wraps your value for formatting.
///
/// ```
/// # use fancy_status::utils::byte_size::ByteSize;
/// assert_eq!(format!("{}", ByteSize(1024_u32)), "1.0 KiB");
/// ```
#[derive(Debug, Clone, Copy)]
pub struct ByteSize<T>(pub T) where T: Into<u64> + Copy;

impl fmt::Display for Unit {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{:?}", self)
    }
}

impl From<u8> for Unit {
    /// Convert magnitude to unit.
    ///
    /// | Magnitude | `Unit`      |
    /// |-----------|-------------|
    /// | `0`       | `Unit::B`   |
    /// | `1`       | `Unit::KiB` |
    /// | `2`       | `Unit::MiB` |
    /// | `3`       | `Unit::GiB` |
    /// | `4`       | `Unit::TiB` |
    /// | `5`       | `Unit::PiB` |
    /// | `6`       | `Unit::EiB` |
    /// | Other     | `panic`     |
    fn from(num: u8) -> Self {
        match num {
            0 => Unit::B,
            1 => Unit::KiB,
            2 => Unit::MiB,
            3 => Unit::GiB,
            4 => Unit::TiB,
            5 => Unit::PiB,
            6 => Unit::EiB,
            _ => unreachable!(),
        }
    }
}

impl<T> fmt::Display for ByteSize<T>
    where T: Into<u64> + Copy
{
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let (x, unit) = unitize(self.0.into());
        match unit {
            Unit::B => write!(f, "{:.0} {}", x, unit),
            _ => write!(f, "{:.1} {}", x, unit),
        }
    }
}

/// `unitize(1024)` gives you `(1.0_f32, Unit::KiB)`.
pub fn unitize(x: u64) -> (f32, Unit) {
    let mag = size_magnitude(x);
    let x = (x as f32) / ((1_u64 << (mag * 10)) as f32);
    (x, mag.into())
}

fn size_magnitude(x: u64) -> u8 {
    if x < (1 << 10) {
        0
    } else {
        ((63 - x.leading_zeros()) / 10) as u8
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn unit_sequencing() {
        assert_eq!(Unit::B as u8, 0);
        assert_eq!(Unit::KiB as u8, 1);
        assert_eq!(Unit::MiB as u8, 2);
        assert_eq!(Unit::GiB as u8, 3);
        assert_eq!(Unit::TiB as u8, 4);
        assert_eq!(Unit::PiB as u8, 5);
        assert_eq!(Unit::EiB as u8, 6);
    }

    #[test]
    fn unit_from_magnitude() {
        assert_eq!(Unit::B, 0.into());
        assert_eq!(Unit::KiB, 1.into());
        assert_eq!(Unit::MiB, 2.into());
        assert_eq!(Unit::GiB, 3.into());
        assert_eq!(Unit::TiB, 4.into());
        assert_eq!(Unit::PiB, 5.into());
        assert_eq!(Unit::EiB, 6.into());
    }

    #[test]
    #[should_panic]
    fn unit_magnitude_too_big() {
        Unit::from(7);
    }

    #[test]
    fn unit_stringify() {
        assert_eq!(Unit::B.to_string(), "B");
        assert_eq!(Unit::KiB.to_string(), "KiB");
        assert_eq!(Unit::MiB.to_string(), "MiB");
        assert_eq!(Unit::GiB.to_string(), "GiB");
        assert_eq!(Unit::TiB.to_string(), "TiB");
        assert_eq!(Unit::PiB.to_string(), "PiB");
        assert_eq!(Unit::EiB.to_string(), "EiB");
    }

    #[test]
    fn unit_ordering() {
        assert!(Unit::B < Unit::KiB);
        assert!(Unit::KiB < Unit::MiB);
        assert!(Unit::MiB < Unit::GiB);
        assert!(Unit::GiB < Unit::TiB);
        assert!(Unit::TiB < Unit::PiB);
        assert!(Unit::PiB < Unit::EiB);
    }

    #[test]
    fn byte_size() {
        assert_eq!(ByteSize(0_u8).to_string(), "0 B");
        assert_eq!(ByteSize(27_u16).to_string(), "27 B");
        assert_eq!(ByteSize(999_u32).to_string(), "999 B");
        assert_eq!(ByteSize(1_000_u64).to_string(), "1000 B");
        assert_eq!(ByteSize(1_023_u64).to_string(), "1023 B");
        assert_eq!(ByteSize(1_024_u64).to_string(), "1.0 KiB");
        assert_eq!(ByteSize(1_728_u64).to_string(), "1.7 KiB");
        assert_eq!(ByteSize(110_592_u64).to_string(), "108.0 KiB");
        assert_eq!(ByteSize(7_077_888_u64).to_string(), "6.8 MiB");
        assert_eq!(ByteSize(452_984_832_u64).to_string(), "432.0 MiB");
        assert_eq!(ByteSize(28_991_029_248_u64).to_string(), "27.0 GiB");
        assert_eq!(ByteSize(1_855_425_871_872_u64).to_string(), "1.7 TiB");
        assert_eq!(ByteSize(9_223_372_036_854_775_807_u64).to_string(), "8.0 EiB");
    }
}

