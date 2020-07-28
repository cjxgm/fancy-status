#pragma once
#include "types.hpp"
#include "utility.hpp"

namespace fancy_status
{
    inline namespace prelude
    {
        template <class T, bool Source_Has_Sentinel=false>
        struct Span;

        template <class T, size_t N>
        Span(T (&)[N]) -> Span<T, false>;

        template <size_t N>
        Span(char (&)[N]) -> Span<char, true>;

        template <size_t N>
        Span(char const (&)[N]) -> Span<char const, true>;

        namespace span_detail
        {
            template <class T>
            inline constexpr auto is_span_without_sentinel = false;

            template <class T>
            inline constexpr auto is_span_without_sentinel<Span<T, false>> = true;

            template <class T>
            constexpr auto begin(T&& x) noexcept -> auto*
            {
                if constexpr (is_array<T>) {
                    return &x[0];
                } else {
                    return &*x.begin();
                }
            }

            template <bool Source_Has_Sentinel, class T>
            constexpr auto end(T&& x) noexcept -> auto*
            {
                if constexpr (is_array<T>) {
                    if constexpr (Source_Has_Sentinel) {
                        static_assert(array_length<T> >= 1, "Array must have at least 1 element for the sentinel.");
                        return &x[array_length<T> - 1];
                    } else {
                        return &x[array_length<T>];
                    }
                } else {
                    // That means, you can never recover the space taken by sentinels.
                    if constexpr (Source_Has_Sentinel && is_span_without_sentinel<T>) {
                        return &*x.end() - 1;
                    } else {
                        return &*x.end();
                    }
                }
            }
        }

        template <class T, bool Source_Has_Sentinel>
        struct Span final
        {
            using value_type = T;
            using size_type = size_t;
            using index_type = ptrdiff_t;
            using pointer = value_type*;
            using const_pointer = value_type const*;
            using reference = value_type&;
            using const_reference = value_type const&;
            using mutable_reference = remove_const<value_type>&;
            using iterator = pointer;
            using const_iterator = const_pointer;

            ~Span() noexcept = default;
            constexpr Span() noexcept = default;

            template <class Iter_First, class Iter_Last>
            constexpr Span(Iter_First&& first, Iter_Last&& last) noexcept
                // Use "&*iter" instead of "iter" to make sure that iterators are converted to pointers.
                : first{&*first}, last{&*last}
            {}

            template <class Iter_First>
            constexpr Span(Iter_First&& first, size_type size) noexcept
                : Span{first, first + size}
            {}

            // Handles:
            // - Copying of Span
            // - Moving  of Span
            // - Span<T> to Span<T const>
            // - Container<T> to Span<T>
            // - T[N] to Span<T>
            template <class U>
            constexpr Span(U&& x) noexcept
                : Span{span_detail::begin(forward<U>(x)), span_detail::end<Source_Has_Sentinel>(forward<U>(x))}
            {}

            template <class U>
            constexpr auto operator = (U&& x) noexcept -> Span&
            {
                first = span_detail::begin(forward<U>(x));
                last = span_detail::end<Source_Has_Sentinel>(forward<U>(x));
                return *this;
            }

            constexpr auto empty() const noexcept -> bool { return (first >= last); }

            constexpr auto begin() const noexcept -> iterator { return first; }
            constexpr auto   end() const noexcept -> iterator { return  last; }

            constexpr auto front() const noexcept -> reference { return first[0]; }
            constexpr auto  back() const noexcept -> reference { return last[-1]; }

            constexpr auto data() const noexcept -> pointer { return first; }
            constexpr auto size() const noexcept -> size_type { return (empty() ? size_type(0) : size_type(last - first)); }

            constexpr auto sentinel() const noexcept -> mutable_reference {
                static_assert(Source_Has_Sentinel);
                return const_cast<mutable_reference>(*last);
            }

            constexpr auto ignore_sentinel() const noexcept -> Span<value_type, false> { return {first, last}; }
            constexpr auto read_only() const noexcept -> Span<value_type const, Source_Has_Sentinel> { return cast<value_type const>(); }

            constexpr auto data_with_sentinel(const_reference sentinel_value={}) const noexcept -> pointer {
                auto& s = sentinel();
                if (!(s == sentinel_value))
                    s = sentinel_value;

                return first;
            }

            constexpr auto operator [] (index_type idx) const noexcept -> reference { return first[idx]; }

            // - Span<T>            can cast to Span<byte>  or Span<byte const>.
            // - Span<byte>         can cast to Span<T>     or Span<T const>.
            // - Span<T const>      can cast to Span<byte const>.
            // - Span<byte const>   can cast to Span<T const>.
            // - Otherwise, casting Span<T> to Span<U> is possible if static casting T* to U* is possible.
            template <class U>
            constexpr auto cast() const noexcept -> Span<U, Source_Has_Sentinel>
            {
                if constexpr (false) {}

                else if constexpr (is_same_type<U, byte>) {
                    static_assert(!is_const<value_type>, "Cannot remove const qualifier.");
                    return {
                        static_cast<U*>(static_cast<void*>(first)),
                        static_cast<U*>(static_cast<void*>( last)),
                    };
                }

                else if constexpr (is_same_type<U, byte const>) {
                    return {
                        static_cast<U*>(static_cast<void const*>(first)),
                        static_cast<U*>(static_cast<void const*>( last)),
                    };
                }

                else if constexpr (is_same_type<value_type, byte>) {
                    return {
                        static_cast<U*>(static_cast<void*>(first)),
                        static_cast<U*>(static_cast<void*>( last)),
                    };
                }

                else if constexpr (is_same_type<value_type, byte const>) {
                    return {
                        static_cast<U*>(static_cast<void const*>(first)),
                        static_cast<U*>(static_cast<void const*>( last)),
                    };
                }

                else {
                    return {
                        static_cast<U*>(first),
                        static_cast<U*>( last),
                    };
                }
            }

            constexpr auto as_bytes() const noexcept -> Span<byte const>
            {
                return cast<byte const>().ignore_sentinel();
            }

            constexpr auto as_mutable_bytes() const noexcept -> Span<byte>
            {
                return cast<byte>().ignore_sentinel();
            }

            constexpr auto align_first(size_t alignment) const noexcept -> Span { return {align_ptr(first, alignment), last}; }
            constexpr auto align_last (size_t alignment) const noexcept -> Span { return {first, align_ptr(last, alignment)}; }

            // [first, last) ---reset_first(*)-----> [        *,  last    )
            // [first, last) ---reset_last(*)------> [first    ,         *)
            // [first, last) ---remove_prefix(*)---> [first + *,  last    )
            // [first, last) ---remove_suffix(*)---> [first    ,  last - *)
            // [first, last) ---add_prefix(*)------> [first - *,  last    )
            // [first, last) ---add_suffix(*)------> [first    ,  last + *)
            // [first, last) ---prefix(*)----------> [first    , first + *)
            // [first, last) ---suffix(*)----------> [ last - *,  last    )
            // It is undefined behavior when the result goes out of [first, last).
            constexpr auto reset_first(pointer x) const noexcept -> Span { return {x, last}; }
            constexpr auto reset_last (pointer x) const noexcept -> Span { return {first, x}; }
            constexpr auto remove_prefix(size_type size) const noexcept -> Span { return {first + size, last}; }
            constexpr auto remove_suffix(size_type size) const noexcept -> Span { return {first, last - size}; }
            constexpr auto add_prefix(size_type size) const noexcept -> Span { return {first - size, last}; }
            constexpr auto add_suffix(size_type size) const noexcept -> Span { return {first, last + size}; }
            constexpr auto prefix(size_type size) const noexcept -> Span { return {first, first + size}; }
            constexpr auto suffix(size_type size) const noexcept -> Span { return {last - size, last}; }

            // Returns end() if not found.
            constexpr auto find_first_of(const_reference x) const noexcept -> iterator
            {
                for (auto& v: *this)
                    if (v == x)
                        return &v;
                return end();
            }

            // Returns end() if not found.
            constexpr auto find_first_not_of(const_reference x) const noexcept -> iterator
            {
                for (auto& v: *this)
                    if (!(v == x))
                        return &v;
                return end();
            }

            constexpr auto starts_with(Span x) const noexcept -> bool { return (size() >= x.size() && prefix(x.size()) == x); }
            constexpr auto   ends_with(Span x) const noexcept -> bool { return (size() >= x.size() && suffix(x.size()) == x); }

        private:
            pointer first{};
            pointer last{};
        };

        template <class T, class U, bool T_Has_Sentinel, bool U_Has_Sentinel>
        constexpr auto operator == (Span<T, T_Has_Sentinel> a, Span<U, U_Has_Sentinel> b) noexcept -> bool
        {
            if (a.size() != b.size())
                return false;

            auto p = b.begin();
            for (auto& x: a)
                if (!(x == *p++))
                    return false;

            return true;
        }

        template <class T, class U, bool T_Has_Sentinel, bool U_Has_Sentinel>
        constexpr auto operator != (Span<T, T_Has_Sentinel> a, Span<U, U_Has_Sentinel> b) noexcept -> bool
        {
            return !(a == b);
        }

        template <class T, bool Has_Sentinel, class U> constexpr auto operator == (Span<T, Has_Sentinel> a, U const& b) noexcept -> bool { return (a == Span{b}); }
        template <class T, bool Has_Sentinel, class U> constexpr auto operator == (U const& a, Span<T, Has_Sentinel> b) noexcept -> bool { return (Span{a} == b); }
        template <class T, bool Has_Sentinel, class U> constexpr auto operator != (Span<T, Has_Sentinel> a, U const& b) noexcept -> bool { return (a != Span{b}); }
        template <class T, bool Has_Sentinel, class U> constexpr auto operator != (U const& a, Span<T, Has_Sentinel> b) noexcept -> bool { return (Span{a} != b); }

        template <class T>
        constexpr auto to_span_with_sentinel(T* first, T const& sentinel_value={}) noexcept -> Span<T, true>
        {
            auto last = first;
            while (!(*last == sentinel_value))
                last++;

            return {first, last};
        }
    }
}

