#pragma once
#include "token.hpp"

namespace fancy_status::fastup
{
    auto treelize(Block_List const& token_storage, Span<Token> tokens) noexcept -> Token const*;
}

