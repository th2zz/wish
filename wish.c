#include <stdio.h>
#include <string.h>
#include "commands.h"
#include <stdlib.h>
#include <unistd.h>
#include "str.h"
#include "wish.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <ctype.h>
#define SPACE " \n\t\a\r"
char* PATH = "/bin";
int log_counter = 0;//counter for history recorded
char** wish_log;
int path_flag = 0;//default    -1 only execute builtin commands  1 normal state (path has been set)
int redir_flag = 0;

int run_command(char* actual_path, char*args[]){
	int fork_rc = fork();
	if (fork_rc == -1) {//fail to fork
		error();	
	} else if (fork_rc == 0) {//success
		execv(actual_path, args);
		//printf("this line should not be printed. execv_rc: %d\n",execv_rc);
		error();
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
                        path_arr = split_str(PATH, SPACE, &len);
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
			history_noarg();
		} else if (numargs == 1) {
			history_hasarg(tokens);
		} else {//more than 1 args
			error();
		}
	
	} else {
		//default
		if(path_flag == 0) {//path is inital path
			build_path_and_execute(tokens, numtokens);
		} else if(path_flag == 1) {//path has been manually set
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
//check if the line should be redirected and update global flag  return parsed array of size 2
//arr[0] commands  arr[1]file name list
char** redirection_check(char* line){
	redir_flag = 0;//reset
	int i = 0;
	int redir_count = 0;
	while (line[i]!= '\0') {
		//printf("%c", line[i]);
		if(line[i] == '>') redir_count++;
		i++;
	}
	if(redir_count > 1) {
		error();
		return NULL;
	} 
	if (redir_count == 0) return NULL;
	char **tokens;
        size_t numtokens = 0;
	tokens = split_str(line, ">", &numtokens);
	char* fileslist = tokens[1];
	size_t numfiles = 0;
	char** temp = split_str(fileslist, SPACE, &numfiles);
	if (numfiles > 1) {
		error();
		return NULL; //no redirection
	}
	tokens[1] = temp[0]; //may have a problem here TODO
	redir_flag = 1;
	return tokens; 
}
int read_command(FILE* fp, int* fd){
	char *line = NULL;
        size_t len = 0;
        ssize_t read;
	fflush(fp);
	if((read = getline(&line, &len, fp)) == -1 || line[0] == EOF) {
		return 1;
	}
	fflush(stdin);
	char **tokens;
        size_t numtokens = 0;
	tokens = split_str(line, SPACE, &numtokens);
	if(numtokens == 0 || (strcmp(line, "\n") == 0) ||(strcmp(line, "") == 0)) {
        	return -1;//empty commands
	}
	keep_record(line);	
        //perform redirection if needed
	char**tokens_redir;
	tokens_redir = redirection_check(line);
	if(redir_flag == 1 && tokens_redir != NULL) {
		//S_IRWXU   READ WRITE EXECUTE BY OWNER
		int fd_out = open(tokens_redir[1], O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR);
        	if (fd_out > 0){
                	dup2(fd_out, STDOUT_FILENO);
        		dup2(fd_out, STDERR_FILENO);
			fflush(stdout);
        	}
		*fd = fd_out;
		char* temp = tokens_redir[0];
		cleanup(tokens, (int*)&numtokens);
		numtokens = 0;
		tokens = split_str(temp, SPACE, &numtokens);
	}
	//free tokens_redir
	process_line(tokens, numtokens);
	if(redir_flag != 1) cleanup(tokens, (int*)&numtokens);
	return 0;
}
int main(int argc, char* argv[]) {
	//interactive mode
	if (argc == 1) {
		//at most store 1024 logs
                wish_log = malloc(2048*sizeof(char*));
		while(1){
			int fd = -1;
                	printf("wish> ");
                	fflush(stdout);	
			int saved_stdout = dup(1);
			int saved_stderr = dup(2);
			int rc = read_command(stdin, &fd);
			fflush(stdin);
			if (rc == -1) continue;	
			if (rc == 1) break;
			if (fd >= 0) {//reset stdout 
				close(fd);
				dup2(saved_stdout, 1);
				dup2(saved_stderr, 2);
				close(saved_stdout);
				close(saved_stderr);
			}
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
	if (path_flag == 1 && PATH != NULL) free(PATH); //flag == 1 path has been set!!
	//hit eof 
	exit(0);
}
