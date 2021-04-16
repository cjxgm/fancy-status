#pragma once
#include "fastup/token.hpp"

namespace fancy_status::widget
{
    auto expand_widgets(Block_List const& token_storage, fastup::Token const* document) noexcept -> void;
}

