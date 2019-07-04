#include <stdlib.h>
#include "ast.h"

struct AST* initnode(TOKEN type, char* cmd)
{
    struct AST* node = malloc(sizeof(struct AST));
    node->type = type;
    node->tok = cmd;
    node->sib = NULL;
    node->son = NULL;
}

void destroyTree(struct AST* node)
{
    if (!node)
        return;
    if (node->son)
        destroyTree(node->son);
    if (node->sib)
        destroyTree(node->sib);
    if (node->tok)
        free(node->tok);
    free(node);
}
