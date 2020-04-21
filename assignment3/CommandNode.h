#ifndef COMMANDNODE_H
#define COMMANDNODE_H
#import <stdbool.h>

/**
 * Struct for a Command Node and function prottypes for a linked-list of commands
 */ 

typedef struct command_struct {
	char command[20][20];
	int index;
	int PID;
	int start_time;
	bool active;
	struct command_struct* nextNodePtr;
} CommandNode;

void CreateCommandNode(CommandNode* thisNode, char cmd[20][20], int numCommands, int index, CommandNode* nextNode);
void InsertCommandAfter(CommandNode* thisNode, CommandNode* newNode);
CommandNode* GetNextCommand(CommandNode* thisNode);
CommandNode* FindCommand(CommandNode* findNode, int pid);

#endif
