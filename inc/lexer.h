#ifndef LEXER_H
#define LEXER_H

#define BUFF_SIZE 4096

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
    char buff[BUFF_SIZE];
    short len;
    short offset;
    char iseof;
};

char initLexer(struct lexer* lex, int fd);
char* eatToken(struct lexer* lex, TOKEN toktype);
//void buffin(struct lexer* lex, short toSave);

#endif /* !LEXER_H */
