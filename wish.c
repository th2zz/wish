#include <stdio.h>
#include <string.h>
#include "commands.h"
#include <stdlib.h>
#include <unistd.h>
#include "str.h"
#include "wish.h"
#include <sys/types.h>
#include <sys/wait.h>
char* PATH = "/bin";
int log_counter = 0;//counter for history recorded
char** wish_log;
int flag = 0;//default    -1 only execute builtin commands  1 normal state (path has been set)
void error() {
                char error_message[30] = "An error has occurred\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
}
int run_command(char* actual_path, char*args[]){
	int fork_rc = fork();
	if (fork_rc == -1) {//fail to fork
		error();	
	} else if (fork_rc == 0) {//success
		int execv_rc = execv(actual_path, args);
		printf("this line should not be printed. execv_rc: %d\n",execv_rc);
	} else {
		int wait_rc = wait(NULL);
		if (wait_rc == -1) {
			error();
			//wait failure
		}//parent process wait till child process return
	}
	
	return 0;
}
void build_path_and_execute(char** tokens, int numtokens){
                        char **path_arr;
                        size_t len = 0;
                        path_arr = split_str(PATH, " \t\n", &len);
                        char* actual_path = NULL;
                        int counter = -1;
                        for (int i = 0; i < len; i++) {
                                //build the actual path for each path read from path_arr
                                int size = strlen(tokens[0]) + strlen(path_arr[i]) + 2;
                                actual_path = malloc(size);
                                strcpy(actual_path, path_arr[i]);
                                //***concatenate the path with "/" if the last char is not "/"
                                char* temp = path_arr[i];
                                temp += strlen(temp)-1;
                                if (strcmp(temp, "/") != 0)     strcat(actual_path, "/");
                                strcat(actual_path, tokens[0]);//concat the binary file name
                                int access_rc = access(actual_path, X_OK);
                                if (access_rc != 0) {//file does not exist at the path
                                        counter++;
                                        if(counter == len - 1) {
                                                error();
						//TODO printf("@\n");
                                                if(actual_path != NULL) free(actual_path);
                                                return;
                                        }
                                        if(actual_path != NULL) free(actual_path);
                                } else {//exist, run the command with arguments list
                                        char* args[numtokens + 1];
                                        for (int i = 0; i < numtokens; i++) {
                                                args[i] = tokens[i];
                                        }
                                        args[numtokens] = NULL;
                                        run_command(actual_path, args);
                                        if(actual_path != NULL) free(actual_path);
                                        break;//return back to main loop
                                }
                        }//note: only support executing binaires in search path. Absolute/relative path execution not supported

}
void process_line (char** tokens, int numtokens) {
	if (strcmp(tokens[0], "exit") == 0) {
		wish_exit(tokens, numtokens);
	} else if (strcmp(tokens[0], "cd") == 0) {
		wish_cd(tokens, numtokens);		
	} else if (strcmp(tokens[0], "path") == 0) {
		set_path(tokens, numtokens);
	} else if (strcmp(tokens[0], "history") == 0) {
		int numargs = numtokens - 1;
		if(numargs == 0) {
			//print all the history
			for(int i = 0; i < log_counter;i++) {
				printf("%s",wish_log[i]);	
			}
		} else if (numargs == 1) {
			//TODO check whether second argument is int
			//if so print previous n lines of inputs otherwise error
		} else {//more than 1 args
			error();
		}
	
	} else {
		//default
		if(flag == 0) {//path is inital path
			build_path_and_execute(tokens, numtokens);
		} else if(flag == 1) {//path has been manually set
			build_path_and_execute(tokens, numtokens);
		} else {//path has been manually set to nothing - flag == -1 only be able to run builtin program		
			error();
		}	
	}

}
/*
 * 	clean up memory for a string array
 */
void cleanup(char*tokens[], int* numtokens) {
        for(int i = 0; i < *numtokens; i++){
                free(tokens[i]);
        }
        if (tokens != NULL) free(tokens);
        *numtokens = 0;
}
/**
 *	keep a record of current line to global log
 */
void keep_record(char* line){
        wish_log[log_counter] = line;
        //printf("%s\n%d\n", wish_log[*counter], *counter);
        log_counter += 1;

}
int read_command(FILE* fp){
	char *line = NULL;
        size_t len = 0;
	fflush(fp);
        ssize_t read;
	if((read = getline(&line, &len, fp)) == -1 || line[0] == EOF) {
		return 1;
	}
	fflush(stdin);
        char **tokens;
        size_t numtokens = 0;
        tokens = split_str(line, " \t\n", &numtokens);
        if(numtokens == 0 || (strcmp(line, "\n") == 0) ||(strcmp(line, "") == 0)) {
        	return -1;
	}
	keep_record(line);	
        process_line(tokens, numtokens);
	cleanup(tokens, (int*)&numtokens);
	return 0;
}
int main(int argc, char* argv[]) {
	//char *line = NULL;
    	//size_t len = 0;
    	//ssize_t read;
	//interactive mode
	if (argc == 1) {
		//at most store 1024 logs
                wish_log = malloc(2048*sizeof(char*));
		while(1){
			
                	printf("wish> ");
                	fflush(stdout);	
			int rc = read_command(stdin);
			fflush(stdin);
			if (rc == -1) continue;	
			if (rc == 1) break;
		}
		cleanup(wish_log, (int*)&log_counter);
		exit(0);
	} 

    



    


	//batch mode no prompt & read from the input file of commands 
	int num_files = argc - 1;
	if (num_files > 1) exit(1); // invoked with more than one file, or if the shell is passed a bad batch file
	char* filename = argv[1];
	FILE *fp = fopen(filename, "r");	
	if (fp == NULL) {//upon failure -  bad batch file
                        error();
			exit(1); // invoked with more than one file, or if the shell is passed a bad batch file
        }
	char *line = NULL;
        size_t len = 0;
        ssize_t read;
	while ((read = getline(&line, &len, fp)) != -1) {
        	//TODO
		printf("%s",line);
        }
	
	if (line != NULL) free(line);
	if (flag == 1 && PATH != NULL) free(PATH); //flag == 1 path has been set!!
	//hit eof 
	exit(0);
}
