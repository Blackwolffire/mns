#include "builtin.h"

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
        exit(0);
    }
    if (!strcmp("kill", argv[0]))
    {
        kill(argc, argv);
        return 1;
    }

    return 0;
}
