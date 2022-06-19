#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lib.h"
#include <unistd.h>

/** Name and function pointer for a built-in command.
 */
struct BuiltIn {
    char const *const name;
    int(*fn)(char const**);
};

/** Handler for `exit` built-in.
 */
static int fn_exit(char const **args)
{
    return 0;
}

/** Handler for `cd` built-in.
 */
static int fn_cd(char const **args)
{
    if (args[1] == NULL) {
        fprintf(stderr, "missing argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("failed to cd");
        }
    }

    return 1;
}

/** Mapping of built-in names to functions, which must be terminated with
 * a sentinel { NULL, NULL }.
 */
static struct BuiltIn builtIns[] = {
    { "exit", fn_exit },
    { "cd", fn_cd },
    { NULL, NULL }
};

/** Parses the arguments, and executes either a built-in (if the first argument
 * matches the name of a built-in), otherwise treats the first argument as the
 * name of an external program and launches a new process for it.
 */
static int execute(char const **args)
{
    struct BuiltIn const *builtIn;

    if (args[0] == NULL) {
        return 1;
    }
    
    for (builtIn = builtIns; builtIn->name; ++builtIn) {
        if (strcmp(args[0], builtIn->name) == 0) {
            return (*builtIn->fn)(args);
        }
    }

    launchprogram(args);
    return 1;
}

/** Read, evaluate, print, loop
 */
static void repl()
{
    int status = 1;

    while (status) {
        char *line;
        char const **args;

        printf("\x1b[35m$ \x1b[1m");
        line = readline();
        printf("\x1b[0m");
        fflush(stdout);

        args = tokenize(line);
        status = execute(args);

        free(args);
        free(line);
    }
}

int main(int argc, char *argv[])
{
    repl();
    return EXIT_SUCCESS;
}
