/**
* Splits the given line into individual tokens using space as a delimiter, 
* treating text between quotes as a single token.
*
* @param    line    line to be tokenized. This string will be mutated
*                   to insert '\0' terminators between tokes.
* @return           arrays of pointers to tokens in the original line. Caller
*                   must free the array (but *not* the individual tokens, as
*                   they still inside the original line buffer's allocation.)
*/
char const **tokenize(char *line);

/**
* malloc wrapper that exits gracefully if memory cannot be allocated.
*
* @param n  bytes to be allocated
* @return   pointer to freshly allocated memory
*/
void *ckalloc(size_t n);

/**
* realloc wrapper that exits gracefully if memory cannot be allocated.
*
* @param p  buffer that needs reallocating
* @param n  new size of buffer
* @return   pointer to freshly allocated memory
*/
void *ckrealloc(void *p, size_t n);

/**
* Reads and returned a zero-terminated line from stdin. Ending newlines are
* not included in the line.
*
* @return   zero-terminated pointer to string. Caller must free.
*/
char *readline();

/**
* Launches the program specified with args[0], passing the rest of the args
* as arguments to the process. 
*
* @param args   pointer to array of arguments, the first of which is the name
*               of the program, and the rest are treated as arguments. The last
*               argument must be NULL
*/
void launchprogram(char const **args);
