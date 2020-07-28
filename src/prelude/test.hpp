#pragma once
#include "print.hpp"
#include "string.hpp"
#include "utility.hpp"

namespace fancy_status
{
    inline namespace prelude
    {
        namespace test_detail
        {
            extern thread_local String_Slice scratch_buffer;
        }

        using test_case_fn = auto () -> void;

        struct Test_Case;

        // In any context
        auto new_test_case(String_View name) noexcept -> Test_Case&;     // `name` must have global lifetime.
        auto register_test_case_fn(Test_Case& test, test_case_fn* fn) noexcept -> void;
        auto operator / (Test_Case& test, test_case_fn* fn) noexcept -> nullptr_t;

        // Returns true if all cases passed.
        // Returns false if any cases failed.
        auto run_all_test_cases() noexcept -> bool;

        // In test cases
        auto finish_this_assertion(bool passed, String_View assert_name) noexcept -> void;
    }
}

// Usage: FANCY_STATUS_TEST_CASE("name") { /* test code */ };
#define FANCY_STATUS_TEST_CASE(NAME) \
    static auto FANCY_STATUS_CONCAT(fancy_status_test_case_, __COUNTER__) = ::fancy_status::prelude::new_test_case(NAME) / [] () -> void

// When failed, report failure and continue.
#define FANCY_STATUS_TEST_CHECK(NAME, PRED) FANCY_STATUS_TEST_ASSERT(false, NAME, PRED)
#define FANCY_STATUS_TEST_CHECK_BIN_OP(A, OP, B) FANCY_STATUS_TEST_ASSERT_BIN_OP(false, A, OP, B)
#define FANCY_STATUS_TEST_CHECK_EQ(A, B) FANCY_STATUS_TEST_ASSERT_EQ(false, A, B)
#define FANCY_STATUS_TEST_CHECK_NE(A, B) FANCY_STATUS_TEST_ASSERT_NE(false, A, B)

// When failed, report failure and stop.
#define FANCY_STATUS_TEST_ENSURE(NAME, PRED) FANCY_STATUS_TEST_ASSERT(true, NAME, PRED)
#define FANCY_STATUS_TEST_ENSURE_BIN_OP(A, OP, B) FANCY_STATUS_TEST_ASSERT_BIN_OP(true, A, OP, B)
#define FANCY_STATUS_TEST_ENSURE_EQ(A, B) FANCY_STATUS_TEST_ASSERT_EQ(true, A, B)
#define FANCY_STATUS_TEST_ENSURE_NE(A, B) FANCY_STATUS_TEST_ASSERT_NE(true, A, B)

#define FANCY_STATUS_TEST_ASSERT(ENSURE, NAME, PRED) do { \
    if (PRED) { \
        ::fancy_status::prelude::finish_this_assertion(true, NAME); \
    } else { \
        ::fancy_status::prelude::finish_this_assertion(false, NAME); \
        if constexpr (ENSURE) return; \
    } \
} while (false)

#define FANCY_STATUS_TEST_ASSERT_BIN_OP(ENSURE, A, OP, B) do { \
    auto __fancy_status_test_subject_a = (A); \
    auto __fancy_status_test_subject_b = (B); \
    if (__fancy_status_test_subject_a OP __fancy_status_test_subject_b) { \
        ::fancy_status::prelude::finish_this_assertion(true, #A " " #OP " " #B); \
    } else { \
        ::fancy_status::prelude::finish_this_assertion( \
            false, \
            ::fancy_status::prelude::print_into( \
                ::fancy_status::prelude::test_detail::scratch_buffer \
                , #A " " #OP " " #B \
                , "\n\x20\x20" #A " = ", __fancy_status_test_subject_a \
                , "\n\x20\x20" #B " = ", __fancy_status_test_subject_b \
            ) \
        ); \
        if constexpr (ENSURE) return; \
    } \
} while (false)

#define FANCY_STATUS_TEST_ASSERT_EQ(ENSURE, A, B) FANCY_STATUS_TEST_ASSERT_BIN_OP(ENSURE, A, ==, B)
#define FANCY_STATUS_TEST_ASSERT_NE(ENSURE, A, B) FANCY_STATUS_TEST_ASSERT_BIN_OP(ENSURE, A, !=, B)

