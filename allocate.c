#include <stdio.h>
#include <stdlib.h> // For exit() function
#include <string.h>

struct process {
    int arr_time;                // make unsigned long int
    int pid;      // make unsigned long int
    int exec_time;
    char *parallelisability;     // Not sure if needed for now // need to conver to just character later
    struct process *next; // Pointer pointing to the next process stored in the Linked List

    // store CPU id too - -1 for no cpu
};




void insert_process(struct process** head_ptr, char* process_data) {

   struct process* new_node = (struct process*)malloc(sizeof(struct process));
   struct process* temp;
   
   // Extract the first token
   char * token = strtok(process_data, " ");
   int count = 0;
   // loop through the string to extract all other tokens
   while( token != NULL ) {
    //   printf("%s\n", token); //printing each token

    /* Inserting into struct */
      if (count==0) {
          new_node->arr_time = atoi(token);
      }
      else if (count==1){
          new_node->pid = atoi(token);
      }
      else if (count==2){
          new_node->exec_time = atoi(token);
      }
      else if (count==3){
          new_node->parallelisability = token;
      } 
      
    /****************************/
      token = strtok(NULL, " ");
      count++;
   }

   new_node->next = NULL;

//    printf("%d\n", new_node->arr_time);
//    printf("%d\n", new_node->pid);
//    printf("%d\n", new_node->exec_time);
//    printf("%s\n", new_node->parallelisability);
//    printf("%p\n", new_node->next);                // Confusion on this output
//    printf("\n");

   if (*head_ptr == NULL){
       *head_ptr = new_node;
    //    printf("%d\n", (*head_ptr)->arr_time);
       return; 
   }

   temp = *head_ptr;
   while (temp->next != NULL) {
       temp = temp->next;
   }
   temp->next = new_node;
   return;

}

//void insert_process(char* process_data) {

    // create_node(process_data);

 /*    // first process or not
    if (head == NULL) {
        head = (struct process *)malloc(sizeof(struct process));
        head->next = 
    } */

    // If First process -> Create Node -> Move "head pointer" from "Null" to that "Node address"
    

    // If Latter processes -> Create Node -> Traverse to end of Linked List -> Insert process there

    // If Last process read -> Create Node -> Traverse to end of Linked List -> Insert process there -> Point the "next" poniter to "Null"
//}

int main(int argc, char* argv[]) {

    struct process* head = NULL;  // name of the pointer is head and not pointing to anything...  

    char line[1000]; // use malloc later

    int proc_rem = 0;
        
    FILE *fptr;
    if ((fptr = fopen("processes.txt", "r")) == NULL) {
        printf("Error! opening file");
        // Program exits if file pointer returns NULL.
        exit(1);
    }

    // reads text until newline is encountered
    while (EOF != fscanf(fptr, "%[^\n]\n", line)) {
        insert_process(&head, line);
        proc_rem++;
    }

    // printf("HEAD POINTER = %p\n", head);
    struct process* temp = head;
    while (temp != NULL){
        printf("%d\n", temp->arr_time);
        temp = temp->next;
    }
    
    
    fclose(fptr);

    return 0;
}