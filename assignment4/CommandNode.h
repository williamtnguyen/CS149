#ifndef COMMANDNODE_H
#define COMMANDNODE_H

/**
 * Struct for a Command Node and function signatures for a linked-list of commands
 */

typedef struct command_struct {
	char* command;
	int index;
	struct command_struct* nextNodePtr;
} CommandNode;

void CreateCommandNode(CommandNode* thisNode, char* cmd, int index, CommandNode* nextNode);
void InsertCommandAfter(CommandNode* thisNode, CommandNode* newNode);
CommandNode* GetNextCommand(CommandNode* thisNode);
void PrintNodes(CommandNode* headNode);
void FreeNodes(CommandNode* headNode);

#endif
