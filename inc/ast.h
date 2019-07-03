#ifndef AST_H
#define AST_H

#include "lexer.h"

struct AST
{
    TOKEN type;
    char* tok;
    struct AST* sib;
    struct AST* son;
};

#endif /* ! AST_H */
