#pragma once
#include "utility.hpp"
#include "string.hpp"

namespace fancy_status::printer
{
    // Convert x to string and write to the front of buf.
    // Returns the remaining space of buf, i.e. the back of buf that can still be written to.
    template <class T>
    auto do_print_into(String_Slice buf, T x) noexcept -> String_Slice;
}

namespace fancy_status
{
    inline namespace prelude
    {
        namespace print_detail
        {
            extern thread_local String_Slice scratch_buffer;
        }

        // All errors in the dump*() are silently ignored.
        auto dump(String_View x) noexcept -> void;
        auto dump_stderr(String_View x) noexcept -> void;

        // Print arguments into the front of a string buffer.
        // Returns the remaining space of buf, i.e. the back of buf that can still be written to,
        // so that you can chain printing functions.
        template <class... Args>
        auto chain_print_into(String_Slice buf, Args&&... xs) noexcept -> String_Slice
        {
            ((buf = printer::do_print_into(buf, forward<Args>(xs))), ...);
            return buf;
        }

        // Print arguments into the front of a string buffer.
        // Returns the part that has been written to.
        template <class... Args>
        auto print_into(String_Slice buf, Args&&... xs) noexcept -> String_Slice
        {
            auto remaining = chain_print_into(buf, forward<Args>(xs)...);
            return buf.reset_last(remaining.begin());
        }

        template <class... Args>
        auto print(Args&&... xs) noexcept -> void
        {
            dump(print_into(print_detail::scratch_buffer, forward<Args>(xs)...));
        }

        template <class... Args>
        auto print_stderr(Args&&... xs) noexcept -> void
        {
            dump_stderr(print_into(print_detail::scratch_buffer, forward<Args>(xs)...));
        }
    }
}

