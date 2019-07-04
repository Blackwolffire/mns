#ifndef ast_H
#define ast_H

#include "lexer.h"

struct ast
{
    enum TOKEN type;
    char* tok;
    struct ast* sib;
    struct ast* son;
};

struct ast* initnode(enum TOKEN type, char* cmd);
void destroyTree(struct ast* node);
int nbbro(struct ast* n);

#endif /* ! ast_H */
