#pragma once
#include "string.hpp"
#include "print.hpp"

namespace fancy_status
{
    inline namespace prelude
    {
        namespace assert_detail
        {
            extern thread_local String_Slice scratch_buffer;
        }

        auto show_error(String_View description) noexcept -> void;

        auto show_assertion_error(
            String_View func,
            String_View file,
            int line,
            String_View description
        ) noexcept -> void;

        [[noreturn]] auto abort() noexcept -> void;
    }
}

// Support disabling assertions
#ifdef FANCY_STATUS_DISABLE_ASSERTIONS
    #define FANCY_STATUS_ASSERTION_PREDICATE(...)     true
#else
    #define FANCY_STATUS_ASSERTION_PREDICATE(...)     static_cast<bool>(__VA_ARGS__)
#endif

// Support hiding the filename from assertion error messages.
#ifdef FANCY_STATUS_HIDE_FILENAME_IN_ASSERTIONS
    #define FANCY_STATUS_SHOW_ASSERTION_ERROR(...) \
        ::fancy_status::prelude::show_assertion_error( \
            ::fancy_status::prelude::zstring_to_view(__func__), \
            {}, \
            0, \
            (__VA_ARGS__) \
        )
#else
    #define FANCY_STATUS_SHOW_ASSERTION_ERROR(...) \
        ::fancy_status::prelude::show_assertion_error( \
            ::fancy_status::prelude::zstring_to_view(__func__), \
            __FILE__, \
            __LINE__, \
            (__VA_ARGS__) \
        )
#endif

// FANCY_STATUS_PAUSE(): Pause the execution of current code and go into the debugger.
// FANCY_STATUS_PAUSE() may not work for certain platforms.
// FANCY_STATUS_PAUSE() returns when the user asks to continue in the debugger.
#if defined(FANCY_STATUS_UNDEFINED)
#elif defined(__linux__)
    #define FANCY_STATUS_PAUSE() ({ __asm__("int3"); })
#else
    #define FANCY_STATUS_PAUSE() 0
#endif

// FANCY_STATUS_TRAP(): Stop current code and go into the debugger.
// FANCY_STATUS_TRAP() behaves as [[noreturn]].
#define FANCY_STATUS_TRAP() (\
    FANCY_STATUS_PAUSE(), \
    ::fancy_status::prelude::abort() \
)

// FANCY_STATUS_ABORT_WITH(REASON...): Shows an error message of REASON using print and stop into the debugger.
// FANCY_STATUS_ABORT_WITH(REASON...) is an expression, not a statement.
// FANCY_STATUS_ABORT_WITH(REASON...) behaves as [[noreturn]].
#define FANCY_STATUS_ABORT_WITH(...) ( \
    FANCY_STATUS_SHOW_ASSERTION_ERROR( \
        ::fancy_status::prelude::print_into( \
            ::fancy_status::prelude::assert_detail::scratch_buffer, \
            __VA_ARGS__ \
        ) \
    ), \
    FANCY_STATUS_TRAP(), \
    0 \
)

// FANCY_STATUS_BUG(): Marks certain branch (e.g. in switch case statements) is not possible.
// FANCY_STATUS_BUG() behaves as [[noreturn]].
#define FANCY_STATUS_BUG() \
    FANCY_STATUS_ABORT_WITH(::fancy_status::prelude::String_View{"BUG"})

// FANCY_STATUS_TODO(): Marks certain code not yet implemented.
// FANCY_STATUS_TODO() behaves as [[noreturn]].
#define FANCY_STATUS_TODO() \
    FANCY_STATUS_ABORT_WITH(::fancy_status::prelude::String_View{"TODO"})

// FANCY_STATUS_ASSERT_WITH(TRUTH, REASON...): Error out and show REASON using print if TRUTH is false.
// FANCY_STATUS_ASSERT_WITH(TRUTH, REASON...) is an expression, not a statement.
// FANCY_STATUS_ASSERT_WITH(TRUTH, REASON...) behaves as [[noreturn]].
#define FANCY_STATUS_ASSERT_WITH(TRUTH, ...) ( \
    FANCY_STATUS_ASSERTION_PREDICATE(TRUTH) \
    ? 0 \
    : FANCY_STATUS_ABORT_WITH(__VA_ARGS__) \
)

// FANCY_STATUS_ASSERT(TRUTH): Error out if TRUTH is false.
// FANCY_STATUS_ASSERT(TRUTH) is an expression, not a statement.
// FANCY_STATUS_ASSERT(TRUTH) behaves as [[noreturn]].
#define FANCY_STATUS_ASSERT(TRUTH) \
    FANCY_STATUS_ASSERT_WITH(TRUTH, ::fancy_status::prelude::String_View{"Assertion Failed: " #TRUTH})

