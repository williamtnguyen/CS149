#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Include linked-list and stack functions
#include "CommandNode.h"
#include "TraceNode.h"


/* Wrapper function that calls malloc(). Also prints info about memory usage */
void* MALLOC(int typeSize, char* file, int line) {
	void* ptr;
	ptr = malloc(typeSize);
	// Printing info about memory usage
	char* functionSequence = PRINT_TRACE();		
	printf("File %s, line %d, function %s allocated new memory segment at address %d to size %d", file, line, functionSequence, ptr, typeSize);
	return ptr;
}
/* Wrapper function that calls realloc(). Also prints info about memory usage */
void* REALLOC(void* ptr, int typeSize, char* file, int line) {
	ptr = realloc(ptr, typeSize);
	// Printing info about memory usage
	char* functionSequence = PRINT_TRACE();		
	printf("File %s, line %d, function %s reallocated new memory segment at address %d to a new size %d", file, line, functionSequence, ptr, typeSize);
	return ptr;
}
/* Wrapper function that calls free(). Also prints info about memory usage */
void FREE(void* ptr, char* file, int line) {
	// Printing info about memory usage
	char* functionSequence = PRINT_TRACE();
	printf("File %s, line %d, function %s deallocated the memory segment at address %d", file, line, functionSequence, ptr);
	free(ptr);
}


#define malloc(a) MALLOC(a, __FILE__, __LINE__)
#define realloc(a, b) REALLOC(a, b, __FILE__, __LINE__)
#define free(a) FREE(a, __FILE__, __LINE__)


/* Allocates memory for a 2D array of chars (1D array of strings) */
void create_array(char** commandArray, int rows, int columns) {
	PUSH_TRACE("create_array");
	int i;
	commandArray = (char**) malloc(sizeof(char*) * rows);
	for(i = 0; i < rows; i++) {
		commandArray[i] = (char*) malloc(sizeof(char) * columns);
	}
	POP_TRACE();	
}

/* Adds an extra column to a 2D array of chars (1D array of strings) */
void add_column(char** commandArray, int rows, int columns) {
	// pushing function name to the stack
	PUSH_TRACE("add_column");	
	int i;
	for(i = 0; i < rows; i++) {
		commandArray[i] = (char*) realloc(commandArray[i], sizeof(char) * (columns+1));
	}
	POP_TRACE();
}

/**
 * Reads lines from input file and dynamically populates an array and linked-list with those commands, -
 * - while simultaneously reallocating sufficient amount of memory and logging the usage.
 */
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
	int commandIndex = 0;

	// Initialization of linked-list head, currNode, prevNode pointers for traversal/appending
	CommandNode* head = NULL;
	CommandNode* currNode = NULL;	
	CommandNode* prevNode = NULL;
	prevNode = (CommandNode*) malloc(sizeof(CommandNode));
	
	// Opening the input file
	fp = fopen(argv[1], "r");
	if(fp == NULL) {
		perror("mem_tracer");
		exit(1);
	}	

	
	// For every line in input file, populate commandArray and linked-list
	while((read = getline(&line, &len, fp)) != -1) {

		commandArray[commandIndex++] = line;
		
		// Building the linked-list
		currNode = (CommandNode*) malloc(sizeof(CommandNode));
		CreateCommandNode(currNode, line, commandIndex, NULL);
		InsertCommandAfter(prevNode, currNode);
		prevNode = currNode;
		// Keeping reference to the head
		if(commandIndex == 1) { head = currNode; }			
	}
}
