
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



/*****************************************************************        Function Signatures       *****************************************************************************/

void insert_process_data_into_linked_list(struct Process** head_ptr, char* process_data);
struct Process* set_cpu_running_process_ptr(struct Cpu* cpu_ptr);
void remove_process_from_cpu(struct Process** head_ref, float process_id);
void remove_child_process_from_parent(struct Process** children_list_head_ref, float process_id);
void add_process_to_cpu(struct Process* process, int num_cpus, struct Cpu **cpu_array);
bool has_same_arrival_times(struct Process* head);
bool has_process_arrived_at_current_time(struct Process* head, unsigned long int time);
struct Process* get_ptr_to_process_equal_to_curr_time(struct Process* head, unsigned long int time);
struct Process* get_ptr_to_process_equal_to_pid(struct Process* head, float process_id);
void swap(struct Process *a, struct Process *b);
void sort_remaining_execution_times(struct Process *head);
int calculate_turnaround_time(struct Process* head, int process_count);
double calculate_max_time_overhead(struct Process* head, int process_count);
double calculate_avg_time_overhead(struct Process* head, int process_count);
void free_linked_list(struct Process* head);

