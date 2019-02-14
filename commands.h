



#ifndef _COMMANDS_H_
#define _COMMANDS_H_
void error(void);
int wish_exit(char* args[], int numtokens);
int wish_cd(char* args[], int numtokens);
int history_noarg(void);
int history_hasarg(char* tokens[]);
int set_path(char*args[], int numtokens);
#endif


