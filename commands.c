/*this is a file on built-in commands*/
//#include "commands.h"
#include <stdlib.h>
#include "wish.h"
#include "str.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
void error() {
                char error_message[30] = "An error has occurred\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
}
int wish_exit(char*args[], int numtokens){
	if (numtokens != 1) {//passing any arguments to exit return immediately
		error();
		return 0;
	}
	exit(0);
}
int wish_cd(char*args[], int numtokens) {
	int numargs = numtokens - 1;
	if (numargs == 0 || numargs > 1) {
                error();
		return 0;
	}	
	//change directory
	char* dest = args[1];	
	int rc = chdir(dest);
	if (rc != 0) {//fail to go to that path
                error();
		return 0;
	}		
	return 0;
}

// the shell should print a list of all the lines of input which the user has entered since the shell was started
int history_noarg() {
	for(int i = 0; i < log_counter;i++) {
		printf("%s",wish_log[i]);
	}

	return 0;
}

/**
 * 			check whether second argument is int
 *			if so print previous n lines of inputs otherwise error
 */
int history_hasarg(char*tokens[]) {
	char* temp = tokens[1];
        int n = 0;
       	int rc = is_number(temp, &n);
        if (rc == -1) {
   	     //do nothing
        } else if (rc == 1) {
      		error();
        } else {//print previous n lines
        	int start_pos = log_counter - n;
                for (int i = start_pos; i < log_counter; i++) {
                       printf("%s",wish_log[i]);
                }
        }
	return 0;
}
//set path to be the specified path according to the command contents in args, return number of paths 
int set_path(char*args[], int numtokens) {
	//int* ptr = &num_path;
	if(numtokens == 1) {
		//*ptr = numtokens;
		path_flag = -1;//only can run built-in commands
		return 0;//no path is set	num path is default
	}
	if (path_flag == 1 && PATH != NULL) free(PATH);//path has been previously overwritten
	path_flag = 1;//normal state can run all program && path has been overwritten
	char* buffer = malloc(strlen(args[1]) + 2);
	strcpy(buffer, args[1]);
	strcat(buffer, " ");
	int size = strlen(args[1]) + 2;
	//int numpaths = 1;
	for (int i = 2; i < numtokens; i++) {
		size += strlen(args[i]) + 1;	
		buffer = realloc(buffer, size);
		strcat(buffer, args[i]);
		strcat(buffer, " ");
		//numpaths++;	
	}
	PATH = buffer;	
	//printf("result!!! :%s\n", PATH);
	return 0;//num paths
}
