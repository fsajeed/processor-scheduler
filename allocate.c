#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "functions.c"

#define MAX_FILE_LINE_LENGTH 50            // Approximation for maximum length of a line consisting of 2 unsigned long int data types (arrival time and remaining execution time), 1 float data type (process id) and one 1 character data type (parallelisability) along with some whitespaces.



int main(int argc, char* argv[]) {

    struct Process* head = NULL;           // The head pointer to the main Linked List where all the processes will be read in first.

    unsigned long int current_time=0;      // Unsigned long int used due to the range [0, 2^32) of arrival times.

    char line[MAX_FILE_LINE_LENGTH];       // Will store each line representing an individual process's data from the file that is read in.

    unsigned long int proc_rem = 0;

    unsigned long int process_count = 0;    // Used for calculating statistics like Turnaround times and Time overheads.

    char* file_name;

    int num_cpus;

/**************************************************************  Read In File  *************************************************************************/

    // Checks command line arguments for specific flags to set the file to be read and the number of CPUs. 
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
        insert_process_data_into_linked_list(&head, line);
        // Increment process_count as each line from the file is inserted one by one
        process_count++;
    }

    /****************************************************************************************************************************************************/


    // This extra variable with same value as process_count will be used to control and break the main execution loop later.  
    unsigned long int processes_remaining = process_count;


    /***************************************************************   CPU Initialisation    ************************************************************/

    struct Cpu *cpu_array;
    cpu_array = (struct Cpu*)malloc(num_cpus * sizeof(struct Cpu));
    for (int i=0; i<num_cpus; i++){
        cpu_array[i].cpu_id = i;
        cpu_array[i].processes_head = NULL;
        cpu_array[i].cpu_rem_exec_time = 0;
        cpu_array[i].running_process_ptr = NULL;
    }  

    /******************************************************************************************************************************************************/


    /* Main Execution Loop of The Program */
    while (processes_remaining) {

        //Check if any process arrived at current time
        if (has_process_arrived_at_current_time(head, current_time)) {
            
            if (has_same_arrival_times(head)){
                // Sort the main linked list in increasing order of remaining execution times.
                sort_remaining_execution_times(head);
                // Insert all the processes arriving at the same time to relevant CPUs.
                struct Process* curr = head;
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
            // If more than one process is not arriving at any given time, then add process to CPU one by one.
            else {
                add_process_to_cpu(get_ptr_to_process_equal_to_curr_time(head, current_time), num_cpus, &cpu_array);
                proc_rem++;
            }

            // For loop for multiple CPUs
            for (int i=0; i<num_cpus; i++){ 

                // If a process is already running in that CPU
                if (cpu_array[i].running_process_ptr != NULL) {
                    // If the running process in the CPU is NOT equal to the newly assigned running process
                    if (cpu_array[i].running_process_ptr != set_cpu_running_process_ptr(&(cpu_array[i]))) {
                        // If the process running is a Child process (Parallelisability of a child process is 'p' because the parent process does not get added, rather copies of the parent gets added to the CPU as child processes except with different process ids).
                        if (cpu_array[i].running_process_ptr->parallelisability == 'p' && num_cpus > 1){
                            printf("%lu,RUNNING,pid=%.1f,remaining_time=%lu,cpu=%d\n", current_time, cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
                        }
                        else{
                            printf("%lu,RUNNING,pid=%d,remaining_time=%lu,cpu=%d\n", current_time, (int)cpu_array[i].running_process_ptr->pid, cpu_array[i].running_process_ptr->rem_exec_time, cpu_array[i].cpu_id);
                        }
                    }
                }
                
                // If no process is running in that CPU, set a new process to run in that CPU.
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
            }      
        }

/***************************************************************** DECREMENT SECTION ********************************************************************/

        // Loop to check for multiple CPUs
        for (int i=0; i<num_cpus; i++) {
            // Check if there is any process running on the CPU 
            if (cpu_array[i].running_process_ptr != NULL) { 
                // Decrement the running process's execution time
                (cpu_array[i].running_process_ptr->rem_exec_time)--; 
                // Decrement the CPU's remaining execution time that is running the process
                (cpu_array[i].cpu_rem_exec_time)--; 
            }        
        }

/*********************************************************************************************************************************************************/

        current_time++;

/************************************************************** CHECK IF PROCESS ENDED SECTION ************************************************************/

        for (int i=0; i<num_cpus; i++) {
            
            // Checks if the running process has finished executing
            if ((cpu_array[i].running_process_ptr != NULL) && (cpu_array[i].running_process_ptr->rem_exec_time == 0)){

                // Check if the running process that has finished is a Child process or not
                if (cpu_array[i].running_process_ptr->parallelisability == 'p' && num_cpus > 1){

                    //Remove the child process that is completed from it's parent's linked list
                    remove_child_process_from_parent(&cpu_array[i].running_process_ptr->parent->children_list_head, cpu_array[i].running_process_ptr->pid);
                    // Remove the child process that has completed running from cpu's linked list
                    remove_process_from_cpu(&cpu_array[i].processes_head, cpu_array[i].running_process_ptr->pid);

                 /***************************************************************************************************************************************************/

                    // Check if the other Child processes of a parent process are finished or not, before printing out parent process as Finished
                    if (cpu_array[i].running_process_ptr->parent->children_list_head == NULL){
                        proc_rem--;
                        processes_remaining--;
                        printf("%lu,FINISHED,pid=%d,proc_remaining=%lu\n", current_time, (int)cpu_array[i].running_process_ptr->parent->pid, proc_rem);
                        // Setting the Completed Time for a process when it has completed in the Main Linked List
                        get_ptr_to_process_equal_to_pid(head, cpu_array[i].running_process_ptr->parent->pid)->completed_time = current_time;

                        //Setting new processes to run in each CPU
                        for (int j=0; j<num_cpus; j++){
                            set_cpu_running_process_ptr(&(cpu_array[j]));
                        }
                        
                        // Printing out newly assigned running processes in each CPU if they exist.
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
                     

                        // If all processes are completed then BREAK out from the loop
                        if (!processes_remaining){
                            break;   
                        }
                        
                    }
 
                 /***************************************************************************************************************************************************/
                    
                }

                // This below code gets executed if the running process that has completed is NOT a child process.
                else {
                    //Check if there are other processes Finishing at the same time.
                    for (int j=0; j<num_cpus; j++){
                        if ((cpu_array[j].running_process_ptr != NULL) && (cpu_array[j].running_process_ptr->rem_exec_time == 0)){
                            proc_rem--;
                            processes_remaining--;
                            
                        }
                    }

                    //Printing out all the processes that has finished at the same time, and removing them from the CPU they were on.
                    for (int j=0; j<num_cpus; j++){
                        if ((cpu_array[j].running_process_ptr != NULL) && (cpu_array[j].running_process_ptr->rem_exec_time == 0)){
                            printf("%lu,FINISHED,pid=%d,proc_remaining=%lu\n", current_time, (int)cpu_array[j].running_process_ptr->pid, proc_rem);
                            get_ptr_to_process_equal_to_pid(head, cpu_array[j].running_process_ptr->pid)->completed_time = current_time;
                            remove_process_from_cpu(&cpu_array[j].processes_head, cpu_array[j].running_process_ptr->pid);
                        }
                    }

                    if (!processes_remaining){  
                        break;  
                    }

                    // Check if there are other processes to be run on each CPU, if there are, then assign them to be run on that CPU and print them out.
                    else{
                        for (int j=0; j<num_cpus; j++){
                            // If running process has completed on a CPU
                            if ((cpu_array[j].running_process_ptr != NULL) && (cpu_array[j].running_process_ptr->rem_exec_time == 0)){
                                // Then assign new processto run on that CPU
                                set_cpu_running_process_ptr(&(cpu_array[j]));
                                if( cpu_array[j].running_process_ptr != NULL){
                                    // If the process running is a Child process
                                    if (cpu_array[i].running_process_ptr->parallelisability == 'p' && num_cpus > 1){
                                        printf("%lu,RUNNING,pid=%.1f,remaining_time=%lu,cpu=%d\n", current_time, cpu_array[j].running_process_ptr->pid, cpu_array[j].running_process_ptr->rem_exec_time, cpu_array[j].cpu_id);
                                    }
                                    else{
                                        printf("%lu,RUNNING,pid=%d,remaining_time=%lu,cpu=%d\n", current_time, (int)cpu_array[j].running_process_ptr->pid, cpu_array[j].running_process_ptr->rem_exec_time, cpu_array[j].cpu_id);
                                    }
                                }
                            }
                        }
                    }
                }     
            }
        }
    }  // Main execution loop ends

    free(cpu_array);

/******************************************************************************************************************************/

    // Statistics Computation
    printf("Turnaround time %d\n", calculate_turnaround_time(head, process_count));
    printf("Time overhead %.2f %.2f\n", calculate_max_time_overhead(head, process_count), calculate_avg_time_overhead(head, process_count));
    printf("Makespan %lu\n", current_time);

/******************************************************************************************************************************/
    
    free_linked_list(head);

    fclose(fptr);

    return 0;
}
