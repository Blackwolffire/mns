#ifndef ast_H
#define ast_H

#include "lexer.h"

struct ast
{
    TOKEN type;
    char* tok;
    struct ast* sib;
    struct ast* son;
};

#endif /* ! ast_H */
