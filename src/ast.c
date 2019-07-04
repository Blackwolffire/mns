#include <stdlib.h>
#include "ast.h"

struct ast* initnode(TOKEN type, char* cmd)
{
    struct ast* node = malloc(sizeof(struct ast));
    node->type = type;
    node->tok = cmd;
    node->sib = NULL;
    node->son = NULL;
}

void destroyTree(struct ast* node)
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
