#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <stdbool.h>

#include "functions.c"

int main(int argc, char* argv[]) {

    struct process* head = NULL;  // name of the pointer is head and not pointing to anything...  

    int current_time=0;

    char line[1000]; // use malloc later

    int proc_rem = 0;

    char* file_name;

    int num_cpus;

    /************************  Read In File  *********************************/

    for (int i=0; i<argc; i++){

        if (strcmp(argv[i], "-f") == 0) {
            file_name = argv[i + 1];
        }
        if (strcmp(argv[i],"-p") == 0){
            num_cpus = atoi(argv[i + 1]);
        }
    }

    FILE *fptr;
    if ((fptr = fopen(file_name, "r")) == NULL) {
        printf("Error! opening file");
        // Program exits if file pointer returns NULL.
        exit(1);
    }

    // Reads text until newline is encountered
    while (EOF != fscanf(fptr, "%[^\n]\n", line)) {
        insert_process(&head, line);
        // proc_rem++;
    }

    /*****************************************************************************/



    /********************************    CPU     *********************************/

    struct cpu *cpu_array;
    cpu_array = (struct cpu*)malloc(num_cpus * sizeof(struct cpu));
    for (int i=0; i<num_cpus; i++){

        cpu_array[i].cpu_id = i;
        cpu_array[i].process_address_container_head = NULL;
        cpu_array[i].cpu_rem_exec_time = 0;
        cpu_array[i].running_process_ptr = NULL;

    }  

    /*******************************************************************************/


    /* Main Loop of The Program */
    while (!is_all_process_completed(head)){

        // printf("%d ", current_time);
        // print_CPU_process_list(cpu_array[0].process_address_container_head);
        // printf("%d ", current_time);
        // print_CPU_process_list(cpu_array[1].process_address_container_head);
        // printf("\nCurrent_time:%d , CPU-0-REM-TIME:%d , CPU-1-REM-TIME:%d\n", current_time, cpu_array[0].cpu_rem_exec_time, cpu_array[1].cpu_rem_exec_time );
        // RUN A FOR LOOP HERE FOR MORE THAN 1 CPUS
        
        /* Check if any process arrived at current time */
        if (search(head, current_time)) {

            //Check if there are multiple processes arriving at the same time, i.e. check if there are same arrival times value in the linked list of processes
            // if (has_same_arrival_times(head)){

            //     /*******************************************************************************/
            //     for (int i=0; i<num_cpus; i++) {// For loop for multiple CPUs
            //         if (cpu_array[i].running_process_ptr != NULL){ // If there is a process already running  
            //             if( get_process_with_smallest_rem_time_accounting_for_duplicates(head, current_time)->rem_exec_time < cpu_array[i].running_process_ptr->rem_exec_time ){
            //                 run = get_process_with_smallest_rem_time_accounting_for_duplicates(head, current_time);
            //                 if (run->cpu_ptr == NULL){
            //                     add_process_to_cpu(run, num_cpus, &cpu_array); // Add pointer to the process to the relevant cpu
            //                     // NEED A FUNCTION HERE TO SELECT THE PROCESS WITH THE LEAST REM TIME IN THE CPU'S PROCESSES LIST AS THE RUNNING_PROCESS_PTR
            //                     set_cpu_running_process_ptr(&(cpu_array[i]));
            //                     printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
            //                 }
            //                 //RUN A FOR LOOP HERE TO GO THROUGH ALL THE CPUS, GET THEIR RUN POINTERS AND THEN PRINT
            //                 // for (int i=0; i<num_cpus; i++){
            //                 //     if (cpu_array[i].running_process_ptr != NULL){
            //                 //         printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
            //                 //     }    
            //                 // }
            //                 // printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, run->pid, run->rem_exec_time, run->cpu_ptr->cpu_id);
            //                 // printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
            //             }
            //             else {
            //                 // DO NOTHING, i.e. keep runnig the process that is already running
            //             } 
            //         }
            //         else{  // This indicate run = NULL and implies no process is running yet
            //             run = get_process_with_smallest_rem_time_accounting_for_duplicates(head, current_time);
            //             if (run->cpu_ptr == NULL){
            //                 add_process_to_cpu(run, num_cpus, &cpu_array); // Add pointer to the process to the relevant cpu
            //                 // NEED A FUNCTION HERE TO SELECT THE PROCESS WITH THE LEAST REM TIME IN THE CPU'S PROCESSES LIST AS THE RUNNING_PROCESS_PTR
            //                 set_cpu_running_process_ptr(&(cpu_array[i]));
            //                 printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
            //             }

            //             // for (int i=0; i<num_cpus; i++){
            //             //     if (cpu_array[i].running_process_ptr != NULL){
            //             //         printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
            //             //     }    
            //             // }
            //             // printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, run->pid, run->rem_exec_time, run->cpu_ptr->cpu_id);
            //             // printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
            //         }
            //         /*******************************************************************************/
            //     } 
            // }
            
            // IF PROCESSES DON'T HAVE SAME ARRIVAL TIMES
            //else {

                // While all the process 'at this current time' does not have a cpu_id, insert process into CPU process list
                // Insert all the processes arriving at the same time to relevant CPUs
                
                if (has_same_arrival_times(head)){
                    struct process* curr = head;
                    while (curr != NULL){
                        if (curr->arr_time == current_time){
                            add_process_to_cpu(curr, num_cpus, &cpu_array);
                            proc_rem++;
                            curr = curr->next;
                        }
                        else{
                            curr = curr->next;
                        }
                    }
                }
                else {
                    add_process_to_cpu(get_pointer_to_process_equal_to_curr_time(head, current_time), num_cpus, &cpu_array);
                    proc_rem++;
                }

                for (int i=0; i<num_cpus; i++){ // For loop for multiple CPUs

                    // If a process is already running in that CPU
                    if (cpu_array[i].running_process_ptr != NULL) {  
                        // Check if the currently running process in the CPU is the same as the newly assigned process in the CPU 
                        if (cpu_array[i].running_process_ptr == set_cpu_running_process_ptr(&(cpu_array[i]))){
                            //If it is the same process being run, then no need to print it out
                        }
                        else {
                            printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);

                        }
                    }
                    
                    // Else if no process is running in that CPU
                    else {
                            set_cpu_running_process_ptr(&(cpu_array[i]));
                            if (cpu_array[i].running_process_ptr != NULL){
                                printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", current_time, cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
                            }
                        // }
                        
                    }
                } // END OF FOR LOOP
            //}          
        }
/************************************************* DECREMENT SECTION **********************************************************/

        // If there is no process for which the current time is equal to its arr_time, then below code will run
        for (int i=0; i<num_cpus; i++){ // For loop for multiple CPUs
            if ((cpu_array[i].running_process_ptr != NULL) ){ // To check if there is any process allocated to the CPU or not
                (cpu_array[i].running_process_ptr->rem_exec_time)--; //To decrement the running process's execution time
                (cpu_array[i].cpu_rem_exec_time)--; //To decrement the CPU's remaining execution time that is allocated to the running process
            }        
        }
        current_time++;

/******************************************************************************************************************************/



/************************************************* CHECK IF PROCESS ENDED SECTION **********************************************************/

        for (int i=0; i<num_cpus; i++) {
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
                }
                
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