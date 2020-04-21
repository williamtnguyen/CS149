#ifndef TRACENODE_H
#define TRACENODE_H

/**
 * Struct for a Memory-Tracing Node and function signatures for a Memory-Tracing Stack
 */

typedef struct trace_struct {
	char* functionID;
	int index;
	struct trace_struct* nextNodePtr; 
} TraceNode;

// Pointer to the top of the stack
static TraceNode* TRACE_TOP = NULL;

// Pushes a function to the stack
void PUSH_TRACE(char* p);
// Pops/removes the top of the stack
void POP_TRACE();
// Prints a sequence of function calls that are on the stack at the current instance
char* PRINT_TRACE();

#endif
