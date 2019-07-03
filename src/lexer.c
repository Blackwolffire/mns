#include <stdlib.h>
#include <unistd.h>
#include "lexer.h"

char initLexer(struct lexer* lex, int fd)
{
    if (!lex || fd < 0)
        return EXIT_FAILURE;

    lex->istty = isatty(fd);
    lex->fd = fd;
    lex->len = 0;
    lex->offset = 0;
    lex->iseof = 0;
    memset(lex->buff, 0, 4096);

    return EXIT_SUCCESS;
}

char* eatToken(TOKEN toktype)
{

}
