



#ifndef _COMMANDS_H_
#define _COMMANDS_H_
int wish_exit(char* args[], int numtokens);
int wish_cd(char* args[], int numtokens);
int history_noarg(void);
int history_hasarg(char* lines[], int numlines);
int set_path(char*args[], int numtokens);
#endif


