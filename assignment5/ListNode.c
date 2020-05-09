#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ListNode.h"

/**
 * List Node function definitions for linked-list of commands
 */


/* 'Constructor': create a new command node */
void CreateListNode(ListNode* thisNode, char* input, ListNode* nextNode) {
	thisNode->input = input;
	thisNode->nextNodePtr = nextNode;	
}

/* Insert 'newNode' before 'thisNode' */
void InsertNodeBefore(ListNode* thisNode, ListNode* newNode) {
	newNode->nextNodePtr = thisNode;
}

/* Get next node relative to 'thisNode' */
ListNode* GetNextNode(ListNode* thisNode) {
	return thisNode->nextNodePtr;
}

/* Recursive function that prints contents of all nodes */
void PrintNodes(ListNode* headNode) {
	// base case
	if(headNode == NULL) {
		return;
	}

	printf("Index: %d, Command: %s\n", headNode->index, headNode->command);
	PrintNodes(GetNextCommand(headNode));
}

/* Deallocates memory used up by the linked-list */
void FreeNodes(ListNode* headNode) {
	ListNode* currNode = headNode;
	ListNode* nextNode = GetNextNode(headNode);

	while(currNode != NULL) {
		free(currNode);

		if(nextNode != NULL) {
			currNode = nextNode;
			nextNode = GetNextNode(nextNode);
		} else {
			currNode = NULL;
		}
	}
}
