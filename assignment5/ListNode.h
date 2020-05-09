#ifndef LISTNODE_H
#define LISTNODE_H

/**
 * Struct for a Node and function signatures for a linked-list of user inputs from stdin
 */

typedef struct node_struct {
	char* input;
	struct node_struct* nextNodePtr;
} ListNode;

void CreateListNode(CommandNode* thisNode, char* input, CommandNode* nextNode);
void InsertNodeAfter(CommandNode* thisNode, CommandNode* newNode);
ListNode* GetNextNode(CommandNode* thisNode);
void PrintNodes(CommandNode* headNode);
void FreeNodes(CommandNode* headNode);

#endif
