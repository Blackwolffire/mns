#include <stdlib.h>
#include "parser.h"

struct AST* parse(struct lexer* lex)
{
    struct AST* root;
    TOKEN tokt = ANY;
    char* tok = eatToken(lex, &tokt);
    if (tokt == WORD)
    {
        root = initnode(WORD, tok);
    }
}
