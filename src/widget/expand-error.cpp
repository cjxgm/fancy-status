#include "registry.hpp"
#include "fastup/print.hpp"
// A debug dump widget. Hidden because the output always goes to stdout and thus an invalid widget (API-conformance-wise).

namespace fancy_status::widget
{
    namespace
    {
        auto expand(Block_List const& output_storage, fastup::Token const* document) noexcept -> fastup::Token const*
        {
            (void) output_storage;
            print_tree(document);
            return {};
        }

        FANCY_STATUS_WIDGET_ADD(".error", &expand);
    }
}

