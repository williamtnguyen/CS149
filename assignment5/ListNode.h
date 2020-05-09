#ifndef LISTNODE_H
#define LISTNODE_H

/**
 * Struct for a Node and function signatures for a linked-list of user inputs from stdin
 */

typedef struct node_struct {
	char* input;
	struct node_struct* nextNodePtr;
} ListNode;

void CreateListNode(ListNode* thisNode, char* input, ListNode* nextNode);
void InsertNodeAfter(ListNode* thisNode, ListNode* newNode);
ListNode* GetNextNode(ListNode* thisNode);
void PrintNodes(ListNode* headNode);
void FreeNodes(ListNode* headNode);

#endif
