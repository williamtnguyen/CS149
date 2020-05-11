#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

// Include ListNode and THREAD_DATA structs
#include "ThreadData.h"
#include "ListNode.h"


// Thread mutex lock for access to the logIndex
pthread_mutex_t tlock1 = PTHREAD_MUTEX_INITIALIZER;
// Thread mutex lock for critical sections of allocating THREAD_DATA
pthread_mutex_t tlock2 = PTHREAD_MUTEX_INITIALIZER;
// Thread mutex lock for critical sections of allocating/reading ListNodes
pthread_mutex_t tlock3 = PTHREAD_MUTEX_INITIALIZER;
// Thread mutex lock for boolean isReadingComplete
pthread_mutex_t tlock4 = PTHREAD_MUTEX_INITIALIZER;


/* ~~~ SHARED VARIABLES: initialized globally because threads get their own stack ~~~ */

// pthread, start routine, and THREADDATA object pointers
pthread_t tid1, tid2;
void* thread_runner(void*);
THREAD_DATA* p = NULL;

// Variable for indexing of messages by the logging function
int logIndex = 0;
int *logIndexPtr = &logIndex;

// Flag to indicate if the reading of input is complete, so the other thread knows when to stop
bool isReadingComplete = false;

// Condition variable for Thread 2 to wait for linked list changes from Thread 1
pthread_cond_t listCondition = PTHREAD_COND_INITIALIZER;
bool isUpdated = false;

// headNode and currNode pointers for linked list
ListNode* headNode = NULL;
ListNode* currNode = NULL;

// For standard input
char buffer[100];
char* input;


/* Helper function: prints current data and time in C */
char* get_time() {
	// Time/Date string to be returned
	char timeString[20];

	// variables to store date and time components
	int hours, minutes, seconds, day, month, year;

	// time_t is arithmetic time type
	time_t now;
	
	// time() returns the current time of the system as a time_t value
	time(&now);
	
	// localtime converts a time_t value to calendar time and returns
	// a pointer to a tm structure with its members filled with the corresponding values
	struct tm* local = localtime(&now);

	hours = local->tm_hour;
	minutes = local->tm_min;
	seconds = local->tm_sec;
	
	day = local->tm_mday;
	month = local->tm_mon + 1;
	year = local->tm_year + 1990;
	
	// Before midday
	if(hours < 12) {
		sprintf(timeString, "%02d/%02d/%d %02d:%02d:%02d AM", day, month, year, hours, minutes, seconds);
	} 
	// After midday
	else {
		sprintf(timeString, "%02d/%02d/%d %02d:%02d:%02d PM", day, month, year, hours - 12, minutes, seconds);
	}
	return timeString;
} 


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

	pthread_t currThread;
	currThread = pthread_self();
	printf("This is thread %ld (p=%p)\n", currThread, p);
	

	// CRITICAL SECTION: allocating memory for THREAD_DATA object
	pthread_mutex_lock(&tlock2);
	if(p == NULL) {
		p = (THREAD_DATA*) malloc(sizeof(THREAD_DATA));
		p->creator = currThread;
		// CRITICAL SECTION: printing activity
		pthread_mutex_lock(&tlock1);
		printf("LogIndex %d, Thread %ld, PID %d: allocated memory for THREAD_DATA\n", ++logIndex, currThread, getpid());
		pthread_mutex_unlock(&tlock1);
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

			assert(!isUpdated);
			currNode = (ListNode*) malloc(sizeof(ListNode));
			CreateListNode(currNode, input, headNode);
			headNode = currNode;			
			isUpdated = true;		

			// CRITICAL SECTION: logging activity
			pthread_mutex_lock(&tlock1);
			printf("LogIndex %d, Thread %ld, PID %d, %s: allocated memory for a new node\n", ++logIndex, currThread, getpid(), get_time());
			pthread_mutex_unlock(&tlock1);

			pthread_cond_signal(&listCondition); // letting thread 2 know that it can print now
			pthread_mutex_unlock(&tlock3);
		}
		pthread_mutex_lock(&tlock4);
		isReadingComplete = true;
		isUpdated = true;
		pthread_cond_signal(&listCondition); // letting thread 2 know its time to deallocate and exit
		pthread_mutex_unlock(&tlock4);
	}
 
	// Thread 2 (sleeper/waiter): prints the head of the linkedlist
	else {
		printf("This is thread %ld and I can access the THREAD_DATA %p\n\n", currThread, p);
		
		while(!isReadingComplete) {
			pthread_mutex_lock(&tlock3);
			while(!isUpdated)
				pthread_cond_wait(&listCondition, &tlock3);

			// log a message, read contents of headNode and print it
			assert(isUpdated);
			if(isReadingComplete) {
				pthread_mutex_unlock(&tlock3);
				break;
			}
			
			char* headInput = headNode->input;
			isUpdated = false;
			pthread_mutex_unlock(&tlock3);

			// CRITICAL SECTION: logging activity
			pthread_mutex_lock(&tlock1);
			printf("LogIndex %d, Thread %ld, PID %d, %s: head of linked list contains %s\n", ++logIndex, currThread, getpid(), get_time(), headInput);
			pthread_mutex_unlock(&tlock1);
		}	
	}


	// CRITICAL SECTION: deallocating memory from the THREAD_DATA object
	pthread_mutex_lock(&tlock2);
	if(p != NULL && p->creator != currThread){
		free(p);
		p = NULL;

		pthread_mutex_lock(&tlock1);
		printf("This is thread %ld and I deleted the THREAD_DATA object\n", currThread);
		pthread_mutex_unlock(&tlock1);
	} else {
		pthread_mutex_lock(&tlock1);
		printf("LogIndex %d, Thread %ld, PID %d: I did not touch THREAD_DATA object\n", ++logIndex, currThread, getpid());
		pthread_mutex_unlock(&tlock1);
	} 
	pthread_mutex_unlock(&tlock2);


	// CRITICAL SECTION: deallocate linked list	
	pthread_mutex_lock(&tlock3);
	if(headNode != NULL) {
		FreeNodes(headNode);
		headNode = NULL;

		pthread_mutex_lock(&tlock1);
		printf("LogIndex %d, Thread %ld, PID %d: freeing linked list. prepare for deallocation..\n", ++logIndex, currThread, getpid());
		pthread_mutex_unlock(&tlock1);
	}
	pthread_mutex_unlock(&tlock3);	
	
	return NULL;
}
