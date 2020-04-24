#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CommandNode.h"

/**
 * Command Node function definitions for linked-list of commands
 */


/* 'Constructor': create a new command node */
void CreateCommandNode(CommandNode* thisNode, char* cmd, int index, CommandNode* nextNode) {
	//strcpy(thisNode->command, &cmd);
	thisNode->command = cmd;
	thisNode->index = index;
	thisNode->nextNodePtr = nextNode;	
}

/* Insert 'newNode' after 'thisNode' */
void InsertCommandAfter(CommandNode* thisNode, CommandNode* newNode) {
	CommandNode* tmpNext = NULL;
	
	tmpNext = thisNode->nextNodePtr;
	thisNode->nextNodePtr = newNode;
	newNode->nextNodePtr = tmpNext;
}

/* Get next node relative to 'thisNode' */
CommandNode* GetNextCommand(CommandNode* thisNode) {
	return thisNode->nextNodePtr;
}

/* Recursive function that prints contents of all nodes */
void PrintNodes(CommandNode* headNode) {
	// base case
	if(headNode == NULL) {
		return;
	}

	printf("Index: %d, Command: %s\n", headNode->index, headNode->command);
	PrintNodes(GetNextCommand(headNode));
}

/* Deallocates memory used up by the linked-list */
void FreeNodes(CommandNode* headNode) {
	CommandNode* currNode = headNode;
	CommandNode* nextNode = GetNextCommand(headNode);

	while(currNode != NULL) {
		free(currNode);

		if(nextNode != NULL) {
			currNode = nextNode;
			nextNode = GetNextCommand(nextNode);
		} else { 
			currNode = NULL; 
		}
	}
}
