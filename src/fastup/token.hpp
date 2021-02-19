#pragma once
// Token Tree Data Structure.
// See `token.md` for detailed explanation.

namespace fancy_status::fastup
{
    struct alignas(4*8) Token final
        // Guarantees this->text().size() >= 1.
    {
        enum struct Kind: uint64_t
        {
            mark,
            text,
        };

        struct kind_mark final {};
        struct kind_text final {};

        Kind kind{};
        Token const* pair{};

        char const* first_{};
        union Content_ {
            mutable Token const* alt_;  // active when `kind` is one of: mark
            char const* last_;          // active when `kind` is one of: text

            Content_() noexcept: alt_{nullptr} {}
            Content_(Token const* content) noexcept: alt_{content} {}
            Content_(char const* content_end) noexcept: last_{content_end} {}
        } content_;

        Token(kind_mark, char const* char_) noexcept;
        Token(kind_text, String_View text_) noexcept;

        auto alt() const noexcept -> Token const*;
        auto mutable_alt() const noexcept -> Token const*&;
        auto connect_alt(Token const* target) const noexcept -> void;
        auto text() const noexcept -> String_View;

        auto front() const noexcept -> char { return first_[0]; }       // Always valid due to invariants of the Token.

        auto entering() const noexcept -> Token const* { return min(this, (pair == nullptr ? this : pair)); }
        auto  leaving() const noexcept -> Token const* { return max(this, pair); }

        auto is_entering() const noexcept -> bool { return (this < pair); }
        auto  is_leaving() const noexcept -> bool { return (pair && pair < this); }

        auto next() const noexcept -> Token const* { return this + 1; }
        auto prev() const noexcept -> Token const* { return this - 1; }

        auto next_sibling() const noexcept -> Token const* { return  leaving() + 1; };
        auto prev_sibling() const noexcept -> Token const* { return entering() - 1; };

        auto tree() const noexcept -> Span<Token const> { return {entering(), leaving() + 1}; }
        auto descendants() const noexcept -> Span<Token const> { return {entering() + 1, leaving()}; }
    };

    inline auto operator == (Token const* tk, Token::kind_mark) noexcept -> bool { return (tk->kind == Token::Kind::mark); }
    inline auto operator == (Token const* tk, Token::kind_text) noexcept -> bool { return (tk->kind == Token::Kind::text); }
}

