#include <stdio.h>
#include <stdlib.h>

void ComputeAverage(FILE *fp) {
    double lineCount = 0, currSum = 0;
    double currValue;

    // Continue to assign new lines as floating points to currValue until EOF
    while(fscanf(fp, "%lf", &currValue) == 1) {
        ++lineCount;
        currSum += currValue;
   }

   // Handling empty file input
   if(lineCount == 0) { 
      printf(0 + "\n");
      return;
   }

   // Print the average and handling empty file input
   printf("%lf\n", currSum / lineCount);
}

int main(int argc, char *argv[]) {
    // READING ('r') specified filepath
    FILE *fp = fopen(argv[1], "r");

    // Checking if 'fopen' call succeeded
    if(fp == NULL) {
        perror("avg");
        exit(1);
    }

    ComputeAverage(fp);
    fclose(fp);
    return 0;
}
