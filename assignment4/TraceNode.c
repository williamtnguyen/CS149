#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TraceNode.h"

/**
 * Related Function definitions for a Memory-Tracing Stack implementation
 */


/* Pushes a function p onto the stack */
void PUSH_TRACE(char* p) {
	TraceNode* tnode;
	static char globalID[] = "global";
	
	// Stack is empty, initialize it with "global" identifier
	if(TRACE_TOP == NULL) {
		TRACE_TOP = (TraceNode*) malloc(sizeof(TraceNode));
		
		// Checking for allocation failure
		if(TRACE_TOP == NULL) {
			printf("PUSH_TRACE: memory allocation error\n");
			exit(1);
		}
		TRACE_TOP->functionID = globalID;
		TRACE_TOP->nextNodePtr = NULL;
	}

	// Creating TraceNode for function p
	tnode = (TraceNode*) malloc(sizeof(TraceNode));

	// Checking for allocation failure
	if(tnode == NULL) {
		printf("PUSH_TRACE: memory allocation errpr\n");
		exit(1);
	}
	
	// Stack LIFO order: insert tnode at front of "linked-list"
	tnode->functionID = p;
	tnode->nextNodePtr = TRACE_TOP;
	TRACE_TOP = tnode;
}


/* Pops the top of the stack & deallocates the memory used by prev TRACE_TOP node */
void POP_TRACE() {
	TraceNode* tnode;
	tnode = TRACE_TOP;
	TRACE_TOP = tnode->nextNodePtr;
	free(tnode);
}


/* Prints out a sequence of function calls that are on the stack at the current instance */
char* PRINT_TRACE() {

	// 50 levels is max-threshold of depth for our sequence of nodes in stack
	int maxDepth = 50;
	int i, j, length;
	TraceNode* tnode;
	static char buffer[100];
	
	// Stack is uninitialized, so still in the "global" function scope
	if(TRACE_TOP == NULL) {
		strcpy(buffer, "global");
		return buffer;
	}
	
	// Building the string sequence to be printed	
	sprintf(buffer, "%s", TRACE_TOP->functionID);
	length = strlen(buffer); // initial length

	for(tnode = TRACE_TOP->nextNodePtr, i = 1;
			tnode != NULL && i < maxDepth;
				tnode = tnode->nextNodePtr, ++i) {
		
		j = strlen(tnode->functionID);
	
		// 100 characters is the max-threshold for characters in a sequence we want to print
		if(length + j + 1 < 100) {
			sprintf(buffer + length, ":%s", tnode->functionID);
			length += (j + 1); // +1 accounts for the colon ':'
		} else {
			break; 
		}
	}
	return buffer;
}
