#pragma once
#include "types.hpp"

namespace fancy_status
{
    inline namespace prelude
    {
        // Derive from Pinned for deleted copy and deleted move.
        struct Pinned
        {
            constexpr Pinned() = default;
            constexpr Pinned(Pinned const&) = delete;
            constexpr Pinned(Pinned     &&) = delete;
            constexpr auto operator = (Pinned const&) -> Pinned& = delete;
            constexpr auto operator = (Pinned     &&) -> Pinned& = delete;
        };

        template <class T> struct do_type_identity final { using type = T; };
        template <class T>  using type_identity = typename do_type_identity<T>::type;

        template <class T> struct do_remove_reference      final { using type = T; };
        template <class T> struct do_remove_reference<T &> final { using type = T; };
        template <class T> struct do_remove_reference<T&&> final { using type = T; };
        template <class T>  using remove_reference = typename do_remove_reference<T>::type;

        template <class T> struct do_remove_const          final { using type = T; };
        template <class T> struct do_remove_const<T const> final { using type = T; };
        template <class T>  using remove_const = typename do_remove_const<T>::type;

        // Using "type_identity<T>" to prevent you from writing "forward(x)".
        // You must always write "forward<T>(x)".
        template <class T>
        constexpr auto forward(type_identity<T>& x) noexcept -> decltype(auto)
        {
            return static_cast<T&&>(x);
        }

        template <class T>
        constexpr auto move(T&& x) noexcept -> decltype(auto)
        {
            return static_cast<remove_reference<T>&&>(x);
        }

        template <class T, class U = T>
        constexpr auto exchange(T& a, U&& b)
            noexcept(
                true
                && noexcept(a = move(a))
                && noexcept(a = forward<U>(b))
            )
        -> T
        {
            auto c = move(a);
            a = forward<U>(b);
            return c;
        }

        template <class T>
        inline constexpr auto array_length = size_t(-1);

        template <class T, size_t N>
        inline constexpr auto array_length<T (&)[N]> = N;

        template <class T, size_t N>
        inline constexpr auto array_length<T [N]> = N;

        template <class T>
        inline constexpr auto is_array = false;

        template <class T, size_t N>
        inline constexpr auto is_array<T (&)[N]> = true;

        template <class T, size_t N>
        inline constexpr auto is_array<T [N]> = true;

        template <class T, class U>
        inline constexpr auto is_same_type = false;

        template <class T>
        inline constexpr auto is_same_type<T, T> = true;

        template <class T>
        inline constexpr auto is_const = false;

        template <class T>
        inline constexpr auto is_const<T const> = true;

        template <class T> constexpr auto min(T const& x0) noexcept -> T const& { return x0; }
        template <class T> constexpr auto max(T const& x0) noexcept -> T const& { return x0; }

        template <class T> constexpr auto min(T const& x0, T const& x1) noexcept -> T const& { return (x0 < x1 ? x0 : x1); }
        template <class T> constexpr auto max(T const& x0, T const& x1) noexcept -> T const& { return (x0 < x1 ? x1 : x0); }

        template <class T, class... Ts> auto constexpr min(T const& x0, T const& x1, Ts const&... xs) noexcept -> T const& { return min(x0, min(x1, xs...)); }
        template <class T, class... Ts> auto constexpr max(T const& x0, T const& x1, Ts const&... xs) noexcept -> T const& { return max(x0, max(x1, xs...)); }

        template <class Uint> constexpr auto align_uint(Uint x, Uint alignment) noexcept -> Uint { return x + ((-x) & (alignment - 1)); }
        template <class T>    constexpr auto align_ptr (T* x, size_t alignment) noexcept -> T*   { return reinterpret_cast<T*>(align_uint(reinterpret_cast<uintptr_t>(x), static_cast<uintptr_t>(alignment))); }
    }
}

#define FANCY_STATUS_EXPAND(...) FANCY_STATUS_DO_EXPAND(__VA_ARGS__)
#define FANCY_STATUS_DO_EXPAND(...) __VA_ARGS__

#define FANCY_STATUS_CONCAT(A, B) FANCY_STATUS_DO_CONCAT(A, B)
#define FANCY_STATUS_DO_CONCAT(A, B) A ## B

