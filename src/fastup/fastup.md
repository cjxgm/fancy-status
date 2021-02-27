Fastup Syntax and Token Tree
============================

Prerequisite:
- Understand [Token Tree Data Structure](./token.md)

An example Fastup expression:

>     {}    Hello \(w{orl}d (ff0000:  Wow) \)
>       |  [003333:\  This is (333333: So) (443333:great)!  ] < gradient | ffff00 | 00ffff >  {}

Features:
- **Escaping**: Backslash "`\`" removes magic from the following character.
- **Text Color**: `(rrggbb: Text)`. Spaces after the colon "`:`" will be removed unless escaped.
- **Fill Color**: `[rrggbb: Text]`. Spaces after the colon "`:`" will be removed unless escaped.
- **Emphasis**: `{Text}`. Empty emphasis `{}` can be used to prevent the removal of spaces.
- **Widgets**: `<widget|arg0|arg1|arg2|...>`. Spaces around the pipe "`|`" will be removed unless escaped. Spaces after "`<`" and spaces before "`>`" will be removed unless escaped.

The above example expression will be parsed into a token tree like this:

>     mark <> ╾╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌ An expression of `hello` is actually `<|hello>`
>     ├─ mark ^$
>     ├─ mark ^$
>     │  ├─ text "    Hello "
>     │  ├─ text "(" ╾╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌ Escaped character will have its own token.
>     │  ├─ text "w"
>     │  ├─ mark {}
>     │  │  └─ mark ^$
>     │  │     └─ text "orl"
>     │  ├─ text "d "
>     │  ├─ mark ()
>     │  │  ├─ text "ff0000"
>     │  │  └─ mark ^$
>     │  │     └─ text "Wow" ╾╌╌╌╌╌╌╌╌╌╌╌ NOTICE that there is NO leading space.
>     │  ├─ text " "
>     │  └─ text ")"
>     └─ mark ^$
>        ├─ mark []
>        │  ├─ text "003333" ╾╌╌╌╌╌╌╌╌╌╌╌ NOTICE that there is NO colon after this.
>        │  └─ mark ^$
>        │     ├─ text " " ╾╌╌╌╌╌╌╌╌╌╌╌╌╌ The escaped space.
>        │     ├─ text " This is " ╾╌╌╌╌╌ The leading space is kept due to previous escaping.
>        │     ├─ mark ()
>        │     │  ├─ text "333333"
>        │     │  └─ mark ^$
>        │     │     └─ text "So"
>        │     ├─ text " "
>        │     ├─ mark ()
>        │     │  ├─ text "443333"
>        │     │  └─ mark ^$
>        │     │     └─ text "great"
>        │     └─ text "!  " ╾╌╌╌╌╌╌╌╌╌╌╌ NOTICE that there ARE two trailing spaces.
>        ├─ text " "
>        ├─ mark <>
>        │  ├─ text "gradient"
>        │  ├─ mark ^$
>        │  │  └─ text "ffff00"
>        │  └─ mark ^$
>        │     └─ text "00ffff"
>        └─ text "  "

And it can be inferred from the above that, you can actually write pipes outside widgets markup:

>     hello | world

which is equivalent to `<|hello | world>`, which is equivalent to `<|helloworld>`.

The empty-named widget `<>` concats all of its arguments without delimiters, and remember, pipes removes surrounding spaces.

Spaces in Fastup are defined as a sequence of UTF-8 code units:

>     - 0x09 '\t'
>     - 0x0a '\n'
>     - 0x0b '\v'
>     - 0x0c '\f'
>     - 0x0d '\r'
>     - 0x20 ' '

Or, `[\x09-\x0d\x20]+` in regex.

