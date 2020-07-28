#include "string.hpp"
#include "print.hpp"
#include "assert.hpp"
#include <cstring>

namespace fancy_status::printer
{
    namespace
    {
        constexpr auto max_buffer_print_bytes = size_t(20);
    }

    template <>
    auto do_print_into<String_View>(String_Slice buf, String_View x) noexcept -> String_Slice
    {
        return buf.remove_prefix(copy(x, buf));
    }

    template <>
    auto do_print_into<String_Slice>(String_Slice buf, String_Slice x) noexcept -> String_Slice
    {
        return do_print_into<String_View>(buf, x);
    }

    template <>
    auto do_print_into<Buffer_View>(String_Slice buf, Buffer_View x) noexcept -> String_Slice
    {
        auto printing_bytes = x.prefix(min(x.size(), max_buffer_print_bytes));
        for (auto b: printing_bytes)
            buf = chain_print_into(buf, b, ' ');

        auto omitted = (x.size() > max_buffer_print_bytes);
        buf = chain_print_into(
            buf
            , (omitted ? String_View{"... "} : String_View{})
            , String_View{"("}
            , x.size()
            , String_View{" bytes)"}
        );

        return buf;
    }

    template <>
    auto do_print_into<Buffer_Slice>(String_Slice buf, Buffer_Slice x) noexcept -> String_Slice
    {
        return do_print_into<Buffer_View>(buf, x);
    }

    template <>
    auto do_print_into<char*>(String_Slice buf, char* x) noexcept -> String_Slice
    {
        return do_print_into(buf, zstring_to_view(x));
    }

    template <>
    auto do_print_into<char const*>(String_Slice buf, char const* x) noexcept -> String_Slice
    {
        return do_print_into(buf, zstring_to_view(x));
    }
}

namespace fancy_status
{
    inline namespace prelude
    {
        namespace
        {
            auto overlaps(Buffer_View a, Buffer_View b) noexcept -> bool
            {
                return (
                    true
                    && a.begin() < b.end()
                    && b.begin() < a.end()
                );
            }
        }

        auto zstring_to_slice(char* x) noexcept -> String_Slice
        {
            return to_span_with_sentinel(x);
        }

        auto zstring_to_view(char const* x) noexcept -> String_View
        {
            return to_span_with_sentinel(x);
        }

        auto copy(String_View src, String_Slice dst) noexcept -> size_t
        {
            return copy(src.as_bytes(), dst.as_mutable_bytes());
        }

        auto copy(Buffer_View src, Buffer_Slice dst) noexcept -> size_t
        {
            FANCY_STATUS_ASSERT_WITH(!overlaps(src, dst), "src and dst overlaps");

            auto size = min(dst.size(), src.size());
            std::memcpy(dst.data(), src.data(), size);
            return size;
        }
    }
}

