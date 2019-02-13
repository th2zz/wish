/*this is a file on built-in commands*/
//#include "commands.h"
#include <stdlib.h>
#include "wish.h"
#include "str.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
int wish_exit(char*args[], int numtokens){
	if (numtokens != 1) {//passing any arguments to exit return immediately
		char error_message[30] = "An error has occurred\n";
    		write(STDERR_FILENO, error_message, strlen(error_message));	
		return 0;
	}
	exit(0);
}
int wish_cd(char*args[], int numtokens) {
	int numargs = numtokens - 1;
	if (numargs == 0 || numargs > 1) {
                char error_message[30] = "An error has occurred\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
                return 0;
	}	
	//change directory
	char* dest = args[1];	
	//printf("%s\n", dest);
	int rc = chdir(dest);
	if (rc != 0) {//fail to go to that path
                char error_message[30] = "An error has occurred\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
                return 0;
	}		
	return 0;
}

// the shell should print a list of all the lines of input which the user has entered since the shell was started
int history_noarg() {

	return 0;
}

//the shell should print the previous n lines of input
int history_hasarg(char*lines[], int numlines) {

	return 0;
}
//set path to be the specified path according to the command contents in args, return number of paths 
int set_path(char*args[], int numtokens) {
	//int* ptr = &num_path;
	if(numtokens == 1) {
		//*ptr = numtokens;
		flag = -1;//only can run built-in commands
		return 0;//no path is set	num path is default
	}
	if (flag == 1 && PATH != NULL) free(PATH);//path has been previously overwritten
	flag = 1;//normal state can run all program && path has been overwritten
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
