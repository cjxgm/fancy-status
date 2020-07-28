#include "print.hpp"
#include <cstdio>

namespace fancy_status
{
    inline namespace prelude
    {
        namespace print_detail
        {
            namespace
            {
                thread_local char scratch_buffer_content[1 << 20];
            }

            thread_local String_Slice scratch_buffer{scratch_buffer_content};
        }

        auto dump(String_View x) noexcept -> void
        {
            fwrite(x.data(), x.size(), 1, stdout);
        }

        auto dump_stderr(String_View x) noexcept -> void
        {
            fwrite(x.data(), x.size(), 1, stderr);
        }
    }
}

