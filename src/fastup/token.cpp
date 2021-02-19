#include "token.hpp"

namespace fancy_status::fastup
{
    namespace
    {
        static_assert(alignof(Token) == 4*8);
        static_assert( sizeof(Token) == 4*8);
    }

    Token::Token(kind_mark, char const* char_) noexcept: kind{Kind::mark}, first_{char_} {}
    Token::Token(kind_text, String_View text_) noexcept: kind{Kind::text}, first_{text_.begin()}, content_{text_.end()} { FANCY_STATUS_ASSERT(text_.size() >= 1); }

    auto Token::alt() const noexcept -> Token const*
    {
        switch (kind) {
            case Kind::mark: return content_.alt_;
            case Kind::text: return nullptr;
        }
        FANCY_STATUS_BUG();
    }

    auto Token::mutable_alt() const noexcept -> Token const*&
    {
        switch (kind) {
            case Kind::mark: return content_.alt_;
            case Kind::text: FANCY_STATUS_BUG();
        }
        FANCY_STATUS_BUG();
    }

    auto Token::connect_alt(Token const* target) const noexcept -> void
    {
        FANCY_STATUS_ASSERT(target != nullptr);
        FANCY_STATUS_ASSERT(target->is_entering());
        FANCY_STATUS_ASSERT(is_entering());

        auto& entering_alt = mutable_alt();
        auto&  leaving_alt = target->pair->mutable_alt();
        FANCY_STATUS_ASSERT(entering_alt == nullptr);
        FANCY_STATUS_ASSERT( leaving_alt == nullptr);

        entering_alt = target;
         leaving_alt = pair;
    }

    auto Token::text() const noexcept -> String_View
    {
        switch (kind) {
            case Kind::mark: return {first_, size_t{1}};
            case Kind::text: return {first_, content_.last_};
        }
        FANCY_STATUS_BUG();
    }
}

