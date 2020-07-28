#include "assert.hpp"
#include "style.hpp"
#include <cstdlib>

namespace fancy_status
{
    inline namespace prelude
    {
        namespace assert_detail
        {
            namespace
            {
                thread_local char scratch_buffer_content[1 << 20];
            }

            thread_local String_Slice scratch_buffer{scratch_buffer_content};
        }

        auto show_error(String_View description) noexcept -> void
        {
            print_stderr(FANCY_STATUS_STYLE_BAD " ERROR " FANCY_STATUS_STYLE_RESET " ", description, "\n\n");
        }

        auto show_assertion_error(
            String_View func,
            String_View file,
            int line,
            String_View description
        ) noexcept -> void
        {
            char message_buffer_content[1 << 20];
            auto message_buffer = String_Slice{message_buffer_content};

            auto buf = message_buffer;
            buf = chain_print_into(buf, "From function `", func, "`");

            if (!file.empty() && file.size() < buf.size() / 2)
                buf = chain_print_into(buf, " in file ", file, ":", line);

            buf = chain_print_into(buf, "\n\n\x20\x20\x20\x20", description);

            show_error(message_buffer.reset_last(buf.begin()));
        }

        [[noreturn]] auto abort() noexcept -> void
        {
            std::abort();
        }
    }
}

