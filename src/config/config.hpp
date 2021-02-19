#pragma once

namespace fancy_status::config
{
    struct Config final
    {
        bool show_version{};
        bool show_usage{};
        bool show_widget_list{};
        bool show_renderer_list{};
        bool run_unit_tests{};
        bool no_newline{};
        bool render_expression{true};
        bool internal_has_set_expression_{};    // For config internal use only.
        String_View app_name{};
        String_View renderer_name{"tty"};
        String_View expression{"<time>"};
        String_View error_description{};
        String_View bad_argv{};
    };

    auto from_argv(char* argv[]) noexcept -> Config;
}

