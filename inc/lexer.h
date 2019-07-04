#ifndef LEXER_H
#define LEXER_H

#include <sys/types.h>

#define BUFF_SIZE 4096

enum TOKEN
{
    TOKEOF,
    EOL,
    ANY,
    WORD,
    SEMICOL,
    AND,
    OR,
    PIPE,
    RED_LEFT,
    RED_RIGHT,
    DOUBLE_RED_RIGHT,
    DOLLAR,
    QUESTION,
    QUOTE,
    SIMPLE_QUOTE,
    PARENTHESE_LEFT,
    PARENTHESE_RIGHT,
    EQUAL,
    ASSIGN_WORD,
    IONB
};

struct lexer
{
    int istty;
    int fd;
    char buff[BUFF_SIZE];
    ssize_t len;
    ssize_t offset;
    char iseof;
};

char initLexer(struct lexer* lex, int fd);
char* eatToken(struct lexer* lex, enum TOKEN toktype);

#endif /* !LEXER_H */
