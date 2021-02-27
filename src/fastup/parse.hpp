#pragma once
#include "token.hpp"

namespace fancy_status::fastup
{
    auto parse(Block_List const& token_storage, String_View src) noexcept -> Token const*;
    // parse: Scan then treelize.
    // Post-condition:
    // - result != nullptr
    // - result->pair != nullptr
    // - result->pair == &token_storage.consolidate_for_data().back());
}

