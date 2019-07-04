#include <stdlib.h>
#include "parser.h"

struct ast* parse(struct lexer* lex)
{
    struct ast* root;
    TOKEN tokt = EOL;

    root = initnode(ANY, NULL);
    if (parse_list(lex, root))
    {
        destroyTree(root);
        return NULL;
    }
    eatToken(lex, &tokt)
    if (tokt == EOL)
        return root;
    if (lex->iseof)
        return root;
    destroyTree(root);
    return NULL;
}

static char parse_list(struct lexer* lex, struct ast* node)
{
    TOKEN tokt = SEMICOL;
    struct ast* son = initnode(ANY, NULL);
    struct ast* sib;

    node->type = SEMICOL;
    if (ps_and_or(lex, son))
    {
        destroyTree(son);
        return 1;
    }
    node->son = son;
    eatToken(lex, &tokt);
    while (tokt == SEMICOL)
    {
        sib = son;
        son = initnode(ANY, NULL);
        if (ps_and_or(lex, son))
        {
            destroyTree(son);
            return 0;
        }
        sib->sib = son;
        eatToken(lex, &tokt);
    }
    return 0;
}

static char ps_and_or(struct lexer* lex, struct ast* node)
{
    TOKEN tokt = AND;
    struct ast* son = initnode(ANY, NULL);
    struct ast* dad;

    node->type = SEMICOL;
    if (ps_pipe(lex, son))
    {
        destroyTree(son);
        return 1;
    }
    node->son = son;
    eatToken(lex, &tokt);
    while (tokt == AND || tokt == OR)
    {
        if (tokt == OR)
            eatToken(lex, &tokt);
        dad = initnode(tokt, NULL);
        dad->son = node->son;
        node->son = dad;
        tokt = EOL;
        do
        {
            eatToken(lex, &tokt);
        }
        while (tokt == EOL);
        son = initnode(ANY, NULL);
        if (ps_pipe(lex, son))
        {
            destroyTree(son);
            return 1;
        }
        dad->son->sib = son;
    }
    return 0;
}

static char ps_pipe(struct lexer* lex, struct ast* node)
{
    TOKEN tokt = PIPE;
    struct ast* ptr = initnode(ANY, NULL);

    if (ps_command(lex, ptr))
        return 1;
    node->son = ptr;

    eatToken(lex, &tokt);
    while (tokt == PIPE)
    {
        tokt = EOL;
        do
        {
            eatToken(lex, &tokt);
        }
        while (tokt == EOL);

        ptr = initnode(PIPE, NULL);
        ptr->son = node->son;
        node->son = ptr;
        ptr = initnode(ANY, NULL);
        if (ps_command(lex, ptr))
        {
            destroyTree(ptr);
            return 1;
        }
        node->son->son->sib = ptr;

        tokt = PIPE;
        eatToken(lex, &tokt);
    }
    return 0;
}

static char ps_command(struct lexer* lex, struct ast* node)
{
}
