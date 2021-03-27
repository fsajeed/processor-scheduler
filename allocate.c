#include <stdio.h>
#include <stdlib.h> // For exit() function
#include <string.h>
#include "allocate.h"

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

   new_node->rem_exec_time = new_node->exec_time;
   new_node->cpu_id = 0;
   new_node->next = NULL;

//    printf("%d\n", new_node->arr_time);
//    printf("%d\n", new_node->pid);
//    printf("%d\n", new_node->exec_time);
//    printf("%s\n", new_node->parallelisability);
//    printf("%p\n", new_node->next);                // Confusion on this output
//    printf("\n");

   if (*head_ptr == NULL){
       *head_ptr = new_node;
       return; 
   }

   temp = *head_ptr;
   while (temp->next != NULL) {
       temp = temp->next;
   }
   temp->next = new_node;
   return;

}

int main(int argc, char* argv[]) {

    struct process* head = NULL;  // name of the pointer is head and not pointing to anything...  

    int current_time=0;

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


    struct process* run; // To point to the running process

    while (current_time != 130){

        //run = head; // Because first process always run first

        if (search(head, current_time))/* Check if any process arrived at current time */{

            if (head->arr_time == current_time){  // FOR FIRST PROCESS

                run = head; //Pointing to the running process

                printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, run->pid, run->rem_exec_time, run->cpu_id);

                // (head->rem_exec_time)--;

            }
            
            
            //Below code is for any other process except for the first one
            // Check rem_exec_time of currently arrived process with the running process's rem_exec_time
            if (get_pointer_to_process_equal_to_curr_time(head, current_time)->rem_exec_time < run->rem_exec_time){
                run = get_pointer_to_process_equal_to_curr_time(head, current_time);
                printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, run->pid, run->rem_exec_time, run->cpu_id);
            }
            // Else do nothing, i.e. keep run pointer to previous process
            


        }


        // If there is no process for which the current time now is not equal to its arr_time, then below code will run
        (run->rem_exec_time)--; //To decrement the running process's execution time
        current_time++;
        // printf("\n%d\n", run->rem_exec_time);

        if ((run->rem_exec_time == 0)){
            proc_rem--;
            printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", current_time, run->pid, proc_rem);

            // CHECK AGAIN HERE TO SET THE PROCESS WITH THE SHORTEST EXECUTION TIME
            //Traverse trough LL from head to find node with lowest rem_exec_time and set it to the run pointer

            if(!is_all_process_completed(head)){   /*If All process's rem_exec_time is not equal to zero, i.e. if all processes did not complete yet*/
                // printf("\nALL PROCESS DID NOT COMPLETE\n");
                //get the process with the smallest rem_exec_time and set it to run pointer
                run = get_shortest_rem_exec_time_process(head);

            }

            if (run->rem_exec_time != 0){ // If run pointer process is not FINISHED yet, then print the link below
                printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, run->pid, run->rem_exec_time, run->cpu_id);
            }

            (run->rem_exec_time)--; //This increment needs to be added again for the unique case after a process finishes
            current_time++;         //This decrement needs to be added again for the unique case after a process finishes

            
        }
        
        // printf("%d\n", current_time);
    }

    
    fclose(fptr);

    return 0;
}


        //TRAVERSE AND PRINT LL
        // struct process* temp = head;
        // while (temp != NULL){
        //     printf("%d\n", temp->arr_time);
        //     temp = temp->next;
        // }