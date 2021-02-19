#include "config/config.hpp"
#include "config/help.hpp"

namespace fancy_status
{
    auto run(char* argv[]) -> bool
    {
        auto cfg = config::from_argv(argv);
        auto result = true;

        if (cfg.show_version)
            config::print_version();

        if (cfg.show_usage) {
            print("\n");
            config::print_usage(cfg.app_name);
        }

        if (cfg.show_widget_list) {
            print("\n");
            print("WIDGETS:\n");
            // TODO
            print("    <time>\n");
        }

        if (cfg.show_renderer_list) {
            print("\n");
            print("RENDERERS:\n");
            // TODO
            print("    tty\n");
        }

        if (cfg.run_unit_tests) {
            print("\n");
            result &= run_all_test_cases();
        }

        if (cfg.render_expression) {
            // TODO
            print("Using renderer \"", cfg.renderer_name, "\"\n");
            print(cfg.expression, (cfg.no_newline ? String_View{} : String_View{"\n"}));
        }

        if (!cfg.error_description.empty()) {
            print("\n\nBad argument \"", cfg.bad_argv, "\": ", cfg.error_description, "\n\n");
            result = false;
        }

        return result;
    }
}

int main(int argc, char* argv[])
{
    (void) argc;
    return !fancy_status::run(argv);
}

