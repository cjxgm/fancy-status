#include "utility.hpp"
#include "test.hpp"

namespace fancy_status
{
    inline namespace prelude
    {
        FANCY_STATUS_TEST_CASE("utility: type_identity<T>") {
            using T = int;
            using T_ref = int&;
            using T_rref = int&&;
            using T_ptr = int*;
            using T_const = T const;
            using T_cref = T const&;
            using T_cptr = T const*;

            FANCY_STATUS_TEST_CHECK_EQ((is_same_type<type_identity<T>, T>), true);
            FANCY_STATUS_TEST_CHECK_EQ((is_same_type<type_identity<T_ref>, T_ref>), true);
            FANCY_STATUS_TEST_CHECK_EQ((is_same_type<type_identity<T_rref>, T_rref>), true);
            FANCY_STATUS_TEST_CHECK_EQ((is_same_type<type_identity<T_ptr>, T_ptr>), true);
            FANCY_STATUS_TEST_CHECK_EQ((is_same_type<type_identity<T_const>, T_const>), true);
            FANCY_STATUS_TEST_CHECK_EQ((is_same_type<type_identity<T_cref>, T_cref>), true);
            FANCY_STATUS_TEST_CHECK_EQ((is_same_type<type_identity<T_cptr>, T_cptr>), true);
        };

        FANCY_STATUS_TEST_CASE("utility: remove_reference<T>") {
            using T = int;
            using T_ref = int&;
            using T_rref = int&&;
            using T_ptr = int*;
            using T_const = T const;
            using T_cref = T const&;
            using T_cptr = T const*;

            FANCY_STATUS_TEST_CHECK_EQ((is_same_type<remove_reference<T>, T>), true);
            FANCY_STATUS_TEST_CHECK_EQ((is_same_type<remove_reference<T_ref>, T>), true);
            FANCY_STATUS_TEST_CHECK_EQ((is_same_type<remove_reference<T_rref>, T>), true);
            FANCY_STATUS_TEST_CHECK_EQ((is_same_type<remove_reference<T_ptr>, T_ptr>), true);
            FANCY_STATUS_TEST_CHECK_EQ((is_same_type<remove_reference<T_const>, T_const>), true);
            FANCY_STATUS_TEST_CHECK_EQ((is_same_type<remove_reference<T_cref>, T_const>), true);
            FANCY_STATUS_TEST_CHECK_EQ((is_same_type<remove_reference<T_cptr>, T_cptr>), true);
        };

        FANCY_STATUS_TEST_CASE("utility: remove_const<T>") {
            using T = int;
            using T_ref = int&;
            using T_rref = int&&;
            using T_ptr = int*;
            using T_const = T const;
            using T_cref = T const&;
            using T_cptr = T const*;

            FANCY_STATUS_TEST_CHECK_EQ((is_same_type<remove_const<T>, T>), true);
            FANCY_STATUS_TEST_CHECK_EQ((is_same_type<remove_const<T_ref>, T_ref>), true);
            FANCY_STATUS_TEST_CHECK_EQ((is_same_type<remove_const<T_rref>, T_rref>), true);
            FANCY_STATUS_TEST_CHECK_EQ((is_same_type<remove_const<T_ptr>, T_ptr>), true);
            FANCY_STATUS_TEST_CHECK_EQ((is_same_type<remove_const<T_const>, T>), true);
            FANCY_STATUS_TEST_CHECK_EQ((is_same_type<remove_const<T_cref>, T_cref>), true);
            FANCY_STATUS_TEST_CHECK_EQ((is_same_type<remove_const<T_cptr>, T_cptr>), true);
        };

        FANCY_STATUS_TEST_CASE("utility: array_length<T>") {
            constexpr auto length = size_t(42);
            int array[length];
            auto& ref = array;
            auto& cref = (decltype(array) const&) ref;
            auto ptr = array;

            FANCY_STATUS_TEST_CHECK_EQ(array_length<decltype(array)>, length);
            FANCY_STATUS_TEST_CHECK_EQ(array_length<decltype(ref)>, length);
            FANCY_STATUS_TEST_CHECK_EQ(array_length<decltype(cref)>, length);
            FANCY_STATUS_TEST_CHECK_EQ(array_length<decltype(ptr)>, size_t(-1));
        };

        FANCY_STATUS_TEST_CASE("utility: is_array<T>") {
            constexpr auto length = size_t(42);
            int array[length];
            auto& ref = array;
            auto& cref = (decltype(array) const&) ref;
            auto ptr = array;

            FANCY_STATUS_TEST_CHECK_EQ(is_array<decltype(array)>, true);
            FANCY_STATUS_TEST_CHECK_EQ(is_array<decltype(ref)>, true);
            FANCY_STATUS_TEST_CHECK_EQ(is_array<decltype(cref)>, true);
            FANCY_STATUS_TEST_CHECK_EQ(is_array<decltype(ptr)>, false);
        };

        FANCY_STATUS_TEST_CASE("utility: is_same_type<T, U>") {
            using Weak_Alias = int;
            enum struct Strong_Alias: int {};
            enum Old_School_Strong_Alias: int {};

            FANCY_STATUS_TEST_CHECK_EQ((is_same_type<int, float>), false);
            FANCY_STATUS_TEST_CHECK_EQ((is_same_type<int, int>), true);
            FANCY_STATUS_TEST_CHECK_EQ((is_same_type<int, Weak_Alias>), true);
            FANCY_STATUS_TEST_CHECK_EQ((is_same_type<int, Strong_Alias>), false);
            FANCY_STATUS_TEST_CHECK_EQ((is_same_type<int, Old_School_Strong_Alias>), false);
        };

        FANCY_STATUS_TEST_CASE("utility: is_const<T>") {
            using T = int;
            using T_ref = int&;
            using T_rref = int&&;
            using T_ptr = int*;
            using T_const = T const;
            using T_cref = T const&;
            using T_cptr = T const*;

            FANCY_STATUS_TEST_CHECK_EQ(is_const<T>, false);
            FANCY_STATUS_TEST_CHECK_EQ(is_const<T_ref>, false);
            FANCY_STATUS_TEST_CHECK_EQ(is_const<T_rref>, false);
            FANCY_STATUS_TEST_CHECK_EQ(is_const<T_ptr>, false);
            FANCY_STATUS_TEST_CHECK_EQ(is_const<T_const>, true);
            FANCY_STATUS_TEST_CHECK_EQ(is_const<T_cref>, false);
            FANCY_STATUS_TEST_CHECK_EQ(is_const<T_cptr>, false);
        };

        FANCY_STATUS_TEST_CASE("utility: min/max") {
            FANCY_STATUS_TEST_CHECK_EQ(min(1), 1);
            FANCY_STATUS_TEST_CHECK_EQ(max(1), 1);
            FANCY_STATUS_TEST_CHECK_EQ(min(2, 1), 1);
            FANCY_STATUS_TEST_CHECK_EQ(max(2, 1), 2);
            FANCY_STATUS_TEST_CHECK_EQ(min(2, 1, 3), 1);
            FANCY_STATUS_TEST_CHECK_EQ(max(2, 1, 3), 3);
        };

        FANCY_STATUS_TEST_CASE("utility: align_uint/_ptr") {
            static_assert(align_uint(0b0000u, 0b0100u) == 0b0000'0000u);

            static_assert(align_uint(0b0001u, 0b0100u) == 0b0000'0100u);
            static_assert(align_uint(0b0010u, 0b0100u) == 0b0000'0100u);
            static_assert(align_uint(0b0011u, 0b0100u) == 0b0000'0100u);
            static_assert(align_uint(0b0100u, 0b0100u) == 0b0000'0100u);

            static_assert(align_uint(0b0101u, 0b0100u) == 0b0000'1000u);
            static_assert(align_uint(0b0110u, 0b0100u) == 0b0000'1000u);
            static_assert(align_uint(0b0111u, 0b0100u) == 0b0000'1000u);
            static_assert(align_uint(0b1000u, 0b0100u) == 0b0000'1000u);

            static_assert(align_uint(0b1001u, 0b0100u) == 0b0000'1100u);
            static_assert(align_uint(0b1010u, 0b0100u) == 0b0000'1100u);
            static_assert(align_uint(0b1011u, 0b0100u) == 0b0000'1100u);
            static_assert(align_uint(0b1100u, 0b0100u) == 0b0000'1100u);

            static_assert(align_uint(0b1101u, 0b0100u) == 0b0001'0000u);
            static_assert(align_uint(0b1110u, 0b0100u) == 0b0001'0000u);
            static_assert(align_uint(0b1111u, 0b0100u) == 0b0001'0000u);

            FANCY_STATUS_TEST_CHECK_EQ(align_uint(0b0100u, 0b0100u), 0b0100u);
            FANCY_STATUS_TEST_CHECK_EQ(align_uint(0b0101u, 0b0100u), 0b1000u);
            FANCY_STATUS_TEST_CHECK_EQ(align_uint(0b0110u, 0b0100u), 0b1000u);
            FANCY_STATUS_TEST_CHECK_EQ(align_uint(0b0111u, 0b0100u), 0b1000u);

            FANCY_STATUS_TEST_CHECK_EQ(align_uint(0b0000u, 0b0010u), 0b0000u);
            FANCY_STATUS_TEST_CHECK_EQ(align_uint(0b0001u, 0b0010u), 0b0010u);
            FANCY_STATUS_TEST_CHECK_EQ(align_uint(0b0010u, 0b0010u), 0b0010u);
            FANCY_STATUS_TEST_CHECK_EQ(align_uint(0b0011u, 0b0010u), 0b0100u);

            FANCY_STATUS_TEST_CHECK_EQ(align_uint(0b0000u, 0b0001u), 0b0000u);
            FANCY_STATUS_TEST_CHECK_EQ(align_uint(0b0001u, 0b0001u), 0b0001u);
            FANCY_STATUS_TEST_CHECK_EQ(align_uint(0b0010u, 0b0001u), 0b0010u);
            FANCY_STATUS_TEST_CHECK_EQ(align_uint(0b0011u, 0b0001u), 0b0011u);

            FANCY_STATUS_TEST_CHECK_EQ(align_uint(0b0000u, 0b0000u), 0b0000u);
            FANCY_STATUS_TEST_CHECK_EQ(align_uint(0b0001u, 0b0000u), 0b0000u);
            FANCY_STATUS_TEST_CHECK_EQ(align_uint(0b0010u, 0b0000u), 0b0000u);
            FANCY_STATUS_TEST_CHECK_EQ(align_uint(0b0011u, 0b0000u), 0b0000u);

            FANCY_STATUS_TEST_CHECK_EQ(align_ptr(static_cast<int*>(nullptr), 0b0100u), nullptr);
        };
    }
}

