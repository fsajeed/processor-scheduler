#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

struct process {
    int arr_time;                // make unsigned long int
    int pid;      // make unsigned long int
    int exec_time;
    char *parallelisability;     // Not sure if needed for now // need to conver to just character later

    int rem_exec_time;
    int cpu_id;

    struct process *next; // Pointer pointing to the next process stored in the Linked List

    // store CPU id too - -1 for no cpu
};

struct cpu {
    int cpu_id;
    struct process** processes; // NEED to be an array of processes
};


// Function that returns smallest element
// from the linked list.
struct process* smallestElement(struct process* head)       // NEED TO SORT OUT TIES!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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
  
        // If min is greater then head->data then
        // assign value of head->data to min
        // otherwise node point to next node.
        if (min > (temp->arr_time)) {
            min = temp->arr_time;
            min_ptr = temp;
        }
        temp = temp->next;
    }
    return min_ptr;
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

        if (curr==NULL){
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

int calc_remaining_cpu_exec_time(struct process* cpu){
    return 0;
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