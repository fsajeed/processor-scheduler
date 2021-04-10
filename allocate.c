#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "functions.c"
#define MAX_FILE_LINE_LENGTH 50 // 3 unsigned long int data types (arrival time, process id and remaining execution time) and one 1 integer data type(CPU id) along with some whitespaces

int main(int argc, char* argv[]) {

    struct process* head = NULL;  // The head pointer to the main Linked List where all the processes will be read in first

    unsigned long int current_time=0;

    char line[MAX_FILE_LINE_LENGTH]; 

    unsigned long int proc_rem = 0;

    unsigned long int process_count = 0;

    char* file_name;

    int num_cpus;

    /************************  Read In File  *********************************/

    // Checks command line arguments for specific flags to set the file and the number of CPUs. 
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
        // Exit if file pointer returns NULL
        exit(1);
    }

    // Reads text until newline is encountered
    while (EOF != fscanf(fptr, "%[^\n]\n", line)) {
        insert_process_into_linked_list(&head, line);
        process_count++;
    }

    unsigned long int processes_remaining = process_count;

    /*****************************************************************************/



    /********************************    CPU Initialisation    *********************************/

    struct cpu *cpu_array;
    cpu_array = (struct cpu*)malloc(num_cpus * sizeof(struct cpu));
    for (int i=0; i<num_cpus; i++){
        cpu_array[i].cpu_id = i;
        cpu_array[i].processes_head = NULL;
        cpu_array[i].cpu_rem_exec_time = 0;
        cpu_array[i].running_process_ptr = NULL;
    }  

    /*******************************************************************************/


    /* Main Loop of The Program */
    while (processes_remaining){

        // DEBUG CODE
        // printf("%lu ", current_time);
        // print_CPU_process_list(cpu_array[0].processes_head);
        // printf("%lu ", current_time);
        // print_CPU_process_list(cpu_array[1].processes_head);
        // printf("\nCurrent_time:%d , CPU-0-REM-TIME:%d , CPU-1-REM-TIME:%d\n", current_time, cpu_array[0].cpu_rem_exec_time, cpu_array[1].cpu_rem_exec_time );


        
        /* Check if any process arrived at current time */
        if (has_process_arrived_at_current_time(head, current_time)) {
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
                add_process_to_cpu(get_ptr_to_process_equal_to_curr_time(head, current_time), num_cpus, &cpu_array);
                proc_rem++;
            }

            // For loop for multiple CPUs
            for (int i=0; i<num_cpus; i++){ 

                // If a process is already running in that CPU
                if (cpu_array[i].running_process_ptr != NULL) {  
                    // Check if the currently running process in the CPU is the same as the newly assigned process in the CPU 
                    if (cpu_array[i].running_process_ptr == set_cpu_running_process_ptr(&(cpu_array[i]))){
                        //If it is the same process being run, then no need to print it out
                    }
                    else {
                        // If the process running is a Child process
                        if (cpu_array[i].running_process_ptr->parallelisability == 'p' && num_cpus > 1){
                            printf("%lu,RUNNING,pid=%.1f,remaining_time=%lu,cpu=%d\n", current_time, cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
                        }
                        else{
                            printf("%lu,RUNNING,pid=%d,remaining_time=%lu,cpu=%d\n", current_time, (int)cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
                        }
                    }
                }
                
                // Else if no process is running in that CPU
                else {
                    set_cpu_running_process_ptr(&(cpu_array[i]));
                    if (cpu_array[i].running_process_ptr != NULL){
                        // If the process running is a Child process
                        if (cpu_array[i].running_process_ptr->parallelisability == 'p' && num_cpus > 1){
                            printf("%lu,RUNNING,pid=%.1f,remaining_time=%lu,cpu=%d\n", current_time, cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
                        }
                        else{
                            printf("%lu,RUNNING,pid=%d,remaining_time=%lu,cpu=%d\n", current_time, (int)cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
                        }
                    }    
                }
            } // END OF FOR LOOP         
        }


        // DEBUG CODE
        // for (int i=0; i<num_cpus; i++){
        //     printf("\n%lu CPU%d: ", current_time, cpu_array[i].cpu_id);
        //     print_CPU_process_list(cpu_array[i].processes_head);
        //     if (cpu_array[i].running_process_ptr != NULL){
        //     printf("\nRUNNING: %.1f\n", cpu_array[i].running_process_ptr->pid);
        //     }
        // }

         

/************************************************* DECREMENT SECTION **********************************************************/

        // If there is no more process for which the current time is equal to its arr_time, then below code will run
        for (int i=0; i<num_cpus; i++){ // For loop for multiple CPUs
            if (cpu_array[i].running_process_ptr != NULL) { // To check if there is any process allocated to the CPU or not
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
                // If a child process has finished, check if the other child processes have finished before decrementing proc_rem

                // Check if the running process that has finished is a Child process or not
                if (cpu_array[i].running_process_ptr->parallelisability == 'p' && num_cpus > 1){
                    //Remove the child process that is completed from it's parent's linked list
                    remove_child_process_from_parent(&cpu_array[i].running_process_ptr->parent->children_list_head, cpu_array[i].running_process_ptr->pid);
                    // Remove the child process that has completed running from cpu linked
                    remove_process_from_cpu(&cpu_array[i].processes_head, cpu_array[i].running_process_ptr->pid);


                    /***************************************************************************************************************************************************/
                    // Check if the other Child processes are finished or not, before printing out FINISHED for the parent process
                    if (cpu_array[i].running_process_ptr->parent->children_list_head == NULL){
                        // Do the usual things like printing as done below
                        proc_rem--;
                        processes_remaining--;
                        printf("%lu,FINISHED,pid=%d,proc_remaining=%lu\n", current_time, (int)cpu_array[i].running_process_ptr->parent->pid, proc_rem);

                        // Setting the Completed Time for a process when it has completed in the Main Linked List
                        get_ptr_to_process_equal_to_pid(head, cpu_array[i].running_process_ptr->parent->pid)->completed_time = current_time;

                        //Setting new processes to run in each CPU
                        for (int j=0; j<num_cpus; j++){
                            set_cpu_running_process_ptr(&(cpu_array[j]));
                        }
                        // break;
                        
                        for (int j=0; j<num_cpus; j++){
                            if( cpu_array[j].running_process_ptr != NULL){
                                // If the process running is a Child process
                                if (cpu_array[j].running_process_ptr->parallelisability == 'p' && num_cpus > 1){
                                    printf("%lu,RUNNING,pid=%.1f,remaining_time=%lu,cpu=%d\n", current_time, cpu_array[j].running_process_ptr->pid, cpu_array[j].running_process_ptr->rem_exec_time, cpu_array[j].cpu_id);
                                }
                                else{
                                    printf("%lu,RUNNING,pid=%d,remaining_time=%lu,cpu=%d\n", current_time, (int)cpu_array[j].running_process_ptr->pid, cpu_array[j].running_process_ptr->rem_exec_time, cpu_array[j].cpu_id);
                                }
                            }
                        }
                     

                        // If all process is completed then BREAK out from the for loop
                        if (!processes_remaining){
                            break;   
                        }
                        
                    }
                    

                    // set_cpu_running_process_ptr(&(cpu_array[i]));
                    // if( cpu_array[i].running_process_ptr != NULL){
                    //     // If the process running is a Child process
                    //     if (cpu_array[i].running_process_ptr->parallelisability == 'p' && num_cpus > 1){
                    //         printf("%lu,RUNNING,pid=%.1f,remaining_time=%lu,cpu=%d\n", current_time, cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
                    //     }
                    //     else{
                    //         printf("%lu,RUNNING,pid=%d,remaining_time=%lu,cpu=%d\n", current_time, (int)cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
                    //     }
                    // } 
                    // else {
                    //     break;
                    // }

                    
                    /***************************************************************************************************************************************************/
                    
                }


                // This below code gets executed if the running process that has completed is NOT a child process
                else {
                    proc_rem--;
                    processes_remaining--;
                    printf("%lu,FINISHED,pid=%d,proc_remaining=%lu\n", current_time, (int)cpu_array[i].running_process_ptr->pid, proc_rem);
                    // Setting the Completed Time for a process when it has completed in the Main Linked List
                    get_ptr_to_process_equal_to_pid(head, cpu_array[i].running_process_ptr->pid)->completed_time = current_time;
                    remove_process_from_cpu(&cpu_array[i].processes_head, cpu_array[i].running_process_ptr->pid); // Removes the process from the CPU's processes linked list

                    // CHECK AGAIN HERE TO SET THE PROCESS WITH THE SHORTEST EXECUTION TIME
                    //Traverse trough LL from head to find node with lowest rem_exec_time and set it to the run pointer
                    if (!processes_remaining){   /*If All process's rem_exec_time is equal to zero, i.e. if all processes are completed*/
                        break;   // If all process is completed then BREAK out from the while loop
                    }

                    else{
                        // Check the current CPU for any other processes that can be run
                        // IF yes, then set it as the run pointer of the current cpu, and PRINT it out
                        // If no, then don't print out anything of the current CPU, and move to the next CPU
                        set_cpu_running_process_ptr(&(cpu_array[i]));
                        if( cpu_array[i].running_process_ptr != NULL){
                            // If the process running is a Child process
                            if (cpu_array[i].running_process_ptr->parallelisability == 'p' && num_cpus > 1){
                                printf("%lu,RUNNING,pid=%.1f,remaining_time=%lu,cpu=%d\n", current_time, cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
                            }
                            else{
                                printf("%lu,RUNNING,pid=%d,remaining_time=%lu,cpu=%d\n", current_time, (int)cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
                            }
                        } 
                        // else {
                        //     break;
                        // }
                    }
                }
                
            }
        } // FOR LOOP ENDS

        // If all process is completed then BREAK out from the while loop (REDUNDANT)
        if(!processes_remaining) {
            break;                              
        }
/******************************************************************************************************************************/

    }  // MAIN LOOP ENDS

    printf("Turnaround time %d\n", calculate_turnaround_time(head, process_count));
    printf("Time overhead %.2f %.2f\n", calculate_max_time_overhead(head, process_count), calculate_avg_time_overhead(head, process_count));
    printf("Makespan %lu\n", current_time);
    // printf("Parallelisability: %c", head->parallelisability);

/******************************************************************************************************************************/
    
    fclose(fptr);

    return 0;
}