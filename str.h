#ifndef _STR_H_
#define _STR_H_

char** split_str(const char* str, const char* delim, size_t* num_tokens);
int is_digit(char c);
int valid(const char* str);
int is_number(const char* str, int* result);

#endif
