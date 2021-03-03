#include "tree.hpp"
#include "mark.hpp"
#include "error.hpp"

namespace fancy_status::fastup
{
    auto treelize(Block_List const& token_storage, Span<Token> tokens) noexcept -> Token const*
    {
        FANCY_STATUS_ASSERT(tokens.size() >= 2);

        auto back_leaving = (Token const*) nullptr;
        for (auto entering=tokens.end()-1; entering>=tokens.begin(); entering--) {
            switch (is_pairable(entering)) {
                case Is_Pairable::no: continue;
                case Is_Pairable::entering: break;
                case Is_Pairable::leaving: {
                    if (back_leaving == nullptr)
                        back_leaving = entering;
                    continue;
                }
            }

            auto paired = paired_char_of(entering->front());
            auto leaving = entering + 1;
            for (
                ; leaving < tokens.end()
                ; leaving = const_cast<Token*>(leaving->next_sibling())   // The source being Span of non-const Tokens makes this cast legal.
            ) {
                switch (is_pairable(leaving)) {
                    case Is_Pairable::no: continue;
                    case Is_Pairable::entering: {
                        if (leaving->pair == nullptr) {
                            break;
                        } else {
                            continue;
                        }
                    }
                    case Is_Pairable::leaving: break;
                }

                if (leaving->front() != paired)
                    return format_error(token_storage, "token \"", entering, "\" and \"", leaving, "\" is not a pair.");

                leaving->pair = entering;
                entering->pair = leaving;
                break;
            }

            if (leaving == tokens.end())
                return format_error(token_storage, "missing leaving token for \"", entering, "\".");
        }

        if (back_leaving && back_leaving->pair == nullptr)
            return format_error(token_storage, "missing entering token for \"", back_leaving, "\".");

        return &tokens.front();
    }
}

