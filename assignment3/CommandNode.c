#include <stdio.h>
#include <string.h>
#include "CommandNode.h"

/**
 * Related Function implementations
 */

// 'Constructor': create a new command node
void CreateCommandNode(CommandNode* thisNode, char cmd[20][20], int numCommands, int index, CommandNode* nextNode) {
	// Passing numCommands as an parameter to effectively populate cmd array
	for(int i = 0; i < numCommands; i++) {
		strcpy(thisNode->command[i], cmd[i]);
	}
	thisNode->index = index;
	thisNode->nextNodePtr = nextNode;
	//return;
}

// Insert 'newNode' after 'thisNode'
void InsertCommandAfter(CommandNode* thisNode, CommandNode* newNode) {
	CommandNode* tmpNext = NULL;

	tmpNext = thisNode->nextNodePtr;
	thisNode->nextNodePtr = newNode;
	newNode->nextNodePtr = tmpNext;
	//return;
}

// Get next node relative to 'thisNode'
CommandNode* GetNextCommand(CommandNode* thisNode) {
	return thisNode->nextNodePtr;
}

// Find a node in da list based on the PID
CommandNode* FindCommand(CommandNode* findNode, int pid) {
	CommandNode* curr = findNode;
	while(curr != NULL) {
		if(curr->PID == pid) { return curr; }
		curr = curr->nextNodePtr;
	}
	return NULL; // never found. might want to throw an error instead.
}
