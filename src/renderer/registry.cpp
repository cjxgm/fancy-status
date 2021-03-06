#include "registry.hpp"
// TODO: Use hash map.
// TODO: Make sure renderer names are unique.

namespace fancy_status::renderer
{
    namespace
    {
        auto renderer_storage() noexcept -> Block_List const&
        {
            thread_local auto storage = Block_List{};
            return storage;
        }
    }

    auto add_renderer(String_View name, render_fn* render) noexcept -> void
    {
        auto& storage = renderer_storage();
        storage.alloc_back<Named_Renderer>().front() = {
            name,
            render,
        };
    }

    auto find_renderer(String_View name) noexcept -> render_fn*
    {
        for (auto& nr: renderers())
            if (nr.name == name)
                return nr.render;
        return nullptr;
    }

    auto renderers() noexcept -> Span<Named_Renderer const>
    {
        return renderer_storage().consolidate_for_data().cast<Named_Renderer const>();
    }
}

