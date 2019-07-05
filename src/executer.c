#define _POSIX_C_SOURCE 200112L

#include <err.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "executer.h"
#include "glob.h"
#include "builtin.h"
#include "tools.h"

static void exec_cmd(int argc, char** argv)
{
    char str[4] = { 0 };
    int statval;
    pid_t pid;

    if (isbuiltin(argc, argv))
      return;
    if ((pid = fork()))
    {
        wait(&statval);
        if (WIFEXITED(statval))
        {
            statval = WEXITSTATUS(statval);
            itoa(statval, str);
            setenv("?", str, 1);
        }
    }
    else
    {
        execvp(argv[0], argv);
        errx(127, "%s: command not found", *argv);
    }
}

static void ex_word(struct ast* r)
{
    int argc = nbbro(r);
    char** argv = calloc(argc + 1, sizeof(char*));
    struct ast* n = r;

    for (int i = 0; i < argc; ++i)
    {
        argv[i] = n->tok;
        n = n->sib;
    }
    exec_cmd(argc, argv);
    free(argv);
}

static void ex_semicol(struct ast* r)
{
    struct ast *e = r->son;

    if (e && e->type == WORD)
        execute(e);
    else
        while (e)
        {
            execute(e);
            e = e->sib;
        }
}

static void ex_pipe(struct ast* r)
{
    if (!r)
       return;
    //////////////////// PIPEULRLUUU
    if (r->son)
        execute(r->son);
    else
        return;
    if (r->son->sib)
        execute(r->son->sib);
}

static void ex_and(struct ast* r)
{
    if (!r)
        return;
    if (r->son)
        execute(r->son);
    else
        return;
    if (r->son->sib)
        execute(r->son->sib);
}

static void ex_or(struct ast* r)
{
    if (!r)
        return;
    ////////////////// OR
    if (r->son)
        execute(r->son);
    else
        return;
    if (r->son->sib)
        execute(r->son->sib);
}

static void ex_assW(struct ast* r)
{
    if (!r || !r->sib)
        return;
    setenv(r->tok, r->sib->tok, 1);
}

void execute(struct ast* r)
{
    if (!r || mustexit)
        return;
    switch (r->type)
    {
    case ANY:
    case SEMICOL:
        ex_semicol(r);
        break;
    case PIPE:
        ex_pipe(r);
        break;
    case AND:
        ex_and(r);
        break;
    case OR:
        ex_or(r);
        break;
    case WORD:
        ex_word(r);
        break;
    case ASSIGN_WORD:
        ex_assW(r);
        break;
    default:
        return;
    }
}
