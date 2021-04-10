#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>

/*****************************************************************        Structs         **************************************************************************************/

struct Process {
    unsigned long int arr_time;                 // 'unsigned long int' data type used to cover the arrival time range of [0,2^32).
    float pid;                                  // 'float' data type used to cover the process id range of [0,2^32) and also to use as process ids for child processes/subprocesses.
    unsigned long int exec_time;                // 'unsigned long int' data type used to cover the execution time range of [1,2^32).
    char parallelisability;
    unsigned long int rem_exec_time;
    unsigned long int completed_time;
    struct Process* children_list_head;         // Linked List containing all the subprocesses from the process.
    struct Process* parent;                     // Used by child processes when they are initialised, this pointer variable holds reference to the parent process of a child process. Parent processes have this variable set to NULL.
    struct Process *next;                  
};

struct Cpu {
    int cpu_id;
    struct Process* processes_head;             // Linked List containing the queue of processes to be run by the CPU.
    unsigned long int cpu_rem_exec_time;        // Stores the remaining execution time of the CPU.
    struct Process* running_process_ptr;        // Pointer to the process that the CPU is currently running.
};

/*********************************************************************************************************************************************************************************/



/*****************************************************************        Functions         **************************************************************************************/

/*
Function takes a reference (pointer to pointer) to the head of the
main linked list that will contain all the processes, and a string 
of process data. It processes the string data to store it in a newly 
created node in the linked list. 
*/
void insert_process_data_into_linked_list(struct Process** head_ptr, char* process_data) {

   struct Process* new_node = (struct Process*)malloc(sizeof(struct Process));
   struct Process* temp;
   
   // Extract first token
   char * token = strtok(process_data, " ");
   int count = 0;
   // Loop through the string to extract all other tokens
   while( token != NULL ) {

    /* Inserting data from each line of file into Linked List as Nodes (Processes) */
      if (count == 0) {
          new_node->arr_time = atoi(token);
      }
      else if (count == 1){
          new_node->pid = atoi(token);
      }
      else if (count == 2){
          new_node->exec_time = atoi(token);
      }
      else if (count == 3){
          new_node->parallelisability = *token;
      }        
    /********************************************************************************/
      token = strtok(NULL, " ");
      count++;
   }

   new_node->rem_exec_time = new_node->exec_time;     // At first, 'remaining execution time' of a process is equal to the 'execution time'.                     
   new_node->next = NULL; 
   new_node->children_list_head = NULL;
   new_node->parent = NULL;


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


/*
Function takes pointer to a CPU as parameter and returns the
best process to run, i.e. checking for the process with lowest 
remaining execution time, and breaking ties to run process with 
the lowest process id when remaining times processes are the same. 
Returns NULL if no process is in the process queue(linked list)
of the CPU.
*/
struct Process* set_cpu_running_process_ptr(struct Cpu* cpu_ptr)
{
    struct Process* temp =  cpu_ptr->processes_head;
    struct Process* min_ptr = NULL;

    unsigned long int min = ULONG_MAX;

    while (temp != NULL) {
        if (min > (temp->rem_exec_time)){
            min = temp->rem_exec_time;
            min_ptr = temp;
        }
        //Break tie using pid when min value is same as the current process's rem_exec_time
        else if (min == temp->rem_exec_time){
            if(min_ptr->pid > temp->pid) {
                min_ptr = temp;
            }
        }
    temp = temp->next;
    }

    cpu_ptr->running_process_ptr = min_ptr;

    return min_ptr;
}


/* 
Given a reference (pointer to pointer) to the head of the
main linked list and a process id, deletes the process node 
with the process id from the processes Linked List in a CPU 
Attribution: This function was taken from 
https://www.geeksforgeeks.org/linked-list-set-3-deleting-node/
and modified.
*/
void remove_process_from_cpu(struct Process** head_ref, float process_id)
{
    // Store head node
    struct Process *temp = *head_ref, *prev;
 
    // If head node itself holds the process id to be deleted
    if (temp != NULL && temp->pid == process_id) {
        *head_ref = temp->next; // Changed head
        free(temp); // free old head
        return;
    }
 
    // Search for the pid to be deleted, keep track of the
    // previous node as 'prev->next' needs to be changed
    while (temp != NULL && temp->pid != process_id) {
        prev = temp;
        temp = temp->next;
    }
 
    // If pid was not present in linked list
    if (temp == NULL){
        return;
    }

    // Unlink the node from linked list
    prev->next = temp->next;
 
    free(temp); // Free memory
}


/* 
Given a reference (pointer to pointer) to the head of the
Parent process's linked list of child processes and a process id, deletes 
the child process node with the process id from the Linked List. 
Attribution: This function was taken from 
https://www.geeksforgeeks.org/linked-list-set-3-deleting-node/
and modified.
*/
void remove_child_process_from_parent(struct Process** children_list_head_ref, float process_id)
{
    // Store head node
    struct Process *temp = *children_list_head_ref, *prev;
 
    // If head node itself holds the process id to be deleted
    if (temp != NULL && temp->pid == process_id) {
        *children_list_head_ref = temp->next; // Changed head
        free(temp); // free old head
        return;
    }
 
    // Search for the pid to be deleted, keep track of the
    // previous node as 'prev->next' needs to be changed
    while (temp != NULL && temp->pid != process_id) {
        prev = temp;
        temp = temp->next;
    }
 
    // If pid was not present in linked list
    if (temp == NULL){
        return;
    }

    // Unlink the node from linked list
    prev->next = temp->next;
 
    free(temp); // Free memory

}


/*
Function takes a pointer to a process, number of CPUs and the cpu array as parameters.
It finds the CPU with the lowest cpu remaining execution time to be allocated to the given process.
If there are ties between cpu remaining execution times, it breaks ties by CPU ids.
Once CPU is found, it updates the CPU's remaining execution time and adds the process to the linked list (or queue)
of the CPU it is allocated to.
If the process to be added is parallellisable, the required number of child process nodes are created, giving each an unique id,
and added to the CPU linked list as well as to the parent process's child process linked list.
*/
void add_process_to_cpu(struct Process* process, int num_cpus, struct Cpu **cpu_array){
    // Check which CPU's remaining execution time is lowest, and assign to that CPU
    // If tie between remainig times, then check CPU ids of tied ones
    // Assign to the one with smallest cpu_id
    
    unsigned long int min = (*cpu_array)[0].cpu_rem_exec_time;

    struct Cpu* min_cpu_ptr = &((*cpu_array)[0]);

    for (int i=1; i<num_cpus; i++){

        if ((*cpu_array)[i].cpu_rem_exec_time < min){
            min = (*cpu_array)[i].cpu_rem_exec_time;
            min_cpu_ptr = &((*cpu_array)[i]); // Get the pointer to the CPU
        }
        // Resolve ties for same CPU remaining execution times
        else if ((*cpu_array)[i].cpu_rem_exec_time == min){
            if ((*cpu_array)[i].cpu_id > min_cpu_ptr->cpu_id){
                //Do Nothing - the min_cpu_ptr will be the previous min_cpu_ptr
            }
            else {
                min_cpu_ptr = &((*cpu_array)[i]);
            }
        }
    }


    if (process->parallelisability == 'p' && num_cpus > 1) {
        // To extend the time on each CPU
        for (int i=0; i<num_cpus; i++){
            (*cpu_array)[i].cpu_rem_exec_time += ceil((double)process->rem_exec_time / (double)num_cpus) + 1;
        }
    }
    else {
        //Updating the allocated cpu's remaining execution time
        min_cpu_ptr->cpu_rem_exec_time += process->rem_exec_time; 
    }




    if (process->parallelisability == 'p' && num_cpus > 1) {
        // Make as many nodes as there are CPUs for child processes
        for (int i=0; i<num_cpus; i++){
            struct Process* new_node = (struct Process*)malloc(sizeof(struct Process));
            struct Process* temp;

            // Create the child process's id before inserting it into the new node as the process id
            char* cid = (char*)malloc(sizeof(char) * 10);
            sprintf(cid, "%d.%d", (int)process->pid, i);
            float child_id = atof(cid);
            
            new_node->arr_time = process->arr_time;                     
            new_node->pid = child_id;
            new_node->children_list_head = process->children_list_head;
            new_node->parent = process;
            new_node->exec_time = ceil((double)process->rem_exec_time / (double)num_cpus) + 1;
            new_node->parallelisability = process->parallelisability;
            new_node->rem_exec_time = ceil((double)process->rem_exec_time / (double)num_cpus) + 1;
            new_node->completed_time = process->completed_time;
            new_node->next = NULL;
            

            /* Adding to the CPU processes list */
            if ((*cpu_array)[i].processes_head == NULL){
                (*cpu_array)[i].processes_head = new_node; 
            }
            else {
                temp = (*cpu_array)[i].processes_head;
                while (temp->next != NULL) {
                    temp = temp->next;
                }
                temp->next = new_node;
            }
            /***************************************/

            // Another node created to be inserted into the parent process's children linked list
            struct Process* new_node2 = (struct Process*)malloc(sizeof(struct Process));
            
            new_node2->arr_time = process->arr_time;                    
            new_node2->pid = child_id;
            new_node2->children_list_head = process->children_list_head;
            new_node2->parent = process;                                 
            new_node2->exec_time = ceil((double)process->rem_exec_time / (double)num_cpus) + 1;
            new_node2->parallelisability = process->parallelisability;
            new_node2->rem_exec_time = ceil((double)process->rem_exec_time / (double)num_cpus) + 1;
            new_node2->completed_time = process->completed_time;
            new_node2->next = NULL;

            /* Adding to the Parent process's child processes linked list */
            if (process->children_list_head == NULL){ 
                process->children_list_head = new_node2; 
            }
            else {
                temp = process->children_list_head;
                while (temp->next != NULL) {
                    temp = temp->next;
                }
                temp->next = new_node2;
            }
            /**********************************************************/

        }
    }

    // If the process to be added is not parallelisable
    else {

        struct Process* new_node = (struct Process*)malloc(sizeof(struct Process));
        struct Process* temp;

        new_node->arr_time = process->arr_time;                     
        new_node->pid = process->pid;
        new_node->exec_time = process->exec_time;
        new_node->parallelisability = process->parallelisability;
        new_node->rem_exec_time = process->rem_exec_time;
        new_node->completed_time = process->completed_time;
        new_node->next = NULL;

        // For the first process to be inserted into the Linked List.
        if (min_cpu_ptr->processes_head == NULL){        
            min_cpu_ptr->processes_head = new_node; 
        }
        else {
            temp = min_cpu_ptr->processes_head;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = new_node;
        }
    }
    return;
}


/* 
Function checks if there are processes with same arrival times in the Linked List.
Returns true if there are, and false otherwise. 
*/
bool has_same_arrival_times(struct Process* head)
{
    while (head->next != NULL) {
        // Starting from the next node
        struct Process* ptr = head->next;
        while (ptr != NULL) {
            // If some node with same arrival time is found
            if (head->arr_time == ptr->arr_time) {
                return true;
            }
            ptr = ptr->next;
        }
        head = head->next;
    }
    return false;
}

/* 
Function takes a time value and a linked list head pointer as parameters, 
and returns true if any arrival time of processes matches with the given
time value, returns false if no match is found. 
*/
bool has_process_arrived_at_current_time(struct Process* head, unsigned long int time)
{
    struct Process* current = head;  // Initialize current
    while (current != NULL)
    {
        if (current->arr_time == time){
            return true;
        }
        current = current->next;
    }
    return false;
}

/* 
Function takes a time value and a linked list head pointer as parameters, 
and returns the pointer to the process who's arrival time matches with the given
time value, returns NULL if no match is found. 
*/
struct Process* get_ptr_to_process_equal_to_curr_time(struct Process* head, unsigned long int time)
{
    struct Process* current = head;  // Initialize current
    while (current != NULL)
    {
        if (current->arr_time == time){
            // return current;
            break;
        }
        current = current->next;
    }
    return current;
}

/* 
Function takes process_id and a linked list head pointer as parameters, 
and returns the pointer to the process if it is found in the linked list,
otherwise returns NULL. 
*/
struct Process* get_ptr_to_process_equal_to_pid(struct Process* head, float process_id)
{
    struct Process* current = head;  // Initialize current
    while (current != NULL)
    {
        if (current->pid == process_id){
            // return current;
            break;
        }
        current = current->next;
    }
    return current;
}




/* 
Function to swap data of two Process nodes a and b.
Used when the sort_remaining_execution_times() function is called.
*/
void swap(struct Process *a, struct Process *b)
{
    float temp1 = a->pid;
    a->pid = b->pid;
    b->pid = temp1;

    unsigned long int temp2 = a->arr_time;
    a->arr_time = b->arr_time;
    b->arr_time = temp2;

    unsigned long int temp3 = a->exec_time;
    a->exec_time = b->exec_time;
    b->exec_time = temp3;

    unsigned long int temp4 = a->rem_exec_time;
    a->rem_exec_time = b->rem_exec_time;
    b->rem_exec_time = temp4;

    char temp5 = a->parallelisability;
    a->parallelisability = b->parallelisability;
    b->parallelisability = temp5;
}


/* 
Bubble sort the given linked list 
Attribution: This function was taken from 
https://www.geeksforgeeks.org/c-program-bubble-sort-linked-list/
and modified slightly.
*/
void sort_remaining_execution_times(struct Process *head)
{
    int swapped;
    struct Process *ptr1;
    struct Process *lptr = NULL;
  
    /* Checking for empty list */
    if (head == NULL)
        return;
  
    do
    {
        swapped = 0;
        ptr1 = head;
  
        while (ptr1->next != lptr)
        {
            if (ptr1->rem_exec_time > ptr1->next->rem_exec_time)
            { 
                swap(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    }
    while (swapped);
}

int calculate_turnaround_time(struct Process* head, int process_count) {
    double total = 0;
    double turnaround_time;
    struct Process* temp = head;
    while(temp != NULL){
        total += ((temp->completed_time) - (temp->arr_time));
        temp = temp->next;
    }
    turnaround_time = ceil(total/process_count);
    return (int)turnaround_time;
}

double calculate_max_time_overhead(struct Process* head, int process_count) {
    double max = INT_MIN;
    double overhead;
    struct Process* temp = head;
    while(temp != NULL){
        overhead = (double)((temp->completed_time) - (temp->arr_time)) / temp->exec_time;
        if (overhead > max){
            max = overhead;
        }
        temp = temp->next;
    }
    return max;
}

double calculate_avg_time_overhead(struct Process* head, int process_count) {
    double total_overhead = 0;
    double avg;
    struct Process* temp = head;
    while(temp != NULL){
        total_overhead += (double)((temp->completed_time) - (temp->arr_time)) / temp->exec_time;
        temp = temp->next;
    }
    avg = total_overhead/process_count;
    return avg;
}

/**********************************************************************************************************************************************************************************************/



/*************************   DEBUG FUNCTIONS   *****************************/
void print_pids_in_list(struct Process* p)
{
    while (p != NULL) {
        printf(" %d ", (int)p->pid);
        p = p->next;
    }
}

void print_CPU_process_list(struct Process* processes_head)
{
    struct Process* p = processes_head;
    while (p != NULL) {
        printf(" %.1f ", p->pid);
        p = p->next;
    }
    // printf("\n");
}

void print_CPU_ids(struct Cpu* cpu_array, int num_cpus)
{
    for (int i=0; i<num_cpus; i++) {
        printf(" %d ", cpu_array[i].cpu_id);
    }
}

/*********************************************************************************/