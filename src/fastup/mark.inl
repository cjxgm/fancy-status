// No #pragma once intentionally.

#ifndef FANCY_STATUS_FASTUP_DEFINE_MARK
    // ENTER_CHAR cannot be the same as LEAVE_CHAR.
    // ENTER_CHAR and LEAVE_CHAR should be unique across all entries.
    //
    // KIND defines special property for marks.
    // - `hidden` indicates that the symbols won't appear in the Token::text().
    // - `literal` indicates that the symbols will appear in the Token::text().
    #define FANCY_STATUS_FASTUP_DEFINE_MARK(ENTER_CHAR, LEAVE_CHAR, NAME, KIND)
#endif

FANCY_STATUS_FASTUP_DEFINE_MARK('^', '$', group, hidden);
FANCY_STATUS_FASTUP_DEFINE_MARK('<', '>', widget, literal);
FANCY_STATUS_FASTUP_DEFINE_MARK('(', ')', text_color, literal);
FANCY_STATUS_FASTUP_DEFINE_MARK('[', ']', fill_color, literal);
FANCY_STATUS_FASTUP_DEFINE_MARK('{', '}', emphasis, literal);

#undef FANCY_STATUS_FASTUP_DEFINE_MARK

