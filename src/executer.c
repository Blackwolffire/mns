#define _POSIX_C_SOURCE 200112L

#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "executer.h"
#include "glob.h"
#include "builtin.h"
#include "tools.h"

#include <stdio.h>

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
        printf("frr %s \n !!!!!!!! \n", argv[0]);
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
        if (n->type != WORD)
        {
            argc = i;
            break;
        }
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

static int getionb(struct ast* r)
{
    if (!r || r->type != IONB)
        return 0;
    if (r->tok[0] == '1')
        return 1;
    if (r->tok[0] == '2')
        return 2;
    return 0;
}

static void ex_redr(struct ast* r)
{
    int savein;
    int saveout;
    int tmp = 0;
    int fd;

    if (!r || !r->son)
        return;
    if (r->son->type == SEMICOL && ((r->son->sib->type == IONB
                                     && r->son->sib->sib)
                                    || r->son->sib->type == WORD))
    {
        savein = dup(STDIN_FILENO);
        saveout = dup(STDOUT_FILENO);
        switch (r->type)
        {
        case RED_RIGHT:
            if (r->son->sib->type == IONB)
            {
                if (!(tmp = getionb(r->son->sib)))
                {
                    fd = open(r->son->sib->tok, O_CREAT | OWRONLY
                                 | O_TRUNC);
                    dup2(fdout, STDOUT_FILENO);
                    execute(r->son->son);
                    close(fd);
                }
            }
            break;
        default :
            break;
        }

        dup2(savein, STDIN_FILENO);
        dup2(saveout, STDOUT_FILENO);
        close(savein);
        close(saveout);
    }
    else if (r->son)
    {
    }

    execute(r->sib);
}

static void ex_pipe(struct ast* r)
{
    char str[4] = { 0 };
    int statval;
    int fdp[2];
    int savein;
    pid_t pid;

    if (!r || !r->son)
       return;
    if (r->son && !r->son->sib)
        execute(r->son);
    else if (r->son->sib)
    {
        savein = dup(STDIN_FILENO);
        pipe(fdp);

        if (!(pid = fork()))
        {
            close(fdp[0]);
            dup2(fdp[1], STDOUT_FILENO);
            execute(r->son);
            close(savein);
            close(fdp[1]);
            exit(strtol(getenv("?"), NULL, 10));
            errx(-1, "what the duck?!");
        }
        else
        {
            close(fdp[1]);
            dup2(fdp[0], STDIN_FILENO);
            execute(r->son->sib);
            dup2(savein, STDIN_FILENO);
            close(fdp[0]);
            close(savein);
            wait(&statval);
            if (WIFEXITED(statval))
            {
                statval = WEXITSTATUS(statval);
                itoa(statval, str);
                setenv("?", str, 1);
            }
        }
    }
}

static void ex_and(struct ast* r)
{
    if (!r)
        return;
    if (r->son)
        execute(r->son);
    else
        return;
    if (strcmp("0", getenv("?")))
        return;
    if (r->son->sib)
        execute(r->son->sib);
}

static void ex_or(struct ast* r)
{
    if (!r)
        return;
    if (r->son)
        execute(r->son);
    else
        return;
    if (!strcmp("0", getenv("?")))
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
    case RED_RIGHT:
        ex_redr(r);
        break;
    case RED_LEFT:
        ex_redl(r);
        break;
    case DOUBLE_RED_RIGHT:
        ex_dredr(r);
        break;
    default:
        return;
    }
}
