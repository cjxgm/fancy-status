#include "registry.hpp"

namespace fancy_status::renderer
{
    namespace
    {
        auto token_kind_name(fastup::Token::Kind kind) noexcept -> String_View
        {
            switch (kind) {
                case fastup::Token::Kind::mark: return "mark";
                case fastup::Token::Kind::text: return "text";
            }
            FANCY_STATUS_BUG();
        }

        auto render(Block_List const& output_storage, fastup::Token const* document) noexcept -> String_View
        {
            auto pool = Block_List{};
            auto add = [&] (String_View x) noexcept -> void {
                copy(x.as_bytes(), pool.alloc_back<byte>(x.size()));
            };

            auto level_no_siblings = output_storage.alloc_back<bool>(document->tree().size() / 2 + 1).cast<bool>();
            auto indent_level = 0;
            for (auto& tk: document->tree()) {
                if (tk.is_leaving()) {
                    indent_level--;
                } else {
                    auto next_sib = (&tk == document->pair ? nullptr : tk.next_sibling());
                    auto is_last_child = (next_sib == nullptr || next_sib->is_leaving());

                    if (indent_level >= 1) {
                        if (indent_level >= 2) {
                            for (auto no_sib: level_no_siblings.prefix(indent_level).remove_prefix(1))
                                add((no_sib ? String_View{"   "} : String_View{"│  "}));
                        }
                        add((is_last_child ? "└─ " : "├─ "));
                    }

                    add(token_kind_name(tk.kind));
                    add(" ");
                    if (tk.pair > &tk) {
                        add(tk.text());
                        add(tk.pair->text());

                        if (is_last_child) level_no_siblings[indent_level] = true;
                        indent_level++;
                        level_no_siblings[indent_level] = false;
                    } else {
                        add("\"");
                        add(tk.text());
                        add("\"");
                    }
                    add("\n");
                }
            }

            pool.alloc_back<char>();            // allocate for '\0'.
            auto output = String_View{pool.consolidate_for_data().cast<char const>()};
            output_storage.splice_back(move(pool));
            return output.remove_suffix(1);     // remove the last '\n'.
        }

        FANCY_STATUS_RENDERER_ADD("tree", &render);
    }
}

#include "fastup/parse.hpp"

namespace fancy_status::renderer
{
    FANCY_STATUS_TEST_CASE("renderer: tree") {
        auto storage = Block_List{};
        auto document = fastup::parse(storage, R"doc(
            {}    Hello \(w{orl}d (ff0000:  Wow) \)
              |  [003333:\  This is (333333: So) (443333:great)!  ] < gradient | ffff00 | 00ffff >  {}
        )doc");
        auto document_tree = render(storage, document);
        constexpr auto document_tree_truth = String_View{R"tree(mark <>
├─ mark ^$
├─ mark ^$
│  ├─ text "    Hello "
│  ├─ text "("
│  ├─ text "w"
│  ├─ mark {}
│  │  └─ mark ^$
│  │     └─ text "orl"
│  ├─ text "d "
│  ├─ mark ()
│  │  ├─ text "ff0000"
│  │  └─ mark ^$
│  │     └─ text "Wow"
│  ├─ text " "
│  └─ text ")"
└─ mark ^$
   ├─ mark []
   │  ├─ text "003333"
   │  └─ mark ^$
   │     ├─ text " "
   │     ├─ text " This is "
   │     ├─ mark ()
   │     │  ├─ text "333333"
   │     │  └─ mark ^$
   │     │     └─ text "So"
   │     ├─ text " "
   │     ├─ mark ()
   │     │  ├─ text "443333"
   │     │  └─ mark ^$
   │     │     └─ text "great"
   │     └─ text "!  "
   ├─ text " "
   ├─ mark <>
   │  ├─ text "gradient"
   │  ├─ mark ^$
   │  │  └─ text "ffff00"
   │  └─ mark ^$
   │     └─ text "00ffff"
   └─ text "  ")tree"};
        print(document_tree, "\n");
        FANCY_STATUS_TEST_CHECK_EQ(document_tree, document_tree_truth);
    };
}

