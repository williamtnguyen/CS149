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
void InsertNodeBefore(ListNode* thisNode, ListNode* newNode);
ListNode* GetNextNode(ListNode* thisNode);
void FreeNodes(ListNode* headNode);

#endif
