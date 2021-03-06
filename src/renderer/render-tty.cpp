#include "registry.hpp"

namespace fancy_status::renderer
{
    namespace
    {
        auto render(Block_List const& output_storage, fastup::Token const* document) noexcept -> String_View
        {
            // TODO
            return {};
        }

        FANCY_STATUS_RENDERER_ADD("tty", &render);
    }
}

