#pragma once
#include "token.hpp"

namespace fancy_status::fastup
{
    auto prepare_format_error() noexcept -> Block_List;
    auto finish_format_error(Block_List&& token_pool, Block_List const& token_storage) noexcept -> Token const*;

    auto do_format_error(Block_List const& token_pool, String_View x) noexcept -> void;
    auto do_format_error(Block_List const& token_pool, Token const* x) noexcept -> void;    // borrows x->text()

    template <size_t N>
    auto do_format_error(Block_List const& token_pool, char const (&x)[N]) noexcept -> void
    {
        do_format_error(token_pool, String_View{x});
    }

    template <class... Ts>
    auto format_error(Block_List const& token_storage, Ts&&... xs) noexcept -> Token const*
    {
        auto pool = prepare_format_error();
        (do_format_error(pool, forward<Ts>(xs)), ...);
        return finish_format_error(move(pool), token_storage);
    }
}

