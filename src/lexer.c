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
    ssize_t tmp;
    if (toSave >= lex->len || toSave < 0)
        return;
    if (toSave)
        memmove(lex->buff, lex->buff + len - toSave, toSave);
    lex->len = toSave;
    lex->offset = 0;
    memset(lex->buff + toSave, 0, BUFF_SIZE - toSave);
    tmp = read(lex->fd, lex->buff + lex->len, BUFF_SIZE - lex->len);
    if (tmp <= 0 && lex->offset >= lex->len)
    {
        lex->iseof = 1;
        return;
    }
    lex->len += tmp;
}

static char predicatWord(char carac)
{
    return (carac != ' ' && carac != '\n' && carac != '$' && carac != '\''
            && carac != '\"' && carac != ';' && carac != '?' && carac != '|'
            && carac != '<' && carac != '>'  && carac != '(' && carac != ')'
            && carac != '=');
}

static void eatVoid(struct lexer* lex)
{
    while (lex->offset < lex->len && lex->buff[lex->offset] == ' ')
        ++lex->offset;
}

static void getToken(struct lexer* lex, ssize_t* i)
{
    if (*i > 2)
        return WORD;
    if (!*i)
        *i += 1;
    if (*i == 1)
    {
        if (lex->buff[lex->offset] == '\n')
            return EOL;
        if (lex->buff[lex->offset] == ';')
            return SEMICOL;
        if (lex->buff[lex->offset] == '|' && lex->buff[lex->offset + 1] != '|')
            return PIPE;
        if (lex->buff[lex->offset] == '<')
            return RED_LEFT;
        if (lex->buff[lex->offset] == '>' && lex->buff[lex->offset + 1] != '>')
            return RED_RIGHT;
        if (lex->buff[lex->offset] == '$')
            return DOLLAR;
        if (lex->buff[lex->offset] == '?')
            return QUESTION;
        if (lex->buff[lex->offset] == '\"')
            return QUOTE;
        if (lex->buff[lex->offset] == '\'')
            return SIMPLE_QUOTE;
        if (lex->buff[lex->offset] == '(')
            return PARENTHESE_LEFT;
        if (lex->buff[lex->offset] == ')')
            return PARENTHESE_RIGHT;
        if (lex->buff[lex->offset] == '=')
            return EQUAL;
        *i += 1;
    }
    if (*i == 2)
    {
        if (!strncmp(lex->buff + lex->offset, "&&", 2))
            return AND;
        if (!strncmp(lex->buff + lex->offset, "||", 2))
            return OR;
        if (!strncmp(lex->buff + lex->offset, ">>", 2))
            return DOUBLE_RED_RIGHT;
    }
    return WORD;
}

static TOKEN eatWord(struct lexer* lex, ssize_t* i)
{
    if (lex->offset + *i >= lex->len)
        buffin(lex, *i + 1);
    if (lex->iseof)
        return TOKEOF;
    if (!predicatWord(lex->buff[lex->offset]))
        return getToken(lex, i);

    while (predicatWord(lex->buff[offset + *i]))
    {
        if (lex->offset + *i >= lex->len)
            buffin(lex, *i + 1);
        if (lex->iseof)
            return TOKEOF;
        *i += 1;
    }
    return WORD;
}

static TOKEN eatUntil(struct lexer* lex, char carac, ssize_t* i)
{
    if (lex->offset + *i >= lex->len)
        buffin(lex, *i + 1);
    if (lex->iseof)
        return TOKEOF;
    while (lex->buff[offset + *i] != carac)
    {
        if (lex->offset + *i >= lex->len)
            buffin(lex, *i + 1);
        if (lex->iseof)
            return TOKEOF;
        *i += 1;
    }
    return WORD;
}

char* eatToken(struct lexer* lex, TOKEN* toktype)
{
    char* tok = NULL;
    ssize_t i = 0;
    TOKEN tokt = ANY;

    if (!lex || lex->iseof)
        return NULL;

    eatVoid(lex);
    if (lex->offset > lex->len)
        lex->offset = lex->len;
    if (lex->offset + 1 >= lex->len)
    {
        buffin(lex, (lex->offset == lex->len) ? 0 : 1);
        eatVoid(lex);
    }

    switch (*toktype)
    {
        case EOL:
            if (lex->buff[lex->offset] == '\n')
            {
                ++lex->offset;
                *toktype = EOL;
            }
            else
                *toktype = eatWord(lex, &i);
            return NULL;
        case ANY:
            tokt = eatWord(lex, &i);
            break;
        case WORD:
            if (lex->buff[lex->offset] == '\"'
                || lex->buff[lex->offset] == '\'')
            {
                ++lex->offset;
                return eatStr(lex, (lex->buff[lex->offset - 1] == '\'') ?
                              SIMPLE_QUOTE : QUOTE);
            }
            else
                tokt = eatWord(lex, &i);
            break;
        case SEMICOL:
            if (lex->buff[lex->offset] == ';')
            {
                ++lex->offset;
                *toktype = SEMICOL;
            }
            else
                *toktype = eatWord(lex, &i);
            return NULL;
        case AND:
            if (!strncmp(lex->buff + lex->offset, "&&", 2))
            {
                lex->offset += 2;
                *toktype = AND;
            }
            else
                *toktype = eatWord(lex, &i);
            return NULL;
        case OR:
            if (!strncmp(lex->buff + lex->offset, "||", 2))
            {
                lex->offset += 2;
                *toktype = OR;
            }
            else
                *toktype = eatWord(lex, &i);
            return NULL;
        case PIPE:
            if (lex->buff[lex->offset] == '|')
            {
                ++lex->offset;
                *toktype = PIPE;
            }
            else
                *toktype = eatWord(lex, &i);
            return NULL;
        case RED_LEFT:
            if (lex->buff[lex->offset] == '<')
            {
                ++lex->offset;
                *toktype = RED_LEFT;
            }
            else
                *toktype = eatWord(lex, &i);
            return NULL;
        case RED_RIGHT:
            if (lex->buff[lex->offset] == '>')
            {
                ++lex->offset;
                *toktype = RED_RIGHT;
            }
            else
                *toktype = eatWord(lex, &i);
            return NULL;
        case DOUBLE_RED_RIGHT:
            if (!strncmp(lex->buff + lex->offset, ">>", 2))
            {
                lex->offset += 2;
                *toktype = DOUBLE_RED_RIGHT;
            }
            else
                *toktype = eatWord(lex, &i);
            return NULL;
        case DOLLAR:
            if (lex->buff[lex->offset] == '$')
            {
                ++lex->offset;
                *toktype = DOLLAR;
            }
            else
                *toktype = eatWord(lex, &i);
            return NULL;
        case QUESTION:
            if (lex->buff[lex->offset] == '?')
            {
                ++lex->offset;
                *toktype = QUESTION;
            }
            else
                *toktype = eatWord(lex, &i);
            return NULL;
        case QUOTE:
            if (lex->buff[lex->offset] == '\"')
            {
                ++lex->offset;
                *toktype = QUOTE;
            }
            else
                *toktype = eatWord(lex, &i);
            return NULL;
        case SIMPLE_QUOTE:
            if (lex->buff[lex->offset] == '\'')
            {
                ++lex->offset;
                *toktype = SIMPLE_QUOTE;
            }
            else
                *toktype = eatWord(lex, &i);
            return NULL;
        case PARENTHESE_LEFT:
            if (lex->buff[lex->offset] == '(')
            {
                ++lex->offset;
                *toktype = PARENTHESE_LEFT;
            }
            else
                *toktype = eatWord(lex, &i);
            return NULL;
        case PARENTHESE_RIGHT:
            if (lex->buff[lex->offset] == ')')
            {
                ++lex->offset;
                *toktype = PARENTHESE_RIGHT;
            }
            else
                *toktype = eatWord(lex, &i);
            return NULL;
        case EQUAL:
            if (lex->buff[lex->offset] == '=')
            {
                ++lex->offset;
                *toktype = EQUAL;
            }
            else
                return eatWord(lex, &i);
            return NULL;
        default:
            return NULL;
    }
    if (tokt != WORD)
    {
        *toktype = tokt;
        if (tokt == *toktype)
            lex->offset += i;
        return NULL;
    }
    if (!i)
        return NULL;
    if (lex->offset < lex->len && lex->buff[lex->offset] == '=')
    {
        tokt = ASSIGN_WORD;
        ++lex->offset;
    }
    tok = calloc(i + 1, sizeof(char));
    tok = strncpy(tok, lex->buff + lex->offset, i);
    if (tokt == *toktype)
        lex->offset += i;
    toktype = tokt;
    return tok;
}

char *eatStr(struct lexer* lex, TOKEN toktype)
{
    char toto;
    char* str;
    char* tmp;
    ssize_t i = 0;
    ssize_t tmplen;

    if (toktype != QUOTE || toktype != SIMPLE_QUOTE || !lex || lex->iseof)
        return NULL;
    if (lex->offset >= lex->len)
        buffin(lex, 0);
    toto = (toktype == QUOTE) ? '\"' : '\'';
    eatUntil(lex, toto, &i);
    if (!i)
        return NULL;
    str = calloc(i + 1, sizeof(char));
    str = strncpy(str, lex->buff + lex->offset, i);
    lex->offset += i;
    while (lex->offset >= lex->len && !lex->iseof)
    {
        buffin(lex, 0);
        i = 0;
        eatUntil(lex, toto, &i);
        if (!i)
            return str;
        tmplen = strlen(str);
        if (!(tmp = realloc(str, (tmplen + 1 + i) * sizeof(char))))
            return str;
        str = tmp;
        strncpy(str + tmplen, lex->buff + lex->offset, i);
        lex->offset += i;
    }
    if (lex->offset < lex->len)
        ++lex->offset;
    return str;
}
