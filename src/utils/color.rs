//! # A strongly restrictive color type.

use std::fmt;

/// It is guaranteed that `T` can be and will always be only either `u8` or `f32`.
/// It can also be guaranteed that `Display` is not and will never be implemented
/// for `Color<f32>`.
/// It can also be guaranteed that arithmetics are not and will never be implemented
/// for `Color<u8>`.
///
/// ```
/// assert_fail_to_compile!(Color(0.0, 0.0, 0.0).to_string());
/// assert_fail_to_compile!(println!("{}", Color(0.0, 0.0, 0.0)));
/// assert_fail_to_compile!(Color(0, 0, 0) + Color(1, 1, 1));
/// ```
#[derive(Debug, PartialEq, Clone, Copy)]
pub struct Color<T>(T, T, T);

/// `Color888` can only do IO (convert to and from strings), and some simple equality tests.
pub type Color888 = Color<u8>;
/// `Colorf32` can only do arithmetics and convert to/from `Color888`.
pub type Colorf32 = Color<f32>;

impl fmt::Display for Color888 {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let Color(r, g, b) = *self;
        write!(f, "{:02x}{:02x}{:02x}", r, g, b)
    }
}

impl Eq for Color888 {}

impl From<Color888> for Colorf32 {
    fn from(color: Color888) -> Self {
        let Color(r, g, b) = color;
        let r = r as f32 / 255_f32;
        let g = g as f32 / 255_f32;
        let b = b as f32 / 255_f32;
        Color(r, g, b)
    }
}

impl Colorf32 {
    fn clamp_to_888(self) -> Color888 {
        let Color(r, g, b) = self.clamp();
        let r = (r * 255_f32).min(255_f32) as u8;
        let g = (g * 255_f32).min(255_f32) as u8;
        let b = (b * 255_f32).min(255_f32) as u8;
        Color(r, g, b)
    }

    fn clamp_with(self, min: f32, max: f32) -> Self {
        let Color(r, g, b) = self;
        let r = r.max(min).min(max);
        let g = g.max(min).min(max);
        let b = b.max(min).min(max);
        Color(r, g, b)
    }

    fn clamp(self) -> Self {
        self.clamp_with(0_f32, 1_f32)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn lossless_conversion() {
        for i in 0..256 {
            let a = i as u8;
            let color = Color(a, a, a);
            let color2 = Colorf32::from(color).clamp_to_888();
            assert_eq!(color, color2);
        }
    }

    #[test]
    fn clampped_down_conversion() {
        let max = Color(255, 255, 255);
        for i in 256..512 {
            let a = i as f32 / 255_f32;
            let color = Color(a, a, a).clamp_to_888();
            assert_eq!(color, max);
        }
    }

    #[test]
    fn clampped_up_conversion() {
        let min = Color(0, 0, 0);
        for i in -256..0 {
            let a = i as f32 / 255_f32;
            let color = Color(a, a, a).clamp_to_888();
            assert_eq!(color, min);
        }
    }

    #[test]
    fn display_color888() {
        assert_eq!(Color(0, 0, 0).to_string(), "000000");
        assert_eq!(Color(255, 255, 255).to_string(), "ffffff");
        assert_eq!(Color(1, 254, 10).to_string(), "01fe0a");
    }
}
