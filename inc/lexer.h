#ifndef LEXER_H
#define LEXER_H

enum TOKEN
{
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
    EQUAL
};

struct lexer
{
    int istty;
    int fd;
    char buff[4096];
    short len;
    short offset;
    char iseof;
};

char initLexer(struct lexer* lex, int fd);
char* eatToken(TOKEN toktype);

#endif /* !LEXER_H */
