#pragma once
#include "token.hpp"

namespace fancy_status::fastup
{
    auto build_token(Block_List const& token_pool, String_View x) noexcept -> Token const*;
    auto build_token(Block_List const& token_pool, char x) noexcept -> Token const*;

    template <size_t N>
    auto build_token(Block_List const& token_pool, char const (&x)[N]) noexcept -> Token const*
    {
        return build_token(token_pool, String_View{x});
    }

    // build_token: Create tokens into the token pool.
    // build_token("text"): Create Kind::text tokens.
    // build_token('^'): Create Kind::mark tokens.

    template <class... Ts>
    auto build_tokens(Block_List const& token_pool, Ts&&... xs) noexcept -> void
        // Call build_token with each argument.
    {
        (build_token(token_pool, forward<Ts>(xs)), ...);
    }
}

