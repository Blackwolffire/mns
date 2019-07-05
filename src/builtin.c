#define _POSIX_C_SOURCE 210112L

#include <err.h>
#include <ctype.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "builtin.h"
#include "glob.h"
#include "tools.h"

static int echo(int argc, char** argv);
static int cd(int argc, char** argv);
static int killer(int argc, char** argv);
static int myexit(int argc, char** argv);

char isbuiltin(int argc, char** argv)
{
    char isbui = 0;
    char str[4] = { 0 };
    int ret;

    if (!strcmp("cd", argv[0]))
    {
        ret = cd(argc, argv);
        isbui = 1;
    }
    if (!strcmp("echo", argv[0]))
    {
        ret = echo(argc, argv);
        isbui = 1;
    }
    if (!strcmp("exit", argv[0]))
    {
        if (myexit(argc, argv))
            mustexit = 1;
        return 1;
    }
    if (!strcmp("kill", argv[0]))
    {
        ret = killer(argc, argv);
        isbui = 1;
    }

    if (isbui)
    {
        itoa(ret, str);
        setenv("?", str, 1);
    }
    return isbui;
}

static int echo(int argc, char** argv)
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
    return 0;
}

static int cd(int argc, char** argv)
{
    if (argc == 1)
        chdir(getenv("HOME"));
    else if (argc > 2)
    {
        warnx("cd: too many arguments");
        return 1;
    }
    else
    {
        if (argv[1][0] == '-' && !argv[1][1])
            chdir(getenv("OLDPWD"));
        else if (chdir(argv[1]))
        {
            warn("cd: %s: ", argv[1]);
            return 1;
        }
    }
    return 0;
}

static int killer(int argc, char** argv)
{
    char iserr = 0;
    int sig = 9;
    pid_t pid;

    if (argc < 2)
    {
        warnx("kill: usage: kill [-s sigspec | -n signum | -sigspec] pid | "
              "jobspec ... or kill -l [sigspec]");
        return 2;
    }
    else if (argc == 2)
    {
        if (kill(pid = strtol(argv[1], NULL, 10), 9))
        {
            warn("kill: (%s) - No such process", argv[1]);
            return 1;
        }
    }
    else
    {
        sig = strtol(argv[1], NULL, 10);
        for (int i = 2; i < argc; ++i)
            if (kill(pid = strtol(argv[i], NULL, 10), sig))
            {
                warn("kill: (%s) - No such process", argv[i]);
                iserr = 1;
            }
    }
    return iserr;
}

static int myexit(int argc, char** argv)
{
    char isnb = 1;

    if (argc > 2)
    {
        warnx("exit: too many arguments");
        setenv("?", "1", 1);
        return 0;
    }
    if (argc == 2)
    {
        for (size_t i = 0; i < strlen(argv[1]); ++i)
            if (!isdigit(argv[1][i]))
            {
                isnb = 0;
                break;
            }
        if (isnb)
            setenv("?", argv[1], 1);
        else
        {
            warnx("exit: %s: numeric argument required", argv[1]);
            setenv("?", "2", 1);
        }
    }
    return 1;
}
