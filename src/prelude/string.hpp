#pragma once
#include "span.hpp"
#include "types.hpp"

namespace fancy_status
{
    inline namespace prelude
    {
        using String_View = Span<char const, true>;
        using String_Slice = Span<char, true>;

        using Buffer_View = Span<byte const>;
        using Buffer_Slice = Span<byte>;

        auto zstring_to_slice(char* x) noexcept -> String_Slice;
        auto zstring_to_view(char const* x) noexcept -> String_View;

        // Copy src to dst. If src is longer than dst, it will be truncated.
        // It is an ERROR if src and dst overlaps.
        // Returns the number of chars/bytes copied.
        auto copy(String_View src, String_Slice dst) noexcept -> size_t;
        auto copy(Buffer_View src, Buffer_Slice dst) noexcept -> size_t;
    }
}

