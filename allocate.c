#include <stdio.h>
#include <stdlib.h> // For exit() function
#include <string.h>
#include <stdbool.h>
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
   new_node->cpu_ptr = NULL;                           // cpu_id = -1 means no cpu is assigned to the process yet
   new_node->next = NULL;


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

    /************************  Read In File  *********************************/

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

    /*****************************************************************************/



    /********************************    CPU     *********************************/

    int num_cpus = CPU_ARR_LENGTH;  //the argv[] value
    // printf("%d\n", num_cpus);

    struct cpu *cpu_array;
    //struct cpu *ptr_to_cpu_array = cpu_array;//int *p = a; 
    cpu_array = (struct cpu*)malloc(num_cpus * sizeof(struct cpu));
    for (int i=0; i<num_cpus; i++){

        // cpu_array[i] = malloc(sizeof(struct cpu))
        cpu_array[i].cpu_id = i;  //cpu_id with -1 indicates no CPU 

        //NEED TO INITIALISE THE ARRAY OF POINTERS TO STRUCT HERE SOMEHOW
        //Simplify array first to hold all the processes that are there
        // cpu_array[i].processes = (struct process**)malloc(proc_rem * sizeof(struct process*));

        for (int j=0; j<4; j++){
            (cpu_array[i].processes)[j] = NULL; 
        }

        // if memory cannot be allocated
        // if(cpu_array[i].processes == NULL)                     
        // {
        //     printf("Error! memory not allocated.");
        //     exit(0);
        // }

        // cpu_array[i].cpu_rem_exec_time = calc_remaining_cpu_exec_time(cpu_array[i].cpu_id, &cpu_array);
        cpu_array[i].cpu_rem_exec_time = 0;

    }  

    /*******************************************************************************/


    struct process* run = NULL; // To point to the running process
    // printf("%p\n",run);
    // struct process* run2; // For second processor

    /* Main Loop of The Program */
    while (proc_rem != 0){

        /* Check if any process arrived at current time */
        if (search(head, current_time)) {
            //CHECK IF THERE ARE OTHER PROCESSES ARRIVING AT THIS CURRENT TIME, i.e. search for duplicate arr_time
            if (has_same_arrival_times(head)){

                /*******************************************************************************/
                if (run != NULL){
                    if( get_process_with_smallest_rem_time_accounting_for_duplicates(head, current_time)->rem_exec_time < run->rem_exec_time ){
                        run = get_process_with_smallest_rem_time_accounting_for_duplicates(head, current_time);
                        add_process_to_cpu(run, &cpu_array);
                        printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, run->pid, run->rem_exec_time, run->cpu_ptr->cpu_id);
                    }
                    else {
                        // DO NOTHING, i.e. keep runnig the process that is already running
                    } 
                }
                else{
                    run = get_process_with_smallest_rem_time_accounting_for_duplicates(head, current_time);
                    add_process_to_cpu(run, &cpu_array);
                    printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, run->pid, run->rem_exec_time, run->cpu_ptr->cpu_id);
                }
                /*******************************************************************************/

                
            }
            //Put the duplicate ones in another Linked List
            //If there is, then loop through LL to find the one with lowest rem_exec_time
            //If there is a tie, break tie using pid and assign run pointer
            //else {the below code will run} 

            // if (head->arr_time == current_time){  // FOR FIRST PROCESS
            //     run = head; //Pointing to the running process
            //     add_process_to_cpu(run, &cpu_array); // Add pointer to the process to the relevant cpu
            //     printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, run->pid, run->rem_exec_time, run->cpu_ptr->cpu_id);
            //     // printf("CPU = %d , CPU_REM_EXEC_TIME = %d\n", run->cpu_ptr->cpu_id, run->cpu_ptr->cpu_rem_exec_time);
            // }
            
            //Below code is for any other process except for the first one

            // Check if any CPU is free i.e. has zero cpu_rem_exec_time to accommodate the process
            // for (int i=0; i<CPU_ARR_LENGTH; i++){
            //     if (cpu_array[i].cpu_rem_exec_time == 0){
            //         run2 = get_pointer_to_process_equal_to_curr_time(head, current_time);
            //     }
            // }

            // Check rem_exec_time of currently arrived process with the running process's rem_exec_time
            else {
                if (run != NULL) {
                    if (get_pointer_to_process_equal_to_curr_time(head, current_time)->rem_exec_time < run->rem_exec_time){
                        run = get_pointer_to_process_equal_to_curr_time(head, current_time);
                        add_process_to_cpu(run, &cpu_array); // Add pointer to the process to the relevant cpu
                        printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, run->pid, run->rem_exec_time, run->cpu_ptr->cpu_id);
                        // printf("CPU = %d , CPU_REM_EXEC_TIME = %d\n", run->cpu_ptr->cpu_id, run->cpu_ptr->cpu_rem_exec_time);
                    }
                    else {
                        // DO NOTHING, i.e. keep runnig the process that is already running
                    }
                }
                else {
                    run = get_pointer_to_process_equal_to_curr_time(head, current_time); //Else, run this process as it is the first process
                    add_process_to_cpu(run, &cpu_array); // Add pointer to the process to the relevant cpu
                    printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, run->pid, run->rem_exec_time, run->cpu_ptr->cpu_id); 
                }
            }
            // Else do nothing, i.e. keep run pointer to previous process            
        }


/************************************************* DECREMENT SECTION **********************************************************/

        // If there is no process for which the current time now is not equal to its arr_time, then below code will run
        (run->rem_exec_time)--; //To decrement the running process's execution time
        (run->cpu_ptr->cpu_rem_exec_time)--; //To decrement the occupied process's rem execution time
        current_time++;
        // printf("\n%d\n", run->rem_exec_time);

/******************************************************************************************************************************/


/************************************************* CHECK IF PROCESS ENDED SECTION **********************************************************/

        if ((run->rem_exec_time == 0)){
            proc_rem--;
            printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", current_time, run->pid, proc_rem);
            //run->cpu_ptr = NULL; //Need to remove from CPU after process have finished
            // printf("CPU = %d , CPU_REM_EXEC_TIME = %d\n", run->cpu_ptr->cpu_id, run->cpu_ptr->cpu_rem_exec_time);

            // CHECK AGAIN HERE TO SET THE PROCESS WITH THE SHORTEST EXECUTION TIME
            //Traverse trough LL from head to find node with lowest rem_exec_time and set it to the run pointer

            if(!is_all_process_completed(head)){   /*If All process's rem_exec_time is not equal to zero, i.e. if all processes did not complete yet*/
                // printf("\nALL PROCESS DID NOT COMPLETE\n");
                //get the process with the smallest rem_exec_time and set it to run pointer

                // IF there is a tie between rem times, then break tie using pid

                run = get_process_with_smallest_rem_time_breaking_ties(head);
                add_process_to_cpu(run, &cpu_array); // Add pointer to the process to the relevant cpu
                // run->cpu_ptr->cpu_rem_exec_time = (run->cpu_ptr->cpu_rem_exec_time) + (run->rem_exec_time); //Updating the cpu's remaining execution time

            }

            else{
                break;   // If all process is completed then BREAK out from the while loop
            }

            if (run->rem_exec_time != 0){ // If run pointer process is not FINISHED yet, then print the link below
                printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, run->pid, run->rem_exec_time, run->cpu_ptr->cpu_id);
                // printf("CPU = %d , CPU_REM_EXEC_TIME = %d\n", run->cpu_ptr->cpu_id, run->cpu_ptr->cpu_rem_exec_time);
            }

            // (run->rem_exec_time)--; //This increment needs to be added again for the unique case after a process finishes
            // current_time++;         //This decrement needs to be added again for the unique case after a process finishes

            
        }
/******************************************************************************************************************************/


    }  // MAIN LOOP ENDS

    
    fclose(fptr);

    return 0;
}


        //TRAVERSE AND PRINT LL
        // struct process* temp = head;
        // while (temp != NULL){
        //     printf("%d\n", temp->arr_time);
        //     temp = temp->next;
        // }