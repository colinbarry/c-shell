#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/errno.h>
#include <unistd.h>

/** Returns a pointer to the first matching unescaped quote, or to the
 * end sentinel if no match is found.
 */
static char *matchquote(char *s)
{
    int escaped = 0;

    for (; *s; ++s) {
        char const c = *s;
        if (c == '\"' && !escaped) {
            return s;
        }
        escaped = c == '\\';
    }

    return s;
}

/**
* Re-entrant strtok implementation that treats text between quotes as a
* single token.
*
* @param s          string to be tokenized, or NULL or preceeding calls to the
*                   same string
* @param delim      characters to be treated as delimiters
* @param context    internal; used to maintain context between successive calls
* @return           zero-terminated token
*/
static char *qstrtok(char *s, char const *delim, char **context)
{
    char *token = NULL;

    if (s == NULL) {
        s = *context;
        if (s == NULL) {
            return NULL;
        }
    }

    s += strspn(s, delim);
    if (!*s) {
        return NULL;
    } else if (*s == '\"') {
        ++s;
        token = s;
        s = matchquote(s);
        if (!*s) {
            return NULL;
        } 
    } else {
        token = s;
        s = strpbrk(s, delim);
        if (!s) {
            *context = NULL;
            return token;
        }
    }

    *s = '\0';
    *context = s + 1;
    return token;
}

void *ckalloc(size_t const n)
{
    void *p = malloc(n);
    if (!p) {
        fprintf(stderr, "could not allocate memory");
        exit(EXIT_FAILURE);
    }

    return p;
}

void *ckrealloc(void *p, size_t const n)
{
    p = realloc(p, n);
    if (!p) {
        fprintf(stderr, "could not allocated memory");
        exit(EXIT_FAILURE);
    }

    return p;
}

char const **tokenize(char *line)
{
    char const **tokens;
    size_t size, capacity;
    char const *token;
    char *context;

    size = 0;
    capacity = 4;
    tokens = ckalloc(sizeof(*tokens)  *capacity);

    token = qstrtok(line, " ", &context);
    while (token) {
        if (size == capacity) {
            capacity *= 2;
            tokens = ckrealloc(tokens, sizeof(*tokens)  *capacity);
        }
        tokens[size++] = token;
        token = qstrtok(NULL, " ", &context);
    }

    if (size == capacity) {
        ++capacity;
        tokens = ckrealloc(tokens,  sizeof(*tokens)  *capacity);
    }

    tokens[size] = NULL;

    return tokens;
}

char *readline()
{
    size_t size, used;
    char *buf;

    size = 128;
    buf = ckalloc(size);
    used = 0;

    while (1) {
        int c = getc(stdin);
        if (used == size) {
            size *= 2;
            buf = ckrealloc(buf, size);
        }

        if (c == EOF || c == '\n') {
            buf[used] = 0;
            return buf;
        } else {
            buf[used++] = (char)c;
        }
    }

    return buf;
}

void launchprogram(char const **args)
{
    pid_t pid;

    pid = fork();
    if (pid == 0) {
        if (execvp(args[0], (char**)args) == -1) {
            perror("failed to exec");
            exit(EXIT_FAILURE);
        }
    } else if (pid < 0) {
        perror("failed to fork process");
        exit(EXIT_FAILURE);
    } else {
        while (1) {
            int status;
            waitpid(pid, &status, WUNTRACED);
            if (WIFEXITED(status) || WIFSIGNALED(status))
                return;
        };
    }
}

