#include "print.hpp"

namespace fancy_status::fastup
{
    auto print_tree(Token const* x) noexcept -> void
    {
        if (x->pair == nullptr) {
            print(x, "\n");
            return;
        }

        Block_List pending_branches;
        pending_branches.alloc_back<Token const*>().front() = x;

        while (!pending_branches.empty()) {
            auto branches = exchange(pending_branches, {});
            for (auto branch: branches.consolidate_for_data().cast<Token const*>()) {
                print("    BRANCH <<", static_cast<void const*>(branch), " ", static_cast<void const*>(branch->pair), ">>\n");
                for (auto& tk: branch->tree()) {
                    print(&tk);
                    if (auto pending_branch = tk.alt()) {
                        if (pending_branch->is_entering()) {
                            pending_branches.alloc_back<Token const*>().front() = pending_branch;
                            print("    ALT ENTER ", static_cast<void const*>(pending_branch));
                        } else {
                            print("    ALT LEAVE ", static_cast<void const*>(pending_branch));
                        }
                    }
                    print("\n");
                }
            }
        }
    }
}

namespace fancy_status::printer
{
    namespace
    {
        using fastup::Token;

        auto kind_name(Token::Kind x) noexcept -> String_View
        {
            switch (x) {
                case Token::Kind::mark: return "mark";
                case Token::Kind::text: return "text";
            }
            FANCY_STATUS_BUG();
        }

        auto quote_for(Token::Kind x) noexcept -> char
        {
            switch (x) {
                case Token::Kind::mark: return '\'';
                case Token::Kind::text: return '"';
            }
            FANCY_STATUS_BUG();
        }
    }

    template <>
    auto do_print_into<Token const*>(String_Slice buf, Token const* x) noexcept -> String_Slice
    {
        auto quote = quote_for(x->kind);
        return chain_print_into(buf
            , static_cast<void const*>(x)
            , " ->"
            , static_cast<void const*>(x->pair)
            , " "
            , kind_name(x->kind)
            , " "
            , quote
            , x->text()
            , quote
        );
    }

    template <>
    auto do_print_into<Token*>(String_Slice buf, Token* x) noexcept -> String_Slice
    {
        return do_print_into(buf, const_cast<Token const*>(x));
    }
}

