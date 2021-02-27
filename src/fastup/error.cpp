#include "error.hpp"
#include "build.hpp"
#include "tree.hpp"

namespace fancy_status::fastup
{
    namespace
    {
        namespace color
        {
            constexpr auto error_bg = String_View{"551100"};
            constexpr auto error_fg = String_View{"ff6666"};
            constexpr auto error_indicator_fg = String_View{"44ccdd"};
            constexpr auto error_reference_fg = String_View{"ccdd44"};
        }

        thread_local auto recursion_guard = false;
    }

    auto prepare_format_error() noexcept -> Block_List
    {
        FANCY_STATUS_ASSERT_WITH(!recursion_guard, "Invalid Fastup syntax in an error message.");
        recursion_guard = true;

        auto token_pool = Block_List{};
        build_tokens(token_pool, '[', color::error_bg, '(', color::error_fg, " ", '(', color::error_indicator_fg, "<", ')', "ERROR: ");
        return token_pool;
    }

    auto finish_format_error(Block_List&& token_pool, Block_List const& token_storage) noexcept -> Token const*
    {
        build_tokens(token_pool, '(', color::error_indicator_fg, ">", ')', " ", ')', ']');

        auto tokens = token_pool.consolidate_for_data().cast<Token>();
        token_storage.splice_back(move(token_pool));

        auto token = treelize(token_storage, tokens);

        recursion_guard = false;
        return token;
    }

    auto do_format_error(Block_List const& token_pool, String_View x) noexcept -> void
    {
        build_tokens(token_pool, x);
    }

    auto do_format_error(Block_List const& token_pool, Token const* x) noexcept -> void
    {
        FANCY_STATUS_ASSERT(x != nullptr);
        build_tokens(token_pool, '(', color::error_reference_fg, x->text(), ')');
    }
}

#include "print.hpp"

namespace fancy_status::fastup
{
    FANCY_STATUS_TEST_CASE("fastup: format error") {
        auto token_storage = Block_List{};
        auto root = format_error(token_storage, "hello");
        root[2].connect_alt(format_error(token_storage, "world"));
        print_tree(root);
    };
}

