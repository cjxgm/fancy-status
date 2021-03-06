#include "config/config.hpp"
#include "config/help.hpp"
#include "renderer/registry.hpp"
#include "fastup/parse.hpp"

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
            for (auto& nr: renderer::renderers()) {
                if (nr.name.starts_with(".")) continue;     // skip hidden renderers
                print("    ", nr.name, "\n");
            }
        }

        if (cfg.run_unit_tests) {
            print("\n");
            result &= run_all_test_cases();
        }

        if (cfg.render_expression) {
            if (auto render = renderer::find_renderer(cfg.renderer_name)) {
                Block_List storage;
                auto document = fastup::parse(storage, cfg.expression);
                auto output = render(storage, document);
                dump(output);
                if (!cfg.no_newline) dump("\n");
            } else {
                print("\n\nNo such renderer: ", cfg.renderer_name, "\n\n");
                result = false;
            }
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

