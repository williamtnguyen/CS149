#ifndef THREADDATA_H
#define THREADDATA_H
#include <pthread.h>

/**
 * Struct for an object that stores the pthread_t pointer of the thread that allocated it
 */

typedef struct thread_data_struct {
	pthread_t creator;
} THREAD_DATA;

#endif
