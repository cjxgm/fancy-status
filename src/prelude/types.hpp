#pragma once

namespace fancy_status
{
    inline namespace prelude
    {
        using size_t    = decltype(sizeof(0));
        using ptrdiff_t = decltype((char*)0 - (char*)0);
        static_assert(sizeof(size_t)    == 8, "Only support 64-bit systems");
        static_assert(sizeof(ptrdiff_t) == 8, "Only support 64-bit systems");

        using float32_t = float;
        using float64_t = double;
        static_assert(sizeof(float32_t) == 4);
        static_assert(sizeof(float64_t) == 8);

        using int8_t = signed char;
        using int16_t = signed short;
        using int32_t = signed int;
        using int64_t = ptrdiff_t;
        static_assert(sizeof(int8_t)  == 1);
        static_assert(sizeof(int16_t) == 2);
        static_assert(sizeof(int32_t) == 4);
        static_assert(sizeof(int64_t) == 8);

        using uint8_t = unsigned char;
        using uint16_t = unsigned short;
        using uint32_t = unsigned int;
        using uint64_t = size_t;
        static_assert(sizeof(uint8_t)  == 1);
        static_assert(sizeof(uint16_t) == 2);
        static_assert(sizeof(uint32_t) == 4);
        static_assert(sizeof(uint64_t) == 8);

        using  intptr_t = ptrdiff_t;
        using  intmax_t = ptrdiff_t;
        using uintptr_t = size_t;
        using uintmax_t = size_t;
        static_assert(sizeof( intptr_t) == 8);
        static_assert(sizeof( intmax_t) == 8);
        static_assert(sizeof(uintptr_t) == 8);
        static_assert(sizeof(uintmax_t) == 8);
        static_assert(sizeof( intptr_t) == sizeof( intmax_t));
        static_assert(sizeof(uintptr_t) == sizeof(uintmax_t));

        struct alignas(16) max_align_t final {};
        static_assert(alignof(max_align_t) == 16);

        using nullptr_t = decltype(nullptr);

        enum struct byte: uint8_t {};
        static_assert(sizeof(byte) == 1);
        static_assert(alignof(byte) == 1);
    }
}

