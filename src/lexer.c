#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "lexer.h"

char initLexer(struct lexer* lex, int fd)
{
    if (!lex || fd < 0)
        return -1;

    lex->istty = isatty(fd);
    lex->fd = fd;
    lex->len = 0;
    lex->offset = 0;
    lex->iseof = 0;
    memset(lex->buff, 0, BUFF_SIZE);

    return 0;
}

static void buffin(struct lexer* lex, short toSave)
{
    if (toSave > lex->len || toSave < 0)
        return;
    if (toSave)
        memmove(lex->buff, lex->buff + len - toSave, toSave);
    lex->len = toSave;
    lex->offset = 0;
    memset(lex->buff + toSave, 0, BUFF_SIZE - toSave);
    lex->len += read(lex->fd, lex->buff + lex->len, BUFF_SIZE - lex->len);
}

static char predicatWord(char carac)
{
    return (carac != ' ' && carac != '\n' && carac != '$' && carac != '\''
            && carac != '\"' && carac != ';' && carac != '?' && carac != '|'
            && carac != '<' && carac != '>'  && carac != '(' && carac != ')'
            && carac != '=');
}

static char eatWord(struct lexer* lex, short* i)
{
    do
    {
        if (lex->offset + *i >= lex->len)
            buffin(lex, *i + 1);
        if (lex->iseof)
            return 1;
        *i += 1;
    } while (predicatWord(lex->buff[offset + *i]));
    *i += 1;
    return 0;
}

static char eatUntil(struct lexer* lex, char carac, short* i)
{
    do
    {
        if (lex->offset + *i >= lex->len)
            buffin(lex, *i + 1);
        if (lex->iseof)
            return 1;
        *i += 1;
    } while (lex->buff[offset + *i] != carac);
    *i += 1;
    return 0;
}

char* eatToken(struct lexer* lex, TOKEN toktype)
{
    char* tok = NULL;
    short i = 0;

    if (!lex || lex->iseof)
        return NULL;

    if (lex->offset >= lex->len)
        buffin(lex, 0);

    switch (toktype)
    {
        case ANY:
            eatWord(lex, &i);
            break;
        default:
            return NULL;
    }
    if (!i)
        return NULL;
    tok = calloc(i, sizeof(char));
    tok = strncpy(tok, lex->buff + lex->offset, i);
    return tok;
}
