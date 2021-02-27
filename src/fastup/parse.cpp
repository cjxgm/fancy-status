#include "parse.hpp"
#include "build.hpp"
#include "tree.hpp"
#include "error.hpp"

namespace fancy_status::fastup
{
    namespace
    {
        namespace
        {
            auto is_hex_char(char x) noexcept -> bool
                // /[0-9a-f]/, lower-case only.
            {
                return (('0' <= x && x <= '9') || ('a' <= x && x <= 'f'));
            }

            auto is_hex_color(String_View x) noexcept -> bool
            {
                if (x.size() != 6) return false;
                for (auto ch: x) if (!is_hex_char(ch)) return false;
                return true;
            }

            auto is_space(char x) noexcept -> bool
                // ' ', '\t', '\n', '\v', '\f', '\r'
            {
                return (x == '\x20' || ('\x09' <= x && x <= '\x0d'));
            }

            auto is_magical(char x) noexcept -> bool
                // '<', '>', '[', ']', '{', '}', '|', '\\'
            {
                switch (x) {
                    case '<': case '>': case '(': case ')': case '[': case ']': case '{': case '}': case '|': case '\\': {
                        return true;
                    }
                }
                return false;
            }

            template <class Fn>
            auto trim_end_if(String_View x, Fn pred) noexcept -> String_View
            {
                auto p = x.end();
                for (auto first=x.begin(); first<p && pred(p[-1]); p--) {}
                return x.reset_last(p);
            }
        }

        // Returns nullptr if success.
        // Returns error token otherwise.
        auto do_scan(Block_List const& token_storage, Block_List const& token_pool, String_View src) noexcept -> Token const*
        {
            auto eat = [&] (size_t n) noexcept -> String_View {
                n = min(src.size(), n);
                return exchange(src, src.remove_prefix(n)).prefix(n);
            };

            auto eat_if = [&] (auto pred) noexcept -> String_View {
                auto p = src.begin();
                for (auto last=src.end(); p<last && pred(*p); p++) {}
                return exchange(src, src.reset_first(p)).reset_last(p);
            };

            auto eat_spaces = [&] () noexcept -> void {
                eat_if(is_space);
            };

            auto push_mark = [&] (String_View mark) noexcept -> void {
                FANCY_STATUS_ASSERT(mark.size() == 1);
                token_pool.alloc_back<Token>().front() = {Token::kind_mark{}, mark.begin()};
            };

            auto push_text = [&] (String_View text) noexcept -> void {
                FANCY_STATUS_ASSERT(text.size() >= 1);
                token_pool.alloc_back<Token>().front() = {Token::kind_text{}, text};
            };

            eat_spaces();
            while (!src.empty()) {
                switch (src.front()) {
                    case '<': {
                        push_mark(eat(1));
                        eat_spaces();

                        auto name = eat_if([] (char x) noexcept -> bool {
                            return (!is_space(x) && !is_magical(x));
                        });
                        if (name.empty()) {
                            build_tokens(token_pool, '^', '$');
                        } else {
                            push_text(name);
                        }
                        eat_spaces();

                        if (src.empty()) {
                            if (name.empty()) {
                                return format_error(token_storage, "missing widget name.");
                            } else {
                                auto ref = Token{Token::kind_text{}, name};
                                return format_error(token_storage, "expecting \"|\" or \">\" after the widget name \"", &ref, "\".");
                            }
                        }

                        switch (src.front()) {
                            case '|': {
                                eat(1);
                                eat_spaces();
                                build_tokens(token_pool, '^');
                                break;
                            }
                            case '>': {
                                push_mark(eat(1));
                                break;
                            }
                            default: {
                                auto ref = Token{Token::kind_text{}, name.reset_last(src.begin() + 1)};
                                return format_error(token_storage, "invalid widget name \"", &ref, "\".");
                            }
                        }
                        break;
                    }

                    case '(': case '[': {
                        push_mark(eat(1));

                        auto color = eat(6);
                        if (!is_hex_color(color)) {
                            if (color.empty()) {
                                return format_error(token_storage, "expecting hex color in rrggbb format.");
                            } else {
                                auto ref = Token{Token::kind_text{}, color};
                                return format_error(token_storage, "expecting hex color in rrggbb format, but got \"", &ref, "\".");
                            }
                        }
                        push_text(color);

                        if (src.empty() || src.front() != ':') {
                            auto ref = Token{Token::kind_text{}, color};
                            return format_error(token_storage, "missing colon \":\" after the color \"", &ref, "\".");
                        }

                        eat(1);
                        eat_spaces();
                        build_tokens(token_pool, '^');
                        break;
                    }

                    case '{': {
                        if (src.size() >= 2 && src[1] == '}') {     // Optimize for using `{}` to keep spaces.
                            eat(2);
                            break;
                        }
                        push_mark(eat(1));
                        build_tokens(token_pool, '^');
                        break;
                    }

                    case '>': case ']': case ')': case '}': {
                        build_tokens(token_pool, '$');
                        push_mark(eat(1));
                        break;
                    }

                    case '|': {
                        eat(1);
                        eat_spaces();
                        build_tokens(token_pool, '$', '^');
                        break;
                    }

                    case '\\': {
                        eat(1);
                        if (!src.empty())
                            push_text(eat(1));
                        break;
                    }

                    default: {
                        auto text = eat_if([] (char x) noexcept -> bool { return !is_magical(x); });
                        if (src.empty() || src.front() == '|' || src.front() == '>')
                            text = trim_end_if(text, is_space);
                        if (!text.empty())
                            push_text(text);
                        break;
                    }
                }
            }

            return nullptr;
        }
    }

    auto parse(Block_List const& token_storage, String_View src) noexcept -> Token const*
    {
        auto token_pool = Block_List{};

        build_tokens(token_pool, '<', '^', '$', '^');
        if (auto error = do_scan(token_storage, token_pool, src))
            return error;
        build_tokens(token_pool, '$', '>');

        auto tokens = token_pool.consolidate_for_data().cast<Token>();
        token_storage.splice_back(move(token_pool));

        return treelize(token_storage, tokens);
    }
}

#include "print.hpp"

namespace fancy_status::fastup
{
    FANCY_STATUS_TEST_CASE("fastup: parse") {
        auto token_storage = Block_List{};
        auto root = parse(token_storage, R"(
            {}    Hello \(w{orl}d (ff0000:  Wow) \)
              |  [003333:\  This is (333333: So) (443333:great)!  ] < gradient | ffff00 | 00ffff >  {}
        )");
        print_tree(root);
    };
}

