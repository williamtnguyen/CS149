#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <wait.h>

#include "CommandNode.h"


int main(int argc, char* argv[]) {
		
	// Error Checking: Invalid # of args
	if(argc <= 1 || argc > 2) {
		printf("Invalid input. Pass in 1 existing file.");
		exit(EXIT_FAILURE);
	}	

	// For parsing through file
	FILE* fp;
	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	
	// Array of commands/params to be stored in CommandNode and later for exec()
	char newString[20][20];
	int i, j, numCommands;

	// Initialization of LinkedList head, currNode, prevNode pointers for traversal/appending
	CommandNode* head = NULL;
	CommandNode* currNode = NULL;
	CommandNode* prevNode = NULL;
	prevNode = (CommandNode*) malloc(sizeof(CommandNode));
	int nodeIndex = 1;

	// For concurrent tasking
	pid_t pid;	
	struct timespec start, finish;
	double elapsed;

	
	// Opening the file
	fp = fopen(argv[1], "r");
	if(fp == NULL) {
		perror("proc_manager");
		exit(1);
	}


	// For every line, create an array of words that we can pass into execvp
	while((read = getline(&line, &len, fp)) != -1) {

		// getline() reads '\n', so this get rids of it
		if(line[strlen(line) - 1] == '\n') {
			line[strlen(line) - 1] = 0;
		}
	
		// traverse through every char in line and populate an array of commands/parameters
		j = 0, numCommands = 0;
		for(i = 0; i <= strlen(line); i++) {
			// if space or NULL found, assign NULL into newString[numCommand][j]
			if(line[i] == ' ' || line[i] == '\0') {
				newString[numCommands][j] = '\0';
				++numCommands; // next word
				j = 0; // first index of next word
			} else {
				newString[numCommands][j] = line[i];
				j++;	
			}
		}	
		
		
		// Building the linked list
		currNode = (CommandNode*) malloc(sizeof(CommandNode));
		CreateCommandNode(currNode, newString, numCommands, nodeIndex, NULL);
		InsertCommandAfter(prevNode, currNode);
		prevNode = currNode;	
		// Keeping reference to the head
		if(nodeIndex == 1) { head = currNode; }
		++nodeIndex;
	}


	// Concurrency brah
	currNode = head;
	while(currNode != NULL) {
		// Spawn process and recording start time
		pid = fork();		
		clock_gettime(CLOCK_MONOTONIC, &start);

		if(pid < 0) {
			fprintf(stderr, "Fork failed");
			return 1;
		}
		// CHILD process
		else if(pid == 0) {
			// Setup for writing to .out and .err log files
			char outFileName[10];
			char errFileName[10];
			sprintf(outFileName, "%d.out", currNode->index);
			sprintf(errFileName, "%d.err", currNode->index);
			int	out_fd = open(outFileName, O_RDWR | O_CREAT | O_APPEND);
			int err_fd = open(errFileName, O_RDWR | O_CREAT | O_APPEND);

			// Something with permissions
			chmod(outFileName, S_IRWXU);
			chmod(errFileName, S_IRWXU);

			dup2(out_fd, 1);
			dup2(err_fd, 2);

			// Writing to .out file
			fprintf(stdout, "Starting command %d: child PID: %d; parent PID: %d\n", currNode->index, getpid(), getppid());
			fflush(stdout);
		
			// Setting active state of process 
			currNode->active = true;

			// Converting 2D array in node to 1D array of char pointers to pass into execvp()
			char* temp[20];
			int rowNum = 0;
			while(*(currNode->command[rowNum]) != '\0' && rowNum < 20) {				
				temp[rowNum] = currNode->command[rowNum];
				++rowNum;
			}

			temp[rowNum] = NULL;		
			execvp(temp[0], temp);
			
			// Handling execvp() failure			
			printf("execvp() failed for process %d\n", getpid());
			perror(currNode->command[0]);
			return 2;
		}
		// PARENT process: just set the pid for the running process in currNode
		else {
			currNode->start_time = start.tv_sec;
			currNode->PID = pid;
		}

		currNode = GetNextCommand(currNode);
	}	

	
	// Waiting for processes to finish execution
	int status;
	while((pid = wait(&status)) >= 0) {

		// Finding CommandNode for recently terminated process, recording finish/elapsed time
		CommandNode* finishedNode = FindCommand(head, pid);
		clock_gettime(CLOCK_MONOTONIC, &finish);		
		elapsed = (finish.tv_sec - finishedNode->start_time);

		// Setup for APPENDING to .out and .err log files (log files under nodeIndex will have existed already)
		char outFileName[10];
		char errFileName[10];
		sprintf(outFileName, "%d.out", finishedNode->index);
		sprintf(errFileName, "%d.err", finishedNode->index);
		int	out_fd = open(outFileName, O_RDWR | O_CREAT | O_APPEND);
		int err_fd = open(errFileName, O_RDWR | O_CREAT | O_APPEND);
		dup2(out_fd, 1);
		dup2(err_fd, 2);
		
		// Printing exit code or termination signal as well as finish time and elapsed time
		if(WIFEXITED(status)) {
			fprintf(stderr, "Child %d terminated normally with exit code: %d\n", pid, WEXITSTATUS(status));
		}
		else if(WIFSIGNALED(status)) {
			fprintf(stderr, "Child %d terminated abnormally with signal number: %d\n", pid, WTERMSIG(status));
		}
	
		fprintf(stdout, "Finished at %ld seconds, runtime duration: %f seconds\n", finish.tv_sec, elapsed);
		fflush(stdout);

		// Restarting processes if need be
		if(elapsed > 2.0) {
			// Spawn process and recording start time
			pid = fork();
			clock_gettime(CLOCK_MONOTONIC, &start);

			if(pid < 0) {
				fprintf(stderr, "Fork failed\n");
				return 1;
			}
			// Re-start process if exec time > 2
			else if(pid == 0) {
				// Writing to .out file
				fprintf(stdout, "Starting command %d: child PID: %d; parent PID: %d\n", finishedNode->index, getpid(), getppid());
				fflush(stdout);	
				// Re-setting active state of process
				finishedNode->active = true;
				// Converting 2D array in node to 1D array of char pointers
				char* temp[20];
				int rowNum = 0;
				while(*(currNode->command[rowNum]) != '\0' && rowNum < 20) {
					temp[rowNum] = finishedNode->command[rowNum];
					++rowNum;
				}
				temp[rowNum] = NULL;
				execvp(temp[0], temp);
	
				// Handling execvp() failure		
				printf("execvp() failed for process %d\n", getpid());
				perror(finishedNode->command[0]);
				return 2;
			}
			
			// PARENT process: just set the pid for the running process in currNode
			else {
				finishedNode->start_time = start.tv_sec;
				finishedNode->PID = pid;
			}
		}
		else if(elapsed < 2.0) {
			finishedNode->active = false;
			fprintf(stderr, "Spawning too fast\n");	
		}

	}		
	return 0;
}	
