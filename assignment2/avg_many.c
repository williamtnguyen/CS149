#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void ComputeAverage(FILE *fp, int fd) {
    double lineCount = 0, currSum = 0;
    double currValue;
    
    // Continue to assign new lines as floating points to currValue until EOF 
    if(fp != NULL) {
	while(fscanf(fp, "%lf", &currValue) == 1) { 
            ++lineCount;
            currSum += currValue;
        }
    }
    
    write(fd, &currSum, sizeof(currSum));
    write(fd, &lineCount, sizeof(lineCount));
    close(fd);
}


int main(int argc, char *argv[]) {
    
    // For IPC
    int fd[2];
    pid_t pid;
    double totalSum = 0;
    double totalLineCount = 0;

    // No files specified in command line
    if(argc == 1) {
        printf("Invalid input. Pass in 1 or more existing files\n");
        return 0;
    }


    for(int i = 1; i < argc; i++) {
	
    	// Initializing the pipe
    	if(pipe(fd) == -1) {
	    fprintf(stderr, "Pipe Failed");
	    return 1;
    	}
	
        pid = fork();

        if(pid < 0) { 
	    fprintf(stderr, "Fork Failed");
	    return 1; 
        }
	// CHILD Process
	else if(pid == 0) {
	    // Closing the reading end of the pipe
	    close(fd[0]);

	    // READING ('r') specified filepath
	    FILE *fp = fopen(argv[i], "r");

	    if(fp == NULL) {
	        //perror("avg");
	 	//exit(1);
		fprintf(stderr, "Cannot open '%s'\n", argv[i]);	
    	    }
	
	    // Passing writing end of pipe to the function
	    ComputeAverage(fp, fd[1]);
	    // Closing writing end of the pipe
	    close(fd[1]);
	    fclose(fp);
	    exit(0);
	}
	// PARENT process
	else {
	    int status;
	    double processSum, processLineCount;	  
	    waitpid(pid, &status, 0);	
	   
	    // Closing the writing end of the pipe
	    close(fd[1]);
	    read(fd[0], &processSum, sizeof(processSum));
	    read(fd[0], &processLineCount, sizeof(processLineCount));
	    close(fd[0]);
	    
	    // Current file being processed has no contents
	    if(processLineCount == 0) {
	        printf("DISCLAIMER: '%s' has no contents or it DNE.\n", argv[i]);
	    }

	    totalSum += processSum;
	    totalLineCount += processLineCount;
	}

    }
    
    // Handling empty file input: all file args are empty
    if(totalLineCount == 0) { 
        printf("All file(s) passed have no contents.\n");
        return 0;
    }

    printf("%lf\n", totalSum / totalLineCount);
    return 0;
}
