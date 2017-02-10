use time::{self, Tm as Time};
use fastup::{parse_for_first_node, Node};
use super::error_node;

#[derive(Debug, Copy, Clone, Default)]
pub struct Widget;

impl super::Widget for Widget {
    fn expand(&self, args: Vec<String>) -> Node {
        match args.len() {
            0 => node_from_time(time::now()),
            _ => error_node("(ccdd44: time) takes no argument"),
        }
    }
}

fn node_from_time(t: Time) -> Node {
    let source = format!("\
        (000000:\
            (88ff00:{:04}-{:02}-{:02})\
            (ff8800:▐)[ff8800:{}](ff8800:▌)\
            (33ccff:{:02}:{:02}:{:02})\
        )",
        t.tm_year + 1900,
        t.tm_mon + 1,
        t.tm_mday,
        stylised_weekday(t.tm_wday),
        t.tm_hour,
        t.tm_min,
        t.tm_sec);
    parse_for_first_node(&source).unwrap()
}

const WEEKDAYS: &'static str = "×12345·";
fn stylised_weekday(weekday: i32) -> char {
    WEEKDAYS.chars().nth(weekday as usize).unwrap()
}

#[test]
fn test_stylised_weekdays() {
    assert_eq!(stylised_weekday(0), '×');
    assert_eq!(stylised_weekday(1), '1');
    assert_eq!(stylised_weekday(2), '2');
    assert_eq!(stylised_weekday(3), '3');
    assert_eq!(stylised_weekday(4), '4');
    assert_eq!(stylised_weekday(5), '5');
    assert_eq!(stylised_weekday(6), '·');
}

#[test]
#[should_panic]
fn stylised_weekdays_panic_lower_bound() {
    stylised_weekday(-1);
}

#[test]
#[should_panic]
fn stylised_weekdays_panic_upper_bound() {
    stylised_weekday(7);
}

