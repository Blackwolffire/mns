#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "parser.h"
#include "executer.h"

static int getfd(char* path)
{
    if (!path)
        return -1;
    return open(path, O_RDONLY);
}

int main(int argc, char** argv)
{
    char isopen = 0;
    int fdin = STDIN_FILENO;
    struct lexer lex;

    if (argc > 1)
    {
        fdin = getfd(argv[1]);
        isopen = 1;
    }
    if (fdin < 0)
        err(127, "%s: No such file or directory", argv[1]); // watch memleak
    initLexer(&lex, fdin);
    struct ast* tree = parse(&lex);
    execute(tree);
    destroyTree(tree);
    if (isopen)
        close(fdin);

    return 0;
}
