#include <stdlib.h>
#include "parser.h"

static char parse_list(struct lexer* lex, struct ast* node);
static char ps_and_or(struct lexer* lex, struct ast* node);
static char ps_pipe(struct lexer* lex, struct ast* node);
static char ps_command(struct lexer* lex, struct ast* node);
static char ps_scommand(struct lexer* lex, struct ast* node);
static char ps_pref(struct lexer* lex, struct ast* node);
static char ps_ele(struct lexer* lex, struct ast* node);
static char ps_redi(struct lexer* lex, struct ast* node);

struct ast* parse(struct lexer* lex)
{
    struct ast* root;
    enum TOKEN tokt = EOL;

    root = initnode(ANY, NULL);
    if (parse_list(lex, root))
    {
        destroyTree(root);
        return NULL;
    }
    eatToken(lex, &tokt);
    if (tokt == EOL)
        return root;
    if (lex->iseof)
        return root;
    destroyTree(root);
    return NULL;
}

static char parse_list(struct lexer* lex, struct ast* node)
{
    enum TOKEN tokt = SEMICOL;
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
    enum TOKEN tokt = AND;
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
    enum TOKEN tokt = PIPE;
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
    if (ps_scommand(lex, node))
        return 1;
    return 0;
}

static char ps_scommand(struct lexer* lex, struct ast* node)
{
    char ispref = 0;
    char isele = 0;
    struct ast* ptr = NULL;

    do
    {
        if (ps_pref(lex, node))
            break;
        ispref = 1;
        node->sib = initnode(ANY, NULL);
        ptr = node;
        node = node->sib;
    }
    while (1);

    do
    {
        if (ps_ele(lex, node))
            break;
        isele = 1;
        node->sib = initnode(ANY, NULL);
        ptr = node;
        node = node->sib;
    }
    while (1);
    destroyTree(node);
    if (ptr)
        ptr->sib = NULL;

    return (!(ispref || isele));
}

static char ps_pref(struct lexer* lex, struct ast* node)
{
    enum TOKEN tokt = ASSIGN_WORD;
    char *tok = NULL;

    tok = eatToken(lex, &tokt);
    if (tok && tokt == ASSIGN_WORD)
    {
        node->tok = tok;
        node->type = ASSIGN_WORD;
        return 0;
    }
    if (ps_redi(lex, node))
        return 1;
    return 0;
}

static char ps_ele(struct lexer* lex, struct ast* node)
{
    enum TOKEN tokt = WORD;
    char *tok = NULL;

    tok = eatToken(lex, &tokt);
    if (tok && tokt == WORD)
    {
        node->tok = tok;
        node->type = WORD;
        return 0;
    }
    if (ps_redi(lex, node))
        return 1;
    return 0;
}

static char ps_redi(struct lexer* lex, struct ast* node)
{
    char isionb = 0;
    char* tok;
    enum TOKEN tokt = IONB;

    tok = eatToken(lex, &tokt);
    if (tok && tokt == IONB)
    {
        isionb = 1;
        node->son = initnode(IONB, tok);
    }
    tokt = ANY;
    eatToken(lex, &tokt);
    if (tokt != RED_LEFT && tokt != RED_RIGHT && tokt != DOUBLE_RED_RIGHT)
        return 1;
    node->type = tokt;
    eatToken(lex, &tokt);
    tokt = WORD;
    tok = eatToken(lex, &tokt);
    if (!tok || tokt != WORD)
        return 1;
    if (isionb)
        node->son->sib = initnode(WORD, tok);
    else
        node->son = initnode(WORD, tok);

    return 0;
}
