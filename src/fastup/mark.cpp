#include "mark.hpp"

namespace fancy_status::fastup
{
    namespace
    {
        #define FANCY_STATUS_FASTUP_DEFINE_MARK(ENTER_CHAR, LEAVE_CHAR, NAME, KIND) \
            constexpr auto NAME##_enter_mark = char{ENTER_CHAR}; \
            constexpr auto NAME##_leave_mark = char{LEAVE_CHAR}; \
        ;
        #include "mark.inl"
    }

    auto is_pairable(Token const* tk) noexcept -> Is_Pairable
    {
        switch (tk->kind) {
            case Token::Kind::mark: {
                switch (tk->front()) {
                    #define FANCY_STATUS_FASTUP_DEFINE_MARK(ENTER_CHAR, LEAVE_CHAR, NAME, KIND) \
                        case NAME##_enter_mark: return Is_Pairable::entering; \
                        case NAME##_leave_mark: return Is_Pairable::leaving; \
                    ;
                    #include "mark.inl"
                }
                return Is_Pairable::no;
            }

            case Token::Kind::text: {
                return Is_Pairable::no;
            }
        }

        FANCY_STATUS_BUG();
    }

    auto paired_char_of(char pairable) noexcept -> char
    {
        switch (pairable) {
            #define FANCY_STATUS_FASTUP_DEFINE_MARK(ENTER_CHAR, LEAVE_CHAR, NAME, KIND) \
                case NAME##_enter_mark: return NAME##_leave_mark; \
                case NAME##_leave_mark: return NAME##_enter_mark; \
            ;
            #include "mark.inl"
        }
        FANCY_STATUS_BUG();
    }

    auto token_from_pairable(char pairable) noexcept -> Token
    {
        switch (pairable) {
            #define FANCY_STATUS_FASTUP_DEFINE_MARK(ENTER_CHAR, LEAVE_CHAR, NAME, KIND) \
                case NAME##_enter_mark: return Token{Token::kind_mark{}, &NAME##_enter_mark}; \
                case NAME##_leave_mark: return Token{Token::kind_mark{}, &NAME##_leave_mark}; \
            ;
            #include "mark.inl"
        }
        FANCY_STATUS_BUG();
    }
}

