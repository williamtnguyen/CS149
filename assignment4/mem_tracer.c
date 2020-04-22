#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Include linked-list and stack
#include "CommandNode.h"
#include "TraceNode.h"

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
