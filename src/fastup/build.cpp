#include "build.hpp"
#include "mark.hpp"

namespace fancy_status::fastup
{
    auto build_token(Block_List const& token_pool, String_View x) noexcept -> Token const*
    {
        return &(token_pool.alloc_back<Token>().front() = Token{Token::kind_text{}, x});
    }

    auto build_token(Block_List const& token_pool, char x) noexcept -> Token const*
    {
        return &(token_pool.alloc_back<Token>().front() = token_from_pairable(x));
    }
}

