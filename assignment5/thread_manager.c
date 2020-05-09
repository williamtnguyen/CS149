#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

// Include ListNode and THREAD_DATA structs
#include "ThreadData.h"
#include "ListNode.h"


// Thread mutex lock for access to the log index
pthread_mutex_t tlock1 = PTHREAD_MUTEX_INITIALIZER;
// Thread mutex lock for critical sections of allocating THREADDATA
pthread_mutex_t tlock2 = PTHREAD_MUTEX_INITIALIZER;
// Thread mutex lock for critical sections of allocating/reading ListNodes
pthread_mutex_t tlock3 = PTHREAD_MUTEX_INITIALIZER;

// pthread, start routine, and THREADDATA object pointers
pthread_t tid1, tid2;
void* thread_runner(void*);
THREAD_DATA* p = NULL;

// Variable for indexing of messages by the logigng function
int logIndex = 0;
int *logIndexPtr = &logIndex;

// Flag to indicate if the reading of input is complete, so the other thread knows when to stop
bool isReadingComplete = false;

// Condition variable for Thread 2 to wait for linked list changes from Thread 1
pthread_cond_t listCondition = PTHREAD_COND_INITIALIZER;
bool isUpdated = false;


/* Function main creates 2 threads and waits for them to finish */ 
int main() {
	
	printf("Create First Thread\n");
	pthread_create(&tid1, NULL, thread_runner, NULL);
	
	printf("Create Second Thread\n");
	pthread_create(&tid2, NULL, thread_runner, NULL);

	printf("Wait for the First Thread to exit\n");
	pthread_join(tid1, NULL);
	printf("First Thread exited!\n");

	printf("Wait for the Second Thread to exit\n");
	pthread_join(tid2, NULL);
	printf("Second Thread exited!\n");

	exit(0);
}


/* Function thread_runner runs inside each thread */
void* thread_runner(void* x) {

	// Initializing the linked list headNode & currNode pointers for thread_runner (shares the call stack)
	ListNode* headNode = (ListNode*) malloc(sizeof(ListNode));
	free(headNode);
	ListNode* currNode = NULL;
	
	// For standard input
	char buffer[100];
	char* input;

	pthread_t currThread;
	currThread = pthread_self();
	printf("This is thread %ld (p=%p)\n", currThread, p);
	

	// CRITICAL SECTION: allocating memory for THREAD_DATA object
	pthread_mutex_lock(&tlock2);
	if(p == NULL) {
		p = (THREAD_DATA*) malloc(sizeof(THREAD_DATA));
		p->creator = currThread;
	}
	pthread_mutex_unlock(&tlock2);

	
	// Thread 1 (creator/signaller): creating a linked list that stores contents from stdin
	if(p != NULL && p->creator == currThread) {
		printf("This is thread %ld and I created the THREAD_DATA %p\n", currThread, p);
		
		// Continue to read from stdin until Ctrl+C or '\n'
		while((input = fgets(buffer, 100, stdin)) != NULL) {
			if(*input == '\n') break;
			
			// CRITICAL SECTION: prepending to head of linked list (for quicker access times in Thread 2)
			pthread_mutex_lock(&tlock3);

			currNode = (ListNode*) malloc(sizeof(ListNode));
			CreateListNode(currNode, input, headNode);
			headNode = currNode;			
			isUpdated = true;			

			pthread_cond_signal(&listCondition); // letting thread 2 know that he can print now
			pthread_mutex_unlock(&tlock3);
		}
		isReadingComplete = true;
	}
 
	// Thread 2 (sleeper/waiter): prints the head of the linkedlist
	else {
		printf("This is thead %ld and I can access the THREAD_DATA %p\n", currThread, p);

		while(!isReadingComplete) {
			pthread_mutex_lock(&tlock3);
			while(!isUpdated)
				pthread_cond_wait(&listCondition, &tlock3);

			// log a message, read contents of headNode and print it
			char* headInput = headNode->input;
			isUpdated = false;
			pthread_mutex_unlock(&tlock3);
			//TODO: logIndex, thread #, PID, date/time in log message
			// print here; 
			printf("current head node has %s\n", headInput);
		}	
	}


	// CRITICAL SECTION: deallocating memory from the THREAD_DATA object
	pthread_mutex_lock(&tlock2);
	if(p != NULL && p->creator == currThread) {
		printf("This is thread %ld and I did not touch THREAD_DATA object\n", currThread);
	} else {
		free(p);
		p = NULL;
		printf("This is thread %ld and I deleted the THREAD_DATA object\n", currThread);
	}
	pthread_mutex_unlock(&tlock2);
	
	// TODO: free the linked list nodes
	
	pthread_exit(NULL);
	return NULL;
}
