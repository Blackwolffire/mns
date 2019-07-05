#include <err.h>
#include <>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "builtin.h"
#include "glob.h"

static void echo(int argc, char** argv);
static void cd(int argc, char** argv);
static void killer(int argc, char** argv);

char isbuiltin(int argc, char** argv)
{
    if (!strcmp("cd", argv[0]))
    {
        cd(argc, argv);
        return 1;
    }
    if (!strcmp("echo", argv[0]))
    {
        echo(argc, argv);
        return 1;
    }
    if (!strcmp("exit", argv[0]))
    {
        mustexit = 1;
        return 1;
    }
    if (!strcmp("kill", argv[0]))
    {
        killer(argc, argv);
        return 1;
    }

    return 0;
}

static void echo(int argc, char** argv)
{
    char isn = 0;

    if (argc > 1 && !strcmp(argv[1], "-n"))
        isn = 1;
    for (int i = 1 + isn; i < argc; ++i)
    {
        write(STDOUT_FILENO, argv[i], strlen(argv[i]));
        if (i < argc - 1)
            write(STDOUT_FILENO, " ", 1);
    }
    if (!isn)
        write(STDOUT_FILENO, "\n", 1);
}

static void cd(int argc, char** argv)
{
    if (argc == 1)
        chdir(getenv("HOME"));
    else if (argc > 2)
    {
        warn("cd: too many arguments");
    }
    else
    {
        if (argv[1][0] == '-' && !argv[1][1])
            chdir(getenv("OLDPWD"));
        else if (chdir(argv[1]))
            warn("cd: %s: No such file or directory", argv[1]);
    }
}

static void killer(int argc, char** argv)
{
    int sig = 9;
    if (argc < 2)
        warnx("kill: usage: kill [-s sigspec | -n signum | -sigspec] pid | "
              "jobspec ... or kill -l [sigspec]");
    else if (argc == 2)
    {
        if (kill(strtol(argv[1], NULL, 10), 9))
            warn("kill: (%s) - No such process", argv[1]);
    }
    else
    {
        sig = strtol(argv[1], NULL, 10);
        for (int i = 2; i < argc; ++i)
            if (kill(strtol(argv[i], NULL, 10), sig))
                warn("kill: (%s) - No such process", argv[i]);
    }
}
