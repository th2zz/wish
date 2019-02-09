#include <stdio.h>
int main(int argc, char* argv[]) {
	if (argc == 1) printf("interactive mode\n"); //TODO
	//batch mode TODO
	int num_files = argc - 1;
	for (int i = 1; i <= num_files; i++) {
		//TODO process each file
		char* curr = argv[i];
		printf("%s\n", curr);	
	}	
	return 0;
}
