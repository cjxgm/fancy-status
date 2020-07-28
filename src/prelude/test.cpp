#include "test.hpp"
#include "types.hpp"
#include "style.hpp"
#include "assert.hpp"
#include "block.hpp"
#include <stdexcept>

#if __COUNTER__ == __COUNTER__
    #error "Bad compiler"
#endif

namespace fancy_status
{
    inline namespace prelude
    {
        namespace test_detail
        {
            namespace
            {
                thread_local char scratch_buffer_content[1 << 20];
            }

            thread_local String_Slice scratch_buffer{scratch_buffer_content};
        }

        struct Test_Case final
        {
            String_View name{};
            test_case_fn* fn{};
        };

        namespace
        {
            struct Test_Context final
            {
                Test_Case* this_test{};

                size_t num_passed_cases{};
                size_t num_failed_cases{};
                size_t num_passed_assertions{};
                size_t num_failed_assertions{};
            };

            Test_Context test_ctx;

            auto test_cases() noexcept -> Block_List&
            {
                thread_local auto tests = Block_List{};
                return tests;
            }

            auto run_test_case(Test_Case& test) noexcept -> void
            {
                FANCY_STATUS_ASSERT_WITH(test.fn != nullptr, "Test case not registered: ", test.name);

                print(
                    String_View{FANCY_STATUS_STYLE_RESET "CASE" "\x20" FANCY_STATUS_STYLE_TITLE}
                    , test.name
                    , FANCY_STATUS_STYLE_RESET "\n"
                );

                test_ctx.this_test = &test;
                auto passed = false;
                auto initial_num_failed_assertions = test_ctx.num_failed_assertions;

                try {
                    test.fn();

                    if (test_ctx.num_failed_assertions == initial_num_failed_assertions)
                        passed = true;
                }

                catch (std::exception const& e) {
                    print(
                        String_View{FANCY_STATUS_STYLE_BAD  "FAIL" FANCY_STATUS_STYLE_RESET "\x20"}
                        , test_ctx.this_test->name
                        , "\n\x20\x20" "Exception: "
                        , e.what()
                        , "\n"
                    );
                }

                catch (...) {
                    print(
                        String_View{FANCY_STATUS_STYLE_BAD  "FAIL" FANCY_STATUS_STYLE_RESET "\x20"}
                        , test_ctx.this_test->name
                        , "\n\x20\x20" "Unknown exception.\n"
                    );
                }

                (passed ? test_ctx.num_passed_cases : test_ctx.num_failed_cases)++;
            }
        }

        auto new_test_case(String_View name) noexcept -> Test_Case&
        {
            auto& test = test_cases().alloc_back<Test_Case>().front();
            test.name = name;
            return test;
        }

        auto register_test_case_fn(Test_Case& test, test_case_fn* fn) noexcept -> void
        {
            FANCY_STATUS_ASSERT_WITH(test.fn == nullptr, "Test case already registered: ", test.name);
            test.fn = fn;
        }

        auto operator / (Test_Case& test, test_case_fn* fn) noexcept -> nullptr_t
        {
            register_test_case_fn(test, fn);
            return nullptr;
        }

        auto run_all_test_cases() noexcept -> bool
        {
            test_ctx = {};

            auto tests = test_cases().consolidate_for_data().cast<Test_Case>();
            for (auto& test: tests)
                run_test_case(test);

            auto all_passed = (test_ctx.num_failed_cases == 0);

            auto all_if_passed = (all_passed ? String_View{"All\x20"} : String_View{});
            auto style_by_passed = (all_passed ? String_View{FANCY_STATUS_STYLE_GOOD} : String_View{FANCY_STATUS_STYLE_BAD});

            print(
                String_View{FANCY_STATUS_STYLE_TITLE "RESULT" FANCY_STATUS_STYLE_RESET "\x20"}
                , all_if_passed
                , FANCY_STATUS_STYLE_GOOD, test_ctx.num_passed_cases, FANCY_STATUS_STYLE_RESET, " test cases passed.\x20"
                , style_by_passed,         test_ctx.num_failed_cases, FANCY_STATUS_STYLE_RESET, " test cases failed.\n"
            );

            print(
                String_View{FANCY_STATUS_STYLE_TITLE "RESULT" FANCY_STATUS_STYLE_RESET "\x20"}
                , all_if_passed
                , FANCY_STATUS_STYLE_GOOD, test_ctx.num_passed_assertions, FANCY_STATUS_STYLE_RESET, " assertions passed.\x20"
                , style_by_passed,         test_ctx.num_failed_assertions, FANCY_STATUS_STYLE_RESET, " assertions failed.\n"
            );

            return all_passed;
        }

        auto finish_this_assertion(bool passed, String_View assert_name) noexcept -> void
        {
            FANCY_STATUS_ASSERT_WITH(test_ctx.this_test != nullptr, "Can only be called inside test cases: ", assert_name);

            print(
                (
                    passed
                    ? String_View{FANCY_STATUS_STYLE_GOOD "PASS" FANCY_STATUS_STYLE_RESET "\x20"}
                    : String_View{FANCY_STATUS_STYLE_BAD  "FAIL" FANCY_STATUS_STYLE_RESET "\x20"}
                )
                , test_ctx.this_test->name
                , ": "
                , assert_name
                , "\n"
            );

            (passed ? test_ctx.num_passed_assertions : test_ctx.num_failed_assertions)++;
        }
    }
}

