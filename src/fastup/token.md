Token Tree Data Structure
=========================

A token tree is a stream of continuously-stored tokens who caches paired tokens.
Below is an illustration of an example token tree.

>     [0]  [1] [2] [3] [4] [5]     [6] [7] [8]   [9]  [10] [11] [12]         tokens[13] array
>     this is   {  an   (  example of   )  token tree  }   data structure
>               ^       ^---- pair -----^              ^
>               `---------------- pair ----------------'

Pairs (round and curly brackets in the above example) form subtrees.
It can be drawn like a tree below:

>     ROOT                                       |    ROOT
>     \___________________                       |    |- [0] this
>      \    \   \    \    \                      |    |- [1] is
>      [0]  [1] [2]  [11] [12]                   |    |- [2] {} [10]
>      this is    \  data structure              |    |   |- [3] an
>                 {\}                            |    |   |- [4] () [7]
>                   \______________              |    |   |   |- [5] example
>                    \   \   \     \             |    |   |   `- [6] of
>                    [3] [4] [8]   [9]           |    |   |- [8] token
>                    an    \ token tree          |    |   `- [9] tree
>                          (\)                   |    |- [11] data
>                            \________           |    `- [12] structure
>                             \       \          |
>                             [5]     [6]        |
>                             example of         |

It is guaranteed that for a non-`nullptr` token `tk`, we have `tk->pair->pair == tk` if `tk->pair != nullptr`.

For a non-`nullptr` token `tk`,
- if `tk < tk->pair`, we call it an *entering token*, because it enters a subtree;
- if `tk > tk->pair && tk->pair != nullptr`, we call it an *leaving  token*, because it leaves a subtree;
- `tk` cannot be equal to `tk->pair`.

If `tk->pair == nullptr`, `tk` cannot have any child.
If `tk->pair == tk + 1`, `tk` can have children but has none.

To enter a subtree (access its children), just take the next token, like `tk + 1`.
To access the sibling of a token (skip its children), make use of its `pair`, like `tk->pair + 1`.
Please use `tk->next()` and `tk->next_sibling()` in real world situations instead.

As illustrated above, `tokens[2].next_sibling() == &tokens[11]`,
and likewise, `tokens[4].next_sibling() == &tokens[8]`.

