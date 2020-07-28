#include "types.hpp"
#include "string.hpp"
#include "print.hpp"
#include "utility.hpp"
#include <cstdio>
#include <cfloat>
#include <cinttypes>        // for PRId64 etc.

namespace fancy_status::printer
{
    namespace
    {
        template <class... Args>
        auto sprintf_into(String_Slice buf, char const* fmt, Args&&... args) noexcept -> String_Slice
        {
            auto n = std::snprintf(buf.data(), buf.size() + 1, fmt, forward<Args>(args)...);
            if (n < 0) return buf;
            if (size_t(n) > buf.size()) return buf.suffix(0);
            return buf.remove_prefix(n);
        }
    }

    template <>
    auto do_print_into<nullptr_t>(String_Slice buf, nullptr_t) noexcept -> String_Slice
    {
        return do_print_into(buf, String_View{"nullptr"});
    }

    template <>
    auto do_print_into<void const*>(String_Slice buf, void const* x) noexcept -> String_Slice
    {
        if (x == nullptr) {
            return do_print_into(buf, String_View{"(0x00-----00-----00)"});
        } else {
            return sprintf_into(buf, "(0x%016" PRIxPTR ")", x);
        }
    }

    template <>
    auto do_print_into<void*>(String_Slice buf, void* x) noexcept -> String_Slice
    {
        return do_print_into(buf, const_cast<void const*>(x));
    }

    template <>
    auto do_print_into<bool>(String_Slice buf, bool x) noexcept -> String_Slice
    {
        return do_print_into(buf, (x ? String_View{"true"} : String_View{"false"}));
    }

    template <>
    auto do_print_into<char>(String_Slice buf, char x) noexcept -> String_Slice
    {
        if (buf.empty()) return buf;

        buf[0] = x;
        return buf.remove_prefix(1);
    }

    #define FANCY_STATUS_DEFINE_PRINTER_VIA_SPRINTF(TYPE, ...) \
        template <> \
        auto do_print_into<TYPE>(String_Slice buf, TYPE x) noexcept -> String_Slice \
        { \
            return sprintf_into(buf, __VA_ARGS__); \
        }

    FANCY_STATUS_DEFINE_PRINTER_VIA_SPRINTF(byte, "%02x", uint8_t(x));

    FANCY_STATUS_DEFINE_PRINTER_VIA_SPRINTF(int8_t,  "%" PRId8,  x);
    FANCY_STATUS_DEFINE_PRINTER_VIA_SPRINTF(int16_t, "%" PRId16, x);
    FANCY_STATUS_DEFINE_PRINTER_VIA_SPRINTF(int32_t, "%" PRId32, x);
    FANCY_STATUS_DEFINE_PRINTER_VIA_SPRINTF(int64_t, "%" PRId64, x);

    FANCY_STATUS_DEFINE_PRINTER_VIA_SPRINTF(uint8_t,  "%" PRIu8,  x);
    FANCY_STATUS_DEFINE_PRINTER_VIA_SPRINTF(uint16_t, "%" PRIu16, x);
    FANCY_STATUS_DEFINE_PRINTER_VIA_SPRINTF(uint32_t, "%" PRIu32, x);
    FANCY_STATUS_DEFINE_PRINTER_VIA_SPRINTF(uint64_t, "%" PRIu64, x);

    FANCY_STATUS_DEFINE_PRINTER_VIA_SPRINTF(float32_t, "%.*g", x, FLT_DECIMAL_DIG);
    FANCY_STATUS_DEFINE_PRINTER_VIA_SPRINTF(float64_t, "%.*g", x, DBL_DECIMAL_DIG);
}

