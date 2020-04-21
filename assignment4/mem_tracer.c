#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
	
	// Error checking: invalid # of args
	if(argc <= 1 || argc > 2) {
		printf("Invalid input. Pass in only 1 existing file.");
		exit(EXIT_FAILURE);
	}

	// For parsing through input file
	FILE* fp;
	char* line = NULL;
	size_t len = 0;
	ssize_t read;

	// Allocating array memory to an initial size (will reallocate more memory space if needed)
	char** commandArray;
	commandArray = (char**) malloc(5 * sizeof(char*));
	
	// Opening the input file
	fp = fopen(argv[1], "r");
	if(fp == NULL) {
		perror("mem_tracer");
		exit(1);
	}	

	
}
