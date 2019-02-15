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
#define MAX_LOGSIZE 2048
char* PATH = "/bin";
int log_counter = 0;//counter for history recorded
char** wish_log;
int path_flag = 0;//default    -1 only execute builtin commands  1 normal state (path has been set)
int redir_flag = 0;
int pip_flag = 0;
int skip_flag = 0;
/**
 *
 *	run normal command
 */
int run_command(char* actual_path, char*args[]){
	int fork_rc = fork();
	if (fork_rc == -1) {//fail to fork
		error();	
	} else if (fork_rc == 0) {//success
		execv(actual_path, args);
		//printf("this line should not be printed. execv_rc: %d\n",execv_rc);
		error();//TODO exit or error?
	} else {
		int wait_rc = waitpid(fork_rc,NULL,0);
		if (wait_rc == -1) {
			error();
			//wait failure
		}//parent process wait till child process return
	}
	
	return 0;
}
/**
 *
 *	run piping command
 */
int run_command_pip(char* actual_path1, char*args1[],char* actual_path2, char*args2[]){
        int savedstdin = dup(0);
	int savedstdout = dup(1);
	int savedstderr = dup(2);
	pid_t pid1, pid2;
	int pipefd[2];
	pipe(pipefd);
	pid1 = fork();
        if (pid1 == -1) {//fail to fork
                error();
        } else if (pid1 == 0) {//success
                dup2(pipefd[1],STDOUT_FILENO);
		dup2(pipefd[1],STDERR_FILENO);
		close(pipefd[0]);
		execv(actual_path1, args1);
                error();
        	exit(1);//fail to execute
	}  
	pid2 = fork();
	if(pid2 == 0) {
		dup2(pipefd[0], STDIN_FILENO);
		close(pipefd[1]);
		execv(actual_path2, args2);
		error();
                exit(1);
	}
   	close(pipefd[0]);
   	close(pipefd[1]);	
        int wait_rc = waitpid(pid1,NULL,0);
        if (wait_rc == -1) {
                        error();
                      
        }
       	int wait_rc2 = waitpid(pid2, NULL,0);
	if (wait_rc2 == -1) {
                        error();
        }
	//restore file descriptors
	dup2(savedstdin, 0);
	dup2(savedstdout,1);
	dup2(savedstderr,2);
	close(savedstdin);
	close(savedstdout);
	close(savedstderr);
        return 0;
}
/**
 *	build actual path of the excutable from user specified/default search path
 *	check whether the file is there, and run corresponding commands
 *
 */
void build_path_and_execute(char** tokens, int numtokens, char**tokens2, int numtokens2){
                        char **path_arr;
                        size_t len = 0;
                        path_arr = split_str(PATH, SPACE, &len);
			char* actual_path = NULL;
                        char* actual_path2 = NULL;
			char* args1[numtokens + 1];
			char* args2[numtokens2 + 1];
			int found = 0;
			int found2 = 0;
			if (pip_flag == 1) {
			        for (int i = 0; i < len; i++) {
                                        //build the actual path for each path read from path_arr
                                        int size = strlen(tokens[0]) + strlen(path_arr[i]) + 2;
                                        actual_path = malloc(size);
                                        strcpy(actual_path, path_arr[i]);
					//***concatenate the path with "/" if the last char is not "/"
                                        char* temp = path_arr[i];
                                        temp += strlen(temp)-1;
                                        if (strcmp(temp, "/") != 0)     {
						strcat(actual_path, "/");
					}
					strcat(actual_path, tokens[0]);//concat the binary file name
                                        int access_rc = access(actual_path, X_OK);
					if (access_rc != 0) {//file does not exist at the path
                                        	if(actual_path != NULL) free(actual_path);
                                        } else {//exist
                                                for (int i = 0; i < numtokens; i++) {
                                                           args1[i] = tokens[i];
                                                }
                                                args1[numtokens] = NULL;
						found = 1;
                                                break;//return back to main loop
                                        }
                                }
				for (int i = 0; i < len; i++) {
					int size2 = strlen(tokens2[0]) + strlen(path_arr[i]) + 2;
                                        actual_path2 = malloc(size2);
                                        strcpy(actual_path2, path_arr[i]);	
					char* temp = path_arr[i];
                                        temp += strlen(temp)-1;
                                        if (strcmp(temp, "/") != 0)     {
                                                strcat(actual_path2, "/");
                                        }
					strcat(actual_path2, tokens2[0]);
					int access_rc2 = access(actual_path2, X_OK);
					if (access_rc2 != 0) {
                                                if(actual_path2 != NULL) free(actual_path2);
                                        }
					if (access_rc2 == 0){//exist, run the command with arguments list
                                                for (int i = 0; i < numtokens2; i++) {
                                                           args2[i] = tokens2[i];
                                                }
                                                args2[numtokens2] = NULL;
                                                found2 = 1;
                                                break;//return back to main loop
                                        }
				}
				if(found == 0 && found2 == 0) {
                                                        error();
							error();
                                } else if(found == 1 && found2 == 0) {
						error();
						if(actual_path != NULL) free(actual_path);
				} else if(found == 0 && found2 == 1) {
						error();
						run_command(actual_path2, args2);
                                                if(actual_path2 != NULL) free(actual_path2);
				} else {
					run_command_pip(actual_path,args1,actual_path2, args2);
				}
				return;
			} 

			int counter = -1;
		        if(pip_flag != 1){
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
}
/**
 *
 * 	run the tokenized command
 * 	tokens content are stored in array tokens,  size of array is stored in numtokens
 * 	tokens2 & numtokens2 is additional information for command2 for piping
 *
 */
void process_line (char** tokens, int numtokens, char** tokens2, int numtokens2) {
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
		//not built in commands
		if(path_flag == 0 || path_flag == 1) {//path is inital path  or path has been manually set
			build_path_and_execute(tokens, numtokens, tokens2, numtokens2);
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
/**
 *	check whether the line need redirection/piping
 *
 */
char** redirpip_check(char* line){
	redir_flag = 0;//reset
	pip_flag = 0;
	skip_flag = 0;
	int i = 0;
	int redir_count = 0;
	int pip_count = 0;
	while (line[i]!= '\0') {
		//printf("%c", line[i]);
		if(line[i] == '>') redir_count++;
		if(line[i] == '|') pip_count++;
		i++;
	}
	//only support 1 '>' or '|' to appear once at a time for one line
	char **tokens;
        size_t numtokens = 0;
	if(redir_count >= 2 || pip_count >= 2 || redir_count + pip_count >= 2) {
		error();
		skip_flag = 1;
		return NULL;
	} else if (redir_count == 0 && pip_count == 0) {
		//should not skip current line execution
		return NULL;
	} else if (redir_count == 1 && pip_count == 0) {
        	tokens = split_str(line, ">", &numtokens);
		if(numtokens == 0) {// ">"
			error();
                        skip_flag = 1;//parse error empty cmd1
                        return NULL; //no redirection
		}
		if(numtokens == 1){
			error();
                        skip_flag = 1;
                        return NULL; // " >"   "> "
		}
		
		size_t tokens_cmd1 = 0;
		split_str(tokens[0], SPACE, &tokens_cmd1);
		if (tokens_cmd1 == 0) {
			error();
			skip_flag = 1; //"  >xx"
                        return NULL; 
		}
		size_t tokens_cmd2 = 0;
                split_str(tokens[1], SPACE, &tokens_cmd2);
                if (tokens_cmd2 == 0) {
                        error();
                        skip_flag = 1; //"xx>   "
                        return NULL; 
                }
		char* fileslist = tokens[1];
        	size_t numfiles = 0;
        	char** temp = split_str(fileslist, SPACE, &numfiles);
        	if (numfiles > 1) {
                	error();
			skip_flag = 1;
               		return NULL; //no redirection
        	}
        	tokens[1] = temp[0]; //may have a problem here TODO
        	redir_flag = 1;
		//check if the line should be redirected and update global flag  return parsed array of size 2
		//tokens:  tokens[0] commands  tokens[1]file name list	
	} else if (redir_count == 0 && pip_count == 1) {
		//Do pip    preprocess commands	
		tokens = split_str(line, "|", &numtokens);
		if(numtokens == 0) {// "|"
                        error();
                        skip_flag = 1;//parse error empty cmd1
                        return NULL; //no redirection
                }
                if(numtokens == 1){
                        error();
                        skip_flag = 1;
                        return NULL; // " |"   "| "
                }

                size_t tokens_cmd1 = 0;
                split_str(tokens[0], SPACE, &tokens_cmd1);
                if (tokens_cmd1 == 0) {
                        error();
                        skip_flag = 1; //"  |xx"
                        return NULL;
                }
                size_t tokens_cmd2 = 0;
                split_str(tokens[1], SPACE, &tokens_cmd2);
                if (tokens_cmd2 == 0) {
                        error();
                        skip_flag = 1; //"xx|   "
                        return NULL;
                }
		char* command2 = tokens[1];
                size_t numtokens_command2 = 0;
                split_str(command2, SPACE, &numtokens_command2);
                if (numtokens_command2 == 0) {
                        error();
			skip_flag = 1;
                        return NULL; //no following command
                }
                pip_flag = 1;
		//numtokens is 2   tokens[0] command1 tokens[1] command2
	}
	return tokens; 
}
/**
 *	Read command line by line: check whether redirection and piping is needed
 */
int read_command(FILE* fp){
	char *line = NULL;
        size_t len = 0;
        ssize_t read;
	fflush(fp);
	if((read = getline(&line, &len, fp)) == -1 || line[0] == EOF) {
		return 1;
	}
	fflush(stdin);
	//normal command or empty command
	char **tokens;
        size_t numtokens = 0;
	tokens = split_str(line, SPACE, &numtokens);
	if(numtokens == 0 || (strcmp(line, "\n") == 0) ||(strcmp(line, "") == 0)) {
        	return -1;//empty commands, skip
	}
	keep_record(line);
	//check whether redir/pip is needed
	char**check_result = redirpip_check(line);
	if (skip_flag == 1) {
		return -1;
	} else if(skip_flag != 1 && pip_flag == 0 && redir_flag == 1 && check_result != NULL) {
                int saved_stdout = dup(1);
                int saved_stderr = dup(2);
		//S_IRWXU   READ WRITE EXECUTE BY OWNER
		int file_fd = open(check_result[1], O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR);
        	if (file_fd >= 0){
                	dup2(file_fd, STDOUT_FILENO);
        		dup2(file_fd, STDERR_FILENO);
			fflush(stdout);
        	}
		char* temp = check_result[0];
		cleanup(tokens, (int*)&numtokens);
		numtokens = 0;
		tokens = split_str(temp, SPACE, &numtokens);
		process_line(tokens, numtokens, NULL, -1);
		//done   clean up
		int* tmp = malloc(sizeof(int));
                *tmp = 2;
                if (check_result != NULL) cleanup(check_result, tmp);
                free(tmp);
		dup2(saved_stdout, 1);
                dup2(saved_stderr, 2);
                close(saved_stdout);
                close(saved_stderr);	
		close(file_fd);
	} else if(skip_flag != 1 && pip_flag == 1 && redir_flag == 0 && check_result != NULL) {
		//prepare the pipe update token to new thing
		tokens = check_result;
		numtokens = 2;
		pip_flag = 1;
		char* line_cmd1 = tokens[0];
		char** tokens_cmd1;
		size_t num_tokens_cmd1 = 0;
		tokens_cmd1 = split_str(line_cmd1, SPACE, &num_tokens_cmd1);//tokenize cmd1
		char* line_cmd2 = tokens[1];
		char** tokens_cmd2;
                size_t num_tokens_cmd2 = 0;
		tokens_cmd2 = split_str(line_cmd2, SPACE, &num_tokens_cmd2);//tokenize cmd2
		process_line(tokens_cmd1, num_tokens_cmd1,tokens_cmd2, num_tokens_cmd2);//process the piping cmd
		pip_flag = 0;
	} else if (skip_flag != 1 && pip_flag == 0 && redir_flag == 0 && check_result == NULL) {
		process_line(tokens, numtokens,  NULL, -1);	
	}
	//clean up
	if(tokens!=NULL) cleanup(tokens, (int*)&numtokens);
	return 0;
}
/**
 *	main: support interactive mode and batch mode
 */
int main(int argc, char* argv[]) {
	//interactive mode
	if (argc == 1) {
                wish_log = malloc(MAX_LOGSIZE * sizeof(char*));
		while(1){
                	printf("wish> ");
                	fflush(stdout);	
			int rc = read_command(stdin);
			fflush(stdin);
			if (rc == -1) continue;	//invalid commands
			if (rc == 1) break;// reach EOF
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
	wish_log = malloc(MAX_LOGSIZE * sizeof(char*));
	while(1){
		fflush(stdout); 
                int rc = read_command(fp);
                fflush(stdin);
                if (rc == -1) continue; //invalid commands
                if (rc == 1) break;// reach EOF	
	}	
	cleanup(wish_log, (int*)&log_counter);
	if (path_flag == 1 && PATH != NULL) free(PATH); //flag == 1 path has been set!!
	//hit eof 
	exit(0);
}
