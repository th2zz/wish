#include <stdio.h>
#include <string.h>
#include "commands.h"
#include <stdlib.h>
int main(int argc, char* argv[]) {
	if (argc == 1) {
		printf("wish> ");
		char *line = NULL;
		size_t len = 0;
		ssize_t read;
		//read&process user commands line by line until user input exit
		//getline allows to obtain arbitrarily long input lines
		while ((read = getline(&line, &len, stdin)) != -1) {
			if(strcmp(line, "exit\n") == 0) exit(0);
			printf("wish> ");	
			//TODO	
		}
		exit(0);
	} 

	//batch mode no prompt & read from input files of commands TODO
	int num_files = argc - 1;
	for (int i = 1; i <= num_files; i++) {
		//TODO process each file
		char* curr = argv[i];
		printf("%s\n", curr);	
	}	
	exit(0);
}
