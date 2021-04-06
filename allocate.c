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

    struct cpu *cpu_array;
    //struct cpu *ptr_to_cpu_array = cpu_array;//int *p = a; 
    cpu_array = (struct cpu*)malloc(num_cpus * sizeof(struct cpu));
    for (int i=0; i<num_cpus; i++){

        // cpu_array[i] = malloc(sizeof(struct cpu))
        cpu_array[i].cpu_id = i;  //cpu_id with -1 indicates no CPU 

        //NEED TO INITIALISE THE ARRAY OF POINTERS TO STRUCT HERE SOMEHOW
        //Simplify array first to hold all the processes that are there
        // cpu_array[i].processes = (struct process**)malloc(proc_rem * sizeof(struct process*));

        // for (int j=0; j<PROCESSES_ARR_LENGTH; j++){
        //     (cpu_array[i].processes)[j] = NULL; 
        // }

        cpu_array[i].process_address_container_head = NULL;

        // if memory cannot be allocated
        // if(cpu_array[i].processes == NULL)                     
        // {
        //     printf("Error! memory not allocated.");
        //     exit(0);
        // }

        // cpu_array[i].cpu_rem_exec_time = calc_remaining_cpu_exec_time(cpu_array[i].cpu_id, &cpu_array);
        cpu_array[i].cpu_rem_exec_time = 0;
        cpu_array[i].running_process_ptr = NULL;

    }  

    /*******************************************************************************/


    struct process* run = NULL; // To point to the running process
    // printf("%p\n",run);
    // struct process* run2; // For second processor

    /* Main Loop of The Program */
    while (proc_rem != 0){

        print_CPU_process_list(cpu_array[0].process_address_container_head);
        print_CPU_process_list(cpu_array[1].process_address_container_head);

        // if (current_time == 120){
        //     break;
        // }
        // printf("\nCurrent_time:%d , CPU-0-REM-TIME:%d , CPU-1-REM-TIME:%d\n", current_time, cpu_array[0].cpu_rem_exec_time, cpu_array[1].cpu_rem_exec_time );

        // RUN A FOR LOOP HERE FOR MORE THAN 1 CPUS
        
        /* Check if any process arrived at current time */
        if (search(head, current_time)) {

            //Check if there are multiple processes arriving at the same time, i.e. check if there are same arrival times value in the linked list of processes
            if (has_same_arrival_times(head)){

                /*******************************************************************************/
                // for (int i=0; i<CPU_ARR_LENGTH; i++) {// For loop for multiple CPUs
                //     if (cpu_array[i].running_process_ptr != NULL){ // If there is a process already running  
                //         if( get_process_with_smallest_rem_time_accounting_for_duplicates(head, current_time)->rem_exec_time < cpu_array[i].running_process_ptr->rem_exec_time ){
                //             run = get_process_with_smallest_rem_time_accounting_for_duplicates(head, current_time);
                //             if (run->cpu_ptr == NULL){
                //                 add_process_to_cpu(run, &cpu_array); // Add pointer to the process to the relevant cpu
                //                 // NEED A FUNCTION HERE TO SELECT THE PROCESS WITH THE LEAST REM TIME IN THE CPU'S PROCESSES LIST AS THE RUNNING_PROCESS_PTR
                //                 set_cpu_running_process_ptr(&(cpu_array[i]));
                //                 printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
                //             }
                //             //RUN A FOR LOOP HERE TO GO THROUGH ALL THE CPUS, GET THEIR RUN POINTERS AND THEN PRINT
                //             // for (int i=0; i<CPU_ARR_LENGTH; i++){
                //             //     if (cpu_array[i].running_process_ptr != NULL){
                //             //         printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
                //             //     }    
                //             // }
                //             // printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, run->pid, run->rem_exec_time, run->cpu_ptr->cpu_id);
                //             // printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
                //         }
                //         else {
                //             // DO NOTHING, i.e. keep runnig the process that is already running
                //         } 
                //     }
                //     else{  // This indicate run = NULL and implies no process is running yet
                //         run = get_process_with_smallest_rem_time_accounting_for_duplicates(head, current_time);
                //         if (run->cpu_ptr == NULL){
                //             add_process_to_cpu(run, &cpu_array); // Add pointer to the process to the relevant cpu
                //             // NEED A FUNCTION HERE TO SELECT THE PROCESS WITH THE LEAST REM TIME IN THE CPU'S PROCESSES LIST AS THE RUNNING_PROCESS_PTR
                //             set_cpu_running_process_ptr(&(cpu_array[i]));
                //             printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
                //         }

                //         // for (int i=0; i<CPU_ARR_LENGTH; i++){
                //         //     if (cpu_array[i].running_process_ptr != NULL){
                //         //         printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
                //         //     }    
                //         // }
                //         // printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, run->pid, run->rem_exec_time, run->cpu_ptr->cpu_id);
                //         // printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
                //     }
                //     /*******************************************************************************/
                // } 
            }
            
            // IF PROCESSES DON"T HAVE SAME ARRIVAL TIMES
            else {
                for (int i=0; i<CPU_ARR_LENGTH; i++){ // For loop for multiple CPUs
                
                    // If a process is already running in that CPU
                    if (cpu_array[i].running_process_ptr != NULL) {  
                        if (get_pointer_to_process_equal_to_curr_time(head, current_time)->rem_exec_time < cpu_array[i].running_process_ptr->rem_exec_time){
                            // NEED TO STOP CALLING IT "RUN"
                            run = get_pointer_to_process_equal_to_curr_time(head, current_time);
                            if (run->cpu_ptr == NULL){
                                add_process_to_cpu(run, &cpu_array); // Add pointer to the process to the relevant cpu
                                // NEED A FUNCTION HERE TO SELECT THE PROCESS WITH THE LEAST REM TIME IN THE CPU'S PROCESSES LIST AS THE RUNNING_PROCESS_PTR
                                set_cpu_running_process_ptr(&(cpu_array[i]));
                                printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
                            }
                        }
                        else if (get_pointer_to_process_equal_to_curr_time(head, current_time)->rem_exec_time == cpu_array[i].running_process_ptr->rem_exec_time) {
                            if (get_pointer_to_process_equal_to_curr_time(head, current_time)->pid < cpu_array[i].running_process_ptr->pid){
                                run = get_pointer_to_process_equal_to_curr_time(head, current_time);
                            }
                            if (run->cpu_ptr == NULL){
                                add_process_to_cpu(run, &cpu_array); // Add pointer to the process to the relevant cpu
                                // NEED A FUNCTION HERE TO SELECT THE PROCESS WITH THE LEAST REM TIME IN THE CPU'S PROCESSES LIST AS THE RUNNING_PROCESS_PTR
                                set_cpu_running_process_ptr(&(cpu_array[i]));
                                printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
                            }
                        }
                        else{
                            // DO NOTHING, i.e. keep runnig the process that is already running
                        }
                    }
                    
                    // Else if no process is running in that CPU
                    else {
                        run = get_pointer_to_process_equal_to_curr_time(head, current_time); //Else, run this process as it is the first process
                        //Check if the process is already allocated to a CPU
                        if (run->cpu_ptr == NULL){
                            add_process_to_cpu(run, &cpu_array); // Add pointer to the process to the relevant cpu
                            // NEED A FUNCTION HERE TO SELECT THE PROCESS WITH THE LEAST REM TIME IN THE CPU'S PROCESSES LIST AS THE RUNNING_PROCESS_PTR
                            set_cpu_running_process_ptr(&(cpu_array[i]));
                            printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
                        }
                        
                    }
                } // END OF FOR LOOP
            }          
        }
        // if (current_time == 20){
        //     print_CPU_process_list(cpu_array[1].process_address_container_head);
        // }

/************************************************* DECREMENT SECTION **********************************************************/

        // If there is no process for which the current time is equal to its arr_time, then below code will run
        for (int i=0; i<CPU_ARR_LENGTH; i++){ // For loop for multiple CPUs
            if ((cpu_array[i].running_process_ptr != NULL) ){ // To check if there is any process allocated to the CPU or not
                (cpu_array[i].running_process_ptr->rem_exec_time)--; //To decrement the running process's execution time
                (cpu_array[i].cpu_rem_exec_time)--; //To decrement the CPU's remaining execution time that is allocated to the running process
            }        
        }
        current_time++;
        // printf("\n%d\n", current_time);

/******************************************************************************************************************************/



/************************************************* CHECK IF PROCESS ENDED SECTION **********************************************************/

        for (int i=0; i<CPU_ARR_LENGTH; i++) { // For loop for multiple CPUs
            // Checks if the running process has finished executing
            if ((cpu_array[i].running_process_ptr != NULL) && (cpu_array[i].running_process_ptr->rem_exec_time == 0)){

                proc_rem--;

                printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", current_time, cpu_array[i].running_process_ptr->pid, proc_rem);

                // CHECK AGAIN HERE TO SET THE PROCESS WITH THE SHORTEST EXECUTION TIME
                //Traverse trough LL from head to find node with lowest rem_exec_time and set it to the run pointer
                if (is_all_process_completed(head)){   /*If All process's rem_exec_time is equal to zero, i.e. if all processes are completed*/
                    break;   // If all process is completed then BREAK out from the while loop
                }

                else{
                    // Check the current CPU for any other processes that can be run
                    // IF yes, then set it as the run pointer of the current cpu, and PRINT it out
                    // If no, then don't print out anything of the current CPU, and move to the next CPU
                    set_cpu_running_process_ptr(&(cpu_array[i]));
                    if( cpu_array[i].running_process_ptr != NULL){
                        printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
                    } 
                    else {
                        break;
                    }

                    //get the process with the smallest rem_exec_time and set it to run pointer
                    // IF there is a tie between rem times, then break tie using pid

                    // run = get_process_with_smallest_rem_time_breaking_ties(head);
                    // printf("\n %d \n", run->pid);
                    // if (run->cpu_ptr == NULL){
                    //     add_process_to_cpu(run, &cpu_array); // Add pointer to the process to the relevant cpu
                    //     // NEED A FUNCTION HERE TO SELECT THE PROCESS WITH THE LEAST REM TIME IN THE CPU'S PROCESSES LIST AS THE RUNNING_PROCESS_PTR
                    //     // printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
                    // }
                    // set_cpu_running_process_ptr(&(cpu_array[i]));
                    // printf("\n %d \n",cpu_array[i].running_process_ptr->pid);
                    // printf("\n RUNNING PROCESS: %d \n",cpu_array[i].running_process_ptr->pid);
                }

                // printf("\n%d, REM:%d FIRST PROCESS CPU0:%d : %d, SECOND PROCESS CPU0:%d : %d\n",cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[0].process_address_container_head->process_ptr->pid,cpu_array[0].process_address_container_head->process_ptr->rem_exec_time ,cpu_array[0].process_address_container_head->next->process_ptr->pid, cpu_array[0].process_address_container_head->next->process_ptr->rem_exec_time);
                // if ( (cpu_array[i].running_process_ptr != NULL) && (cpu_array[i].running_process_ptr->rem_exec_time != 0) ){ // If run pointer process is not FINISHED yet, then print the statement below (THIS WILL ALWAYS HOLD TRUE THOUGH)
                //     printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
                //     // printf("CPU = %d , CPU_REM_EXEC_TIME = %d\n", run->cpu_ptr->cpu_id, run->cpu_ptr->cpu_rem_exec_time);
                // }
                
            }
        } // FOR LOOP ENDS

        // If all process is completed then BREAK out from the while loop
        if(is_all_process_completed(head)){
            break;                              
        }
/******************************************************************************************************************************/

    }  // MAIN LOOP ENDS

/******************************************************************************************************************************/

    
    fclose(fptr);

    return 0;
}























/************** Checking the processes inside each CPU **********/

// for (int i=0; i<CPU_ARR_LENGTH; i++){
//     for (int j=0; j<PROCESSES_ARR_LENGTH; j++){
//             if (cpu_array[i].processes[j] != NULL){
//                 printf(" %d ", cpu_array[0].processes[j]->pid);
//             }
//     }
//     printf("\n");
// }
/****************************************************************/