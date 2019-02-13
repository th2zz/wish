/**
 *	This file provides methods to manipulate strings.
 *
 * */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "str.h"
/**
 *	split the string according to delim to an array without modifiying original string
 */
char** split_str(const char* str, const char* delim, size_t* num_tokens) {
    //make a copy of str so that str is not modified
    char *s = strdup(str);
    //dynammicall allocate memory for the double array tokens that store result
    size_t tokens_allocated = 1;
    size_t tokens_used = 0;
    char **tokens = calloc(tokens_allocated, sizeof(char*));
    char *token, *strtok_ctx;
    for (token = strtok_r(s, delim, &strtok_ctx);token != NULL;token = strtok_r(NULL, delim, &strtok_ctx)) {
        if (tokens_used == tokens_allocated) {
            tokens_allocated *= 2;
            tokens = realloc(tokens, tokens_allocated * sizeof(char*));
        }
	//tokenize words and store them in array
        tokens[tokens_used] = strdup(token);
	tokens_used++;
    }
    if (tokens_used == 0) {
        free(tokens);
        tokens = NULL;
    } else {
        tokens = realloc(tokens, tokens_used * sizeof(char*));
    }
    *num_tokens = tokens_used;
    free(s);
    return tokens;
}

