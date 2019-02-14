#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
int is_digit(char c){
    switch(c) {
        case '0':
            return 1;
            break;
        case '1':
            return 1;
            break;
        case '2':
            return 1;
            break;
        case '3':
            return 1;
            break;
        case '4':
            return 1;
            break;
        case '5':
            return 1;
            break;
        case '6':
            return 1;
            break;
        case '7':
            return 1;
            break;
        case '8':
            return 1;
            break;
        case '9':
            return 1;
            break;
    }
    return 0;
}
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
int valid(const char* str){
    size_t numparts = 0;
    split_str(str, "-", &numparts);
    if (numparts > 2) return 0;//have multiple - or .  error
    numparts = 0;
    split_str(str, ".", &numparts);
    if (numparts > 2) return 0;//have multiple - or .  error
    //now only have 1 . or -
    if(str[0] == '.') return 0;
    int i = 0;
    while (str[i]!='\0') {
        if (str[i]!='.' && str[i]!='-'&&is_digit(str[i]) != 1) return 0;//found 1 char not digit  error
        i++;
    }
    return 1;//valid
}
int is_number(const char* str, int* result) {
        
        //return value: -1  do nothing  0 good  1 error
        if (valid(str) != 1) return 1;//string not valid  
	float val  = atof(str);
        if (val <= 0) {
           return -1;
        }//only positive now
        
        //get ceiling of the number and return it
        double temp = ceil((double) val);
	*result = (int)temp;
	return 0;
}
//these are for test purposes
//int main(void) 
//{
    
    //char* str0 = "-1.12313";
    //char* str1 = "-1";
    //char* str2 = ".-1";
    //char* str3 = "1.12313";
    //char* str4 = "1";
    //char* str5 = ".1";
    //char* str6 = "f-1";
    //char* str7 = "f.1";
    //char* str8 = "-1f1";
    //char* str9 = "1f1";
    //char* str10 = "-1.f";
    //char* str11 = "1.f";
    //char* str12 = "-1.1.-f";
    //char* str13 = "1.1.-f";
    //char* str14 = "abcjdsaoifj@!^#T&$**";
    //int val = INT_MAX;
    //printf("%s: is number: %d\n",str0, is_number(str0,&val));
    //printf("%s: is number: %d\n",str1, is_number(str1,&val));
    //printf("%s: is number: %d\n",str2, is_number(str2,&val));
    //printf("%s: is number: %d\n",str3, is_number(str3,&val));
    //printf("val: %d\n", val);
    //printf("%s: is number: %d\n",str4, is_number(str4,&val));
    //printf("val: %d\n", val);
    //printf("%s: is number: %d\n",str5, is_number(str5,&val));
    //printf("%s: is number: %d\n",str6, is_number(str6,&val));
    //printf("%s: is number: %d\n",str7, is_number(str7,&val));
    //printf("%s: is number: %d\n",str8, is_number(str8,&val));
    //printf("%s: is number: %d\n",str9, is_number(str9,&val));
    //printf("%s: is number: %d\n",str10, is_number(str10,&val));
    //printf("%s: is number: %d\n",str11, is_number(str11,&val));
    //printf("%s: is number: %d\n",str12, is_number(str12,&val));
    //printf("%s: is number: %d\n",str13, is_number(str13,&val));
    //printf("%s: is number: %d\n",str14, is_number(str14,&val));
    //return 0;
//}

