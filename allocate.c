#include <stdio.h>
#include <stdlib.h> // For exit() function
#include <string.h>

struct process {
    int pid;                // make unsigned long int
    int rem_exec_time;      // make unsigned long int
    char *curr_state;
    char *parallelisability;     // Not sure if needed for now // need to conver to just character later

    struct process *next; // Pointer pointing to the next process stored in the Linked List
};



void insert_process(char* process_data) {

    // If First process -> Create Node -> Move "head pointer" from "Null" to that "Node address"
    

    // If Latter processes -> Create Node -> Traverse to end of Linked List -> Insert process there

    // If Last process read -> Create Node -> Traverse to end of Linked List -> Insert process there -> Point the "next" poniter to "Null"
}

int main(int argc, char* argv[]) {

    char line[1000]; // use malloc later

    struct process* head = NULL;  // name of the pointer is head and not pointing to anything...
    
    
    FILE *fptr;
    if ((fptr = fopen("processes.txt", "r")) == NULL) {
        printf("Error! opening file");
        // Program exits if file pointer returns NULL.
        exit(1);
    }

    // reads text until newline is encountered
    while (EOF != fscanf(fptr, "%[^\n]\n", line)) {
        insert_process(line);
    }
    
    fclose(fptr);

    return 0;
}