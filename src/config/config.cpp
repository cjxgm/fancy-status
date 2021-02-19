#include "config.hpp"

namespace fancy_status::config
{
    namespace
    {
        auto set_config_error(Config& cfg, String_View description, String_View bad_argv) noexcept -> void
        {
            cfg.show_version = true;
            cfg.show_usage = true;
            cfg.render_expression = false;
            cfg.error_description = description;
            cfg.bad_argv = bad_argv;
        };

        // Returns number of arguments to advance (> 0).
        // Returns 0 to stop further processing.
        // Returns < 0 to make sure there are no more arguments. The absolute value is the number of arguments to advance.
        auto do_from_argv(Config& cfg, String_View current, String_View next) noexcept -> int
        {
            auto set_error = [&] (String_View description, String_View bad_argv) noexcept -> int {
                set_config_error(cfg, description, bad_argv);
                return 0;
            };

            if (current == "-V" || current == "--version") {
                cfg.show_version = true;
                cfg.render_expression = false;
                return 1;
            }

            if (current == "-h" || current == "--help") {
                cfg.show_version = true;
                cfg.show_usage = true;
                cfg.show_widget_list = true;
                cfg.show_renderer_list = true;
                cfg.render_expression = false;
                return 1;
            }

            if (current == "-t" || current == "--test") {
                cfg.show_version = true;
                cfg.run_unit_tests = true;
                cfg.render_expression = false;
                return 1;
            }

            if (current == "-n" || current == "--no-newline") {
                cfg.no_newline = true;
                return 1;
            }

            if (current == "-r" || current == "--renderer") {
                if (next.data() == nullptr)
                    return set_error("Missing renderer name.", current);
                if (next.empty())
                    return set_error("Renderer name cannot be empty.", current);
                cfg.renderer_name = next;
                return 2;
            }

            if (current == "--") {
                if (next.data() == nullptr) {
                    if (!cfg.internal_has_set_expression_) {
                        cfg.internal_has_set_expression_ = true;
                        cfg.expression = {};
                    }
                    return -1;
                } else {
                    if (cfg.internal_has_set_expression_)
                        return set_error("Only 1 single expression can be set.", next);
                    cfg.internal_has_set_expression_ = true;
                    cfg.expression = next;
                    return -2;
                }
            }

            if (!current.empty() && current[0] == '-')
                return set_error("Unknown command-line argument.", current);

            if (cfg.internal_has_set_expression_)
                return set_error("Only 1 single expression can be set.", current);
            cfg.internal_has_set_expression_ = true;
            cfg.expression = current;
            return 1;
        }
    }

    auto from_argv(char* argv[]) noexcept -> Config
    {
        Config cfg;

        if (*argv)
            cfg.app_name = zstring_to_view(*argv++);

        while (*argv) {
            auto current = zstring_to_view(argv[0]);
            // argv is guaranteed to have nullptr after the last element.
            auto next = (argv[1] == nullptr ? String_View{} : zstring_to_view(argv[1]));
            auto advance = do_from_argv(cfg, current, next);
            if (advance < 0) {
                argv += -advance;
                if (argv[0]) set_config_error(cfg, "Too many arguments.", zstring_to_view(argv[0]));
            } else {
                argv += advance;
            }
            if (advance <= 0) break;
        }

        return cfg;
    }
}

