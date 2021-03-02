#pragma once
#include "token.hpp"

namespace fancy_status::fastup
{
    enum struct Is_Pairable
    {
        no,
        entering,
        leaving,
    };

    auto is_pairable(Token const* tk) noexcept -> Is_Pairable;
    auto paired_char_of(char pairable) noexcept -> char;
    auto token_from_pairable(char pairable) noexcept -> Token;
}

