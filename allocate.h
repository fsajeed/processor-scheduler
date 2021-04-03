#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

#define CPU_ARR_LENGTH 2

struct process {
    int arr_time;               // make unsigned long int
    int pid;                    // make unsigned long int
    int exec_time;
    char *parallelisability;    // need to conver to just character later
    int rem_exec_time;
    struct cpu *cpu_ptr;        // Should contain the reference/pointer to the CPU
    struct process *next;       // Pointer pointing to the next process stored in the Linked List

    // store CPU id too - -1 for no cpu
};

struct cpu {
    int cpu_id;
    struct process* processes[4]; // NEED to be an array of pointer to processes
    int cpu_rem_exec_time;
    struct process* running_process_ptr; // Pointer to the process that the CPU is currently running
};


//SHOULD ADD THE PROCESS TO THE CPU
// AND ALSO, ADD THE CPU TO THE PROCESS
void add_process_to_cpu(struct process* process, struct cpu **cpu_array){
    // Check which CPU's remaining execution time is lowest, and assign to that CPU
    // If tie between remainig times, then check CPU ids of tied ones
    // Assign to the one with smallest cpu_id

    int cpu_arr_length = CPU_ARR_LENGTH; // it can also be argv[x]
    int min = INT_MAX;
    // int cpu_time;
    struct cpu* min_ptr;
    for (int i=0; i<cpu_arr_length; i++){

        // cpu_time = calc_remaining_cpu_exec_time((*cpu_array)[i].cpu_id, cpu_array);

        if ((*cpu_array)[i].cpu_rem_exec_time < min){
            min = (*cpu_array)[i].cpu_rem_exec_time;
            min_ptr = &((*cpu_array)[i]); // Get the pointer to the CPU
        }
        else if ((*cpu_array)[i].cpu_rem_exec_time == min){ // RESOLVE TIES WITH SAME CPU REMAINING EXECUTION TIME (This case will not happen until the value for the first CPU is calculated)
            // CHECK POINTER OF PREVIOUS CPU i.e. using i-1
            // CHECK FOR CPU WITH LOWEST CPU ID
            if ((*cpu_array)[i].cpu_id > (*cpu_array)[i-1].cpu_id){
                //Put process in cpu with smaller cpu_id, i.e. i-1
                min_ptr = &((*cpu_array)[i-1]);
            }
            else{
                min_ptr = &((*cpu_array)[i]);
            }
        }
        else if ((*cpu_array)[i].cpu_rem_exec_time > min){
            // Do Nothing - because you do not add it to the cpu with higher cpu_time than the one with lower one
        }

    }

    min_ptr->cpu_rem_exec_time = (min_ptr->cpu_rem_exec_time) + (process->rem_exec_time); //Updating the allocated cpu's remaining execution time

    for (int j=0; j<4; j++){
        if ((min_ptr->processes)[j] == NULL){
            (min_ptr->processes)[j] = process; // Add the process to the cpu processes list
            break;
        }
    }

    process->cpu_ptr = min_ptr;     //Add the cpu pointer to the process
    
    return;
}

// Returns pointer to process with the smallest remaiing execution time, after breaking ties with pid
struct process* get_process_with_smallest_rem_time_accounting_for_duplicates(struct process* head, int current_time)   
{
    struct process* temp = head;
    struct process* min_ptr;
    // Declare a min variable and initialize
    // it with UINT_MAX value.
    // UINT_MAX is integer type and its value
    // is 32767 or greater.
    int min = INT_MAX;
  
    // Check loop while head not equal to NULL
    while (temp != NULL) {
        if (temp->rem_exec_time != 0) { // SKIP THE PROCESS THAT HAS FINISHED
            if (temp->arr_time == current_time){   // SKIP THE PROCESS THAT DID NOT ARRIVE AT THE CURRENT TIME
                if (min > (temp->rem_exec_time)){
                    min = temp->rem_exec_time;
                    min_ptr = temp;
                }
                //BREAK TIE USING PID WHEN min value is same as the current process's rem_exec_time
                else if (min == temp->rem_exec_time){
                    if(min_ptr->pid > temp->pid) {
                        min_ptr = temp;
                    }
                    else {
                        // DO NOTHING - min_ptr will not be changed as it already has the lowest pid among the two
                    }
                }
            }
        }
        temp = temp->next;
    }
    return min_ptr;
}


// Returns pointer to process with the smallest remaiing execution time, after breaking ties with pid
struct process* get_process_with_smallest_rem_time_breaking_ties(struct process* head)   
{
    struct process* temp = head;
    struct process* min_ptr;
    // Declare a min variable and initialize
    // it with UINT_MAX value.
    // UINT_MAX is integer type and its value
    // is 32767 or greater.
    int min = INT_MAX;
  
    // Check loop while head not equal to NULL
    while (temp != NULL) {
        if (temp->rem_exec_time != 0) { // SKIP THE PROCESS THAT HAS FINISHED
            if (min > (temp->rem_exec_time)){
                min = temp->rem_exec_time;
                min_ptr = temp;
            }
            //BREAK TIE USING PID WHEN min value is same as the current process's rem_exec_time
            else if (min == temp->rem_exec_time){
                if(min_ptr->pid > temp->pid) {
                    min_ptr = temp;
                }
                else {
                    // DO NOTHING - min_ptr will not be changed as it already has the lowest pid among the two
                }
            }
        }
        temp = temp->next;
    }
    return min_ptr;
}



// Function to check if there are processes with same arrival times in the Linked List
bool has_same_arrival_times(struct process* head)
{
    while (head->next != NULL) {
  
        // Starting from the next node
        struct process* ptr = head->next;
        while (ptr != NULL) {
  
            // If some duplicate node is found
            if (head->arr_time == ptr->arr_time) {
                return true;
                // break;
            }
            ptr = ptr->next;
        }
  
        head = head->next;
    }

    return false;
}

bool search(struct process* head, int x)
{
    struct process* current = head;  // Initialize current
    while (current != NULL)
    {
        if (current->arr_time == x){
            return true;
        }
        current = current->next;
    }
    return false;
}

struct process* get_pointer_to_process_equal_to_curr_time(struct process* head, int time)
{
    struct process* current = head;  // Initialize current
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

bool is_all_process_completed(struct process* head){

    struct process* curr = head;

    while (curr->rem_exec_time == 0){  // While loop checking if all rem_exec_time values are 0
        curr = curr->next;

        if (curr == NULL){
        return true;
        }
    }
    return false;

}

// Function to print the second
// smallest element
struct process* get_shortest_rem_exec_time_process(struct process* head)
{
    int first = 0, second = INT_MAX; // Here first is equal to 0 is because, we know the first lowest rem_exec_time is 0 as one process already finished
  
    struct process* temp = head;
    struct process* ptr;
    while (temp != NULL) {
  
        if (temp->rem_exec_time < first) {
            second = first;
            first = temp->rem_exec_time;
        }
  
        // If current node's data is in between
        // first and second then update second
        else if (temp->rem_exec_time < second && temp->rem_exec_time != first){
            second = temp->rem_exec_time;
            ptr = temp;

        }
  
        temp = temp->next;
    }
  
    if (second == INT_MAX){
        // cout << "There is no second smallest element\n";
        return NULL;
    }
    else {
        return ptr;
    }
}

// Function that returns smallest element
// from the linked list.
struct process* smallestArrivalTime(struct process* head)       // NEED TO SORT OUT TIES!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
{
    struct process* temp = head;
    struct process* min_ptr;
    // Declare a min variable and initialize
    // it with UINT_MAX value.
    // UINT_MAX is integer type and its value
    // is 32767 or greater.
    int min = INT_MAX;
  
    // Check loop while head not equal to NULL
    while (temp != NULL) {
        // If min is greater than temp->arr_time then
        // assign value of temp->arr_time to min
        // otherwise node point to next node.
        if (min > (temp->arr_time)) {
            min = temp->arr_time;
            min_ptr = temp;
        }
        temp = temp->next;
    }
    return min_ptr;
}

void printList(struct process* p)
{
    while (p != NULL) {
        printf("\n%d\n", p->arr_time);
        p = p->next;
    }
}

    // struct process* find_lowest_arr_time(struct process* head_ptr) {

    //     struct process* temp = head_ptr;
    //     struct process* lowest;

    //     while (temp->next != NULL){

            
            
    //         if ((temp->arr_time) > (temp->next->arr_time)){
    //             lowest = temp->next;
    //         }
    //         // else if ((temp->arr_time) == (temp->next->arr_time)){
    //         //     lowest = lowest_pid();
    //         // }
    //         if ((temp->arr_time) < (temp->next->arr_time)){
    //             lowest = temp;
    //         }
    //         temp = temp->next; //increment

    //         // if (temp->next == NULL) {
    //         //     return lowest;
    //         // }
    //     }
    //     return lowest;

    // }


    // int calc_remaining_cpu_exec_time(int cpu_id, struct cpu **cpu_array){

//     int cpu_arr_length = sizeof(*cpu_array)/sizeof(struct cpu);
//     int cpu_rem_exec_time;
//     for (int i=0; i<cpu_arr_length; i++){
//         if (cpu_id == (*cpu_array)[i].cpu_id){  // If cpu with this id is found

//             if ((*cpu_array)[i].cpu_id==-1){
                
//                 int processes_arr_length = sizeof((*cpu_array)[i].processes)/sizeof(struct process*);
//                 for (int j=0; j<processes_arr_length; j++){ // for every process pointer in processes array
//                     if((*cpu_array)[i].processes[j] == NULL){    //if process pointer is equal to NULL, i.e. "if no process is allocated yet" 
//                         break; // break to assign cpu_rem_exec_time=0
//                     } 
//                 }
                
//                 cpu_rem_exec_time=0;// if cpu_id == -1, then cpu_rem_exec_time=0
//             }
//             else {// else, loop through each process pointer in the cpu's processes array and calculate total rem_exec_time of cpu
//                 int cpu_rem_exec_time=0;

//                 if ((*cpu_array)[i]) // CHECKS IF THERE ARE ANY PROCESSES IN THE PROCESS LIST OF THE CPU
//                 int processes_arr_length = sizeof((*cpu_array)[i].processes)/sizeof(struct process*);
//                 // printf("%d\n", processes_arr_length);
//                 for (int j=0; j<processes_arr_length; j++){ // for every process pointer in processes array
//                 cpu_rem_exec_time = cpu_rem_exec_time + (*cpu_array)[i].processes[j]->rem_exec_time; // access the process's rem_exec_time and add to cpu's rem_exec_time
//                 }
//             }
//             return cpu_rem_exec_time;
//         }
//         else{
//             printf("\nNO cpu with cpu_id %d\n", cpu_id);
//         }
//     }
    
//     return 0; // JUST FOR THE SAKE OF STOPPING WARNINGS
// }

// //SHOULD ADD THE PROCESS TO THE CPU
// // AND ALSO, ADD THE CPU TO THE PROCESS
// void add_process_to_cpu(struct process* process, struct cpu **cpu_array) {
//     // Check which CPU's remaining execution time is lowest, and assign to that CPU
//     // If tie between remainig times, then check CPU ids of tied ones
//     // Assign to the one with smallest cpu_id

//     int cpu_arr_length = sizeof(*cpu_array)/sizeof(struct cpu); // it can also be argv[x]
//     int min = INT_MAX;
//     int cpu_time;
//     struct cpu* min_ptr;
//     for (int i=0; i<cpu_arr_length; i++){

//         cpu_time = calc_remaining_cpu_exec_time((*cpu_array)[i].cpu_id, cpu_array);

//         if (cpu_time < min){
//             min = cpu_time;
//             min_ptr = &((*cpu_array)[i]); // Get the pointer to the CPU
//         }
//         else if (cpu_time == min){ // RESOLVE TIES WITH SAME CPU REMAINING EXECUTION TIME (This case will not happen until the value for the first CPU is calculated)
//             // CHECK POINTER OF PREVIOUS CPU i.e. using i-1
//             // CHECK FOR CPU WITH LOWEST CPU ID
//             if ((*cpu_array)[i].cpu_id > (*cpu_array)[i-1].cpu_id){
//                 //Put process in cpu with smaller cpu_id, i.e. i-1
//                 min_ptr = &((*cpu_array)[i-1]);
//             }
//             else{
//                 min_ptr = &((*cpu_array)[i]);
//             }
//         }
//         else if (cpu_time > min){
//             // Do Nothing - because you do not add it to the cpu with higher cpu_time than the one with lower one
//         }

//     }

//     process->cpu_ptr = min_ptr;     //Add the cpu pointer to the process

//     int proc_arr_len = sizeof(min_ptr->processes)/sizeof(struct process*);
//     // struct processes** processes_new = (struct process**)realloc(min_ptr->processes, sizeof(struct process*)*(proc_arr_len + 1));
//     min_ptr->processes = realloc(min_ptr->processes, sizeof(struct process*)*(proc_arr_len + 1));
//     min_ptr->processes[proc_arr_len] = process;         // Add the process pointer to the processes array in the CPU
    
//     return;
// }

//void add_cpu_to_process(struct cpu* cpu, struct process *process) {
    // Check which CPU's remaining execution time is lowest, and assign to that CPU
    // If tie between remainig times, then check CPU ids of tied ones
    // Assign to the one with smallest cpu_id


//}

// int calc_remaining_cpu_exec_time(int cpu_id, struct cpu **cpu_array){

//     int cpu_arr_length = sizeof(*cpu_array)/sizeof(struct cpu);
//     int cpu_rem_exec_time;
//     for (int i=0; i<cpu_arr_length; i++){
//         if (cpu_id == (*cpu_array)[i].cpu_id){  // If cpu with this id is found

//             if ((*cpu_array)[i].cpu_id==-1){
                
//                 int processes_arr_length = sizeof((*cpu_array)[i].processes)/sizeof(struct process*);
//                 for (int j=0; j<processes_arr_length; j++){ // for every process pointer in processes array
//                     if((*cpu_array)[i].processes[j] == NULL){    //if process pointer is equal to NULL, i.e. "if no process is allocated yet" 
//                         break; // break to assign cpu_rem_exec_time=0
//                     } 
//                 }
                
//                 cpu_rem_exec_time=0;// if cpu_id == -1, then cpu_rem_exec_time=0
//             }
//             else {// else, loop through each process pointer in the cpu's processes array and calculate total rem_exec_time of cpu
//                 int cpu_rem_exec_time=0;

//                 if ((*cpu_array)[i]) // CHECKS IF THERE ARE ANY PROCESSES IN THE PROCESS LIST OF THE CPU
//                 int processes_arr_length = sizeof((*cpu_array)[i].processes)/sizeof(struct process*);
//                 // printf("%d\n", processes_arr_length);
//                 for (int j=0; j<processes_arr_length; j++){ // for every process pointer in processes array
//                 cpu_rem_exec_time = cpu_rem_exec_time + (*cpu_array)[i].processes[j]->rem_exec_time; // access the process's rem_exec_time and add to cpu's rem_exec_time
//                 }
//             }
//             return cpu_rem_exec_time;
//         }
//         else{
//             printf("\nNO cpu with cpu_id %d\n", cpu_id);
//         }
//     }
    
//     return 0; // JUST FOR THE SAKE OF STOPPING WARNINGS
// }



// // Function that returns a linked list of processes with same arrival times
// void get_arrival_times_list(struct process* head, int time, struct process** list_head)
// {
//     // int count = 0;
//     (*list_head) = NULL;
//     struct process* temp;
  
//     while (head->next != NULL) {
  
//         // Starting from the next node
//         struct process* ptr = head->next;
//         while (ptr != NULL) {
  
//             // If some duplicate arrival times are found
//             if (head->arr_time == ptr->arr_time) {

//                 if ((*list_head) == NULL){
//                     (*list_head) = head; 
//                     (*list_head)->next = NULL;
//                 }

//                 else if ((*list_head) != NULL){
//                     temp = (*list_head);
//                     while (temp->next != NULL) {
//                         temp = temp->next;
//                     }
//                     temp->next = head;
//                 }

//                 break;
//             }

//             ptr = ptr->next;
//         }

//         head = head->next;
//     }

//     // return count;
// }


// // Function that returns a linked list of processes with same arrival times
// struct process* get_arrival_times_list(struct process* head, int time)
// {
//     struct process* list_head = NULL;
//     struct process* temp;
  
//     while (head != NULL) {
    
//         // If some duplicate arrival times are found
//         if (head->arr_time == time) {

//             if (list_head == NULL){
//                 list_head = head; 
//                 list_head->next = NULL;
//             }

//             else if (list_head != NULL){
//                 temp = list_head;
//                 while (temp->next != NULL) {
//                     temp = temp->next;
//                 }
//                 temp->next = head;
//                 temp->next->next = NULL; // Making sure the last node do not point to anything
//             }
//         }
//         head = head->next;
//     }
//     return list_head;
// }