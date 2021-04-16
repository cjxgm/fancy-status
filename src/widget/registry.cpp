#include "registry.hpp"
// TODO: Use hash map.
// TODO: Make sure widget names are unique.

namespace fancy_status::widget
{
    namespace
    {
        auto widget_storage() noexcept -> Block_List const&
        {
            thread_local auto storage = Block_List{};
            return storage;
        }
    }

    auto add_widget(String_View name, expand_fn* expand) noexcept -> void
    {
        auto& storage = widget_storage();
        storage.alloc_back<Named_Widget>().front() = {
            name,
            expand,
        };
    }

    auto find_widget(String_View name) noexcept -> expand_fn*
    {
        for (auto& nw: widgets())
            if (nw.name == name)
                return nw.expand;
        return nullptr;
    }

    auto widgets() noexcept -> Span<Named_Widget const>
    {
        return widget_storage().consolidate_for_data().cast<Named_Widget const>();
    }
}

