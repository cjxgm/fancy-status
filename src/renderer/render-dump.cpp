#include "registry.hpp"
#include "fastup/print.hpp"
// A debug dump renderer. Hidden because the output always goes to stdout and thus an invalid renderer (API-conformance-wise).

namespace fancy_status::renderer
{
    namespace
    {
        auto render(Block_List const& output_storage, fastup::Token const* document) noexcept -> String_View
        {
            (void) output_storage;
            print_tree(document);
            return {};
        }

        FANCY_STATUS_RENDERER_ADD(".dump", &render);
    }
}

