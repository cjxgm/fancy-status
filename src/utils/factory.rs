/// A factory generator.
///
/// ```rust,ignore
/// factory! {
///     /// Documentation for the `Base`.
///     pub trait Base {
///         fn hello(&self);
///     }
///
///     /// Documentation for the `make` function.
///     make {
///         foo;
///         bar;
///         baz;
///     }
/// }
/// ```
///
/// TODO: explain the code.
#[macro_export]
macro_rules! factory {
    (
        $(#[$base_attr:meta])*
        pub trait $base:ident {
            $( $decl:tt )*
        }

        $(#[$make_attr:meta])*
        make {
            $(
                $maker:ident;
            )*
        }
    ) => {
        $(
            pub mod $maker;
        )*

        $(#[$base_attr])*
        pub trait $base {
            $( $decl )*
        }

        $(#[$make_attr])*
        pub fn make(name: &str) -> Option<Box<$base>> {
            match name {
                $(
                    stringify!($maker) => Some(Box::new($maker::$base::default())),
                )*
                _ => None,
            }
        }
    }
}
