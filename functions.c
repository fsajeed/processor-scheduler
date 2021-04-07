#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <math.h>

struct process {
    unsigned long int arr_time;               // make unsigned long int
    unsigned long int pid;                    // make unsigned long int
    unsigned long int exec_time;
    char *parallelisability;    // need to conver to just character later
    unsigned long int rem_exec_time;
    unsigned long int completed_time;
    struct cpu *cpu_ptr;        // Should contain the reference/pointer to the CPU
    struct process *next;       // Pointer pointing to the next process stored in the Linked List

    // store CPU id too - -1 for no cpu
};

struct cpu {
    int cpu_id;
    // struct process* processes[PROCESSES_ARR_LENGTH]; // NEED to be an array of pointer to processes
    struct process_address_container* process_address_container_head;
    unsigned long int cpu_rem_exec_time;
    struct process* running_process_ptr; // Pointer to the process that the CPU is currently running
};

// The struct used for constructing the Linked List in the CPU struct
struct process_address_container {
    struct process* process_ptr;
    struct process_address_container* next;
};

int calculate_turnaround_time(struct process* head, int process_count){
    double total = 0;
    double turnaround_time;
    struct process* temp = head;
    while(temp != NULL){
        total += ((temp->completed_time) - (temp->arr_time));
        temp = temp->next;
    }
    turnaround_time = ceil(total/process_count);
    return (int)turnaround_time;
}

double calculate_max_time_overhead(struct process* head, int process_count){
    double max = INT_MIN;
    double overhead;
    struct process* temp = head;
    while(temp != NULL){
        overhead = (double)((temp->completed_time) - (temp->arr_time)) / temp->exec_time;
        if (overhead > max){
            max = overhead;
        }
        temp = temp->next;
    }
    return max;
}

double calculate_avg_time_overhead(struct process* head, int process_count){
    double total_overhead = 0;
    double avg;
    struct process* temp = head;
    while(temp != NULL){
        total_overhead += (double)((temp->completed_time) - (temp->arr_time)) / temp->exec_time;
        temp = temp->next;
    }
    avg = total_overhead/process_count;
    return avg;
}

struct process* set_cpu_running_process_ptr(struct cpu* cpu_ptr)
{
    struct process_address_container* temp =  cpu_ptr->process_address_container_head;
    struct process* min_ptr = NULL;

    unsigned long min = ULONG_MAX;
    // Check loop while head not equal to NULL
    while (temp != NULL) {
        if (temp->process_ptr->rem_exec_time != 0 ) { // SKIP THE PROCESS THAT HAS FINISHED
            if (min > (temp->process_ptr->rem_exec_time)){
                min = temp->process_ptr->rem_exec_time;
                min_ptr = temp->process_ptr;
            }
            //BREAK TIE USING PID WHEN min value is same as the current process's rem_exec_time
            else if (min == temp->process_ptr->rem_exec_time){
                if(min_ptr->pid > temp->process_ptr->pid) {
                    min_ptr = temp->process_ptr;
                }
                else {
                    // DO NOTHING - min_ptr will not be changed as it already has the lowest pid among the two
                }
            }
        }
        temp = temp->next;
    }
    // return min_ptr;
    cpu_ptr->running_process_ptr = min_ptr;

    return min_ptr;
}


// SHOULD ADD THE PROCESS TO THE CPU
// AND ALSO, ADD THE CPU TO THE PROCESS
void add_process_to_cpu(struct process* process, int num_cpus, struct cpu **cpu_array){
    // Check which CPU's remaining execution time is lowest, and assign to that CPU
    // If tie between remainig times, then check CPU ids of tied ones
    // Assign to the one with smallest cpu_id
    
    unsigned long int min = ULONG_MAX;
    // int cpu_time;
    struct cpu* min_ptr = NULL;

    for (int i=0; i<num_cpus; i++){

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


    // for (int j=0; j<PROCESSES_ARR_LENGTH; j++){
    //     if ((min_ptr->processes)[j] == NULL){
    //         (min_ptr->processes)[j] = process; // Add the process to the cpu processes list
    //         break;
    //     }
    // }

    struct process_address_container* new_node = (struct process_address_container*)malloc(sizeof(struct process_address_container));
    struct process_address_container* temp;

    new_node->process_ptr = process; // Add the process to the cpu processes list
    new_node->next = NULL;

    
    if (min_ptr->process_address_container_head == NULL){        // For the first address  ontainer to be inserted into the Linked List
        min_ptr->process_address_container_head = new_node; 
    }
    else {
        temp = min_ptr->process_address_container_head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_node;
    }
    // min_ptr->running_process_ptr = process;

    process->cpu_ptr = min_ptr;     //Add the cpu pointer to the process
    
    return;
}


// Returns pointer to process with the smallest remaiing execution time, after breaking ties with pid
struct process* get_process_with_smallest_rem_time_accounting_for_duplicates(struct process* head, int current_time)   
{
    struct process* temp = head;
    struct process* min_ptr = NULL;
    // Declare a min variable and initialize
    // it with UINT_MAX value.
    // UINT_MAX is integer type and its value
    // is 32767 or greater.
    unsigned long int min = ULONG_MAX;
  
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


// Returns pointer to process with the smallest remaining execution time, after breaking ties with pid
struct process* get_process_with_smallest_rem_time_breaking_ties(struct process* head)   
{
    struct process* temp = head;
    struct process* min_ptr = NULL;
    // Declare a min variable and initialize
    // it with UINT_MAX value.
    // UINT_MAX is integer type and its value
    // is 32767 or greater.
    unsigned long int min = ULONG_MAX;
  
    // Check loop while head not equal to NULL
    while (temp != NULL) {
        // SKIP THE PROCESS THAT HAS FINISHED
        if (temp->rem_exec_time != 0) { 
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

bool search(struct process* head, unsigned long int x)
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

struct process* get_pointer_to_process_equal_to_curr_time(struct process* head, unsigned long int time)
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
    unsigned long int first = 0, second = ULONG_MAX; // Here first is equal to 0 is because, we know the first lowest rem_exec_time is 0 as one process already finished
  
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
  
    if (second == ULONG_MAX){
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
    unsigned long int min = ULONG_MAX;
  
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
        printf("\n%lu\n", p->arr_time);
        p = p->next;
    }
}

void print_CPU_process_list(struct process_address_container* p)
{
    while (p != NULL) {
        printf(" %lu ", p->process_ptr->pid);
        p = p->next;
    }
    printf("\n");
}





// void add_all_processes_arriving_at_same_time(struct process* head, int num_cpus, struct cpu **cpu_array, int current_time){

//     struct process* curr = head;
//     while (curr != NULL){
//         if (curr->arr_time == current_time){
//             add_process_to_cpu(curr, num_cpus, cpu_array);
//             curr = curr->next;
//         }
//         else{
//             curr = curr->next;
//         }
//     }
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