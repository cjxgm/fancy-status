#include "help.hpp"

namespace fancy_status::config
{
    auto print_version() noexcept -> void
    {
        print(
            "fancy-status 0.4.dev\n"
            "Giumo Clanjor (Weiju Lan) <cjxgm2@gmail.com>\n"
            "Opinionated visualization of system info.\n"
        );
    }

    auto print_usage(String_View app_name) noexcept -> void
    {
        print(
            "USAGE:\n"
            "    ", app_name, " [<OPTIONS>] [--] [<EXPRESSION>]\n"
            "\n"
            "OPTIONS:\n"
            "    -h, --help                 Print version, usage, widgets list, renderers list, and then quit.\n"
            "    -V, --version              Print version and quit.\n"
            "    -t, --test                 Run unit tests and quit.\n"
            "    -n, --no-newline           Do not append a newline to the expression.\n"
            "    -r, --renderer <name>      Select a renderer (output format). (default: tty)\n"
            "    --                         Mark the end of options.\n"
            "    <EXPRESSION>               A Fastup expression to be evaluated. (default: \"<time>\")\n"
        );
    }
}

