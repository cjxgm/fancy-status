#pragma once
#include "fastup/token.hpp"

namespace fancy_status::renderer
{
    using render_fn = auto (Block_List const& output_storage, fastup::Token const* document) noexcept -> String_View;

    struct Named_Renderer final
    {
        String_View name;
        render_fn* render;
    };

    auto add_renderer(String_View name, render_fn* render) noexcept -> void;
    auto find_renderer(String_View name) noexcept -> render_fn*;
    auto renderers() noexcept -> Span<Named_Renderer const>;
}

#define FANCY_STATUS_RENDERER_ADD(NAME, FN) \
    static auto FANCY_STATUS_CONCAT(fancy_status_renderer_, __COUNTER__) = [] { ::fancy_status::renderer::add_renderer(NAME, FN); return true; } ();

