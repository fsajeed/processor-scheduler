# Process Scheduler
A CPU scheduling simulator for single, dual, and multi-processor environments.

## Overview
This project simulates process scheduling in a multi-processor system. It assigns incoming processes to available processors using a shortest-time-remaining (STR) scheduling algorithm. The scheduler supports:
- ✅ Single processor scheduling (Preemptive STR)
- ✅ Dual processor scheduling (Handles parallelizable processes)
- ✅ Multi-processor scheduling (N ≥ 3)
- ✅ Performance optimization (Custom scheduling strategy)

## Features
- **Preemptive Scheduling:** Assigns CPU time to processes dynamically.
- **Parallelizable Process Handling:** Splits tasks across multiple CPUs.
- **Efficient Makespan Calculation:** Tracks overall execution time.
- **Performance Metrics:** Reports turnaround time, time overhead, and total execution time.

## Installation and Compilation
Ensure you have a C compiler (e.g., GCC) installed. Then, clone the repository and compile the program:
```
git clone https://github.com/fsajeed/processor-scheduler.git  
cd processor-scheduler  
make
```

## Usage
Run the scheduler with the following command:
```
./allocate -f <process_file> -p <processors> [-c]
```
Note:
* -f <process_file>: Specifies the input file containing process data.
* -p <processors>: Number of processors (1, 2, or N).
* -c (optional): Enables custom scheduling optimization.

## Example
```
./allocate -f processes.txt -p 2
```
Simulates scheduling with two processors using the default algorithm.

## Input Format
The process file should contain one process per line, formatted as:
<arrival_time> <process_id> <execution_time> <parallelizable>

Example (processes.txt):
----------
> 0 4 30 n  
> 3 2 40 n  
> 5 1 20 p  
> 20 3 30 n
----------
* arrival_time: When the process enters the queue.
* process_id: Unique identifier.
* execution_time: Time required to complete.
* parallelizable: p (parallelizable) or n (non-parallelizable).

## Output Format
The scheduler prints execution events and performance metrics.

Sample Output
-----------------------------------------
> 20,RUNNING,pid=15,remaining_time=10,cpu=0  
> 30,FINISHED,pid=15,proc_remaining=0

> Turnaround time 62  
> Time overhead 2.93 1.90  
> Makespan 120
-----------------------------------------

## Project Structure
📂 src/               – Source code
  * allocate.c        – Main program
  * functions.c       – Scheduling logic
  * functions.h       – Function prototypes

📂 testcases/          – Sample input files

📄 scheduler_testing_commands.txt – Testing file

📄 Makefile            – Compilation instructions

📄 README.md           – Documentation
