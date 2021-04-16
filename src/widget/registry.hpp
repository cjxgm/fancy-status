#pragma once
#include "fastup/token.hpp"

namespace fancy_status::widget
{
    using expand_fn = auto (Block_List const& token_storage, fastup::Token const* widget_src) noexcept -> fastup::Token const*;

    struct Named_Widget final
    {
        String_View name;
        expand_fn* expand;
    };

    auto add_widget(String_View name, expand_fn* expand) noexcept -> void;
    auto find_widget(String_View name) noexcept -> expand_fn*;
    auto widgets() noexcept -> Span<Named_Widget const>;
}

#define FANCY_STATUS_WIDGET_ADD(NAME, FN) \
    static auto FANCY_STATUS_CONCAT(fancy_status_widget_, __COUNTER__) = [] { ::fancy_status::widget::add_widget(NAME, FN); return false; } ();

