// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-13

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-19


#ifndef OSS_H
#define OSS_H

#include "Shared_Clock.h"

#include <csignal>       // For signal(), SIGALRM, SIGTERM
#include <sys/time.h>    // For setitimer()
#include <unistd.h>      // For fork(), execlp(), getpid(), alarm()
#include <sys/types.h>   // For pid_t
#include <sys/wait.h>    // For waitpid()
#include <cstdlib>       // For exit(), atoi()
#include <iostream>      // For std::cout, std::cerr
#include <sstream>       // For std::ostringstream
#include <getopt.h>      // For getopt()
#include <cstring>       // For strcmp()

// Constants
const int MAX_PCB = 20; // Maximum number of processes
const int INCREMENT_NS = 1000000; // Increment in nanoseconds per loop

// Struct for the Process Control Block (PCB)
struct PCB {
    int occupied; // 1 if occupied, 0 if not
    pid_t pid; // Process ID
    int start_seconds; // Start time in seconds
    int start_nanoseconds; // Start time in nanoseconds
    int message_sent; // track the number of messages sent to the worker
};

extern PCB pcb[MAX_PCB]; // Array of PCB structures

// Global variables declaration
extern volatile sig_atomic_t timeout_flag;
extern volatile sig_atomic_t timer_tick;

//Function prototypes
void signal_handler(int); // Signal handler
void print_usage(); // Print usage information
void init_process_table(); // Initialize the process table
void print_process_table(Clock*); // Print the process table
void increment_clock(Clock*, int); // Increment the clock
void check_terminated_workers(); // Check for terminated workers
int count_running_workers(); // Count the number of running workers
//string to_string(const int); // Convert an integer to a string
bool launch_worker(Clock*, int); // Launch a worker process
void parse_arguments(int, char**, int&, int&, int&, int&); // Parse command line arguments
void setup_timer(int); // Setup the timer
bool is_number(const char*); // Check if a string is a number



// Function definitions

/** @brief Signal handler for SIGUSR1 signal.
 *  @param signum The signal number.
 */
void signal_handler(int sig) {
    if(sig == SIGALRM) {
        timer_tick = 1;
    } else if(sig == SIGTERM) {
        timeout_flag = 1;
    }
}

/** @brief Sets up a timer to send SIGALRM signal at specified intervals.
 *  @param interval_ms The time interval in milliseconds.
 */
void setup_timer(int interval_ms) {
    struct itimerval timer;
    timer.it_interval.tv_sec = interval_ms / 1000;
    timer.it_interval.tv_usec = (interval_ms % 1000) * 1000;
    timer.it_value = timer.it_interval;
    setitimer(ITIMER_REAL, &timer, NULL);
}

/** @brief Parses command line arguments.
 *  @param argc The number of command line arguments.
 *  @param argv The array of command line arguments.
 *  @param num_workers The number of worker processes to launch.
 *  @param max_simul_workers The maximum number of worker processes to allow to run simultaneously.
 *  @param time_limit The maximum time limit in seconds for each worker process.
 *  @param interval The time interval in milliseconds to launch user processes.
 */
void parse_arguments(int argc, char** argv, int& num_workers, int& max_simul_workers, int& time_limit, int& interval) {
    int options;
    num_workers = 0;
    max_simul_workers = 0;
    time_limit = 0;
    interval = 0;

    // Check for -h anywhere in the arguments
    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "-h") == 0) {
            print_usage();
            exit(0);
        }
    }

    while((options = getopt(argc, argv, "hn:s:t:i:")) != -1) {
        switch(options) {
            case 'n':              
                if(!is_number(optarg)) {
                    cout << "Invalid number of workers" << endl;
                    exit(1);
                }
                num_workers = atoi(optarg);
                break;
            case 's':
                if(!is_number(optarg)) {
                    cout << "Invalid number of simultaneous workers" << endl;
                    exit(1);
                }
                max_simul_workers = atoi(optarg);
                break;
            case 't':
                if(!is_number(optarg)) {
                    cout << "Invalid time limit" << endl;
                    exit(1);
                }
                time_limit = atoi(optarg);
                break;
            case 'i':
                if(!is_number(optarg)) {
                    cout << "Invalid interval" << endl;
                    exit(1);
                }
                interval = atoi(optarg);
                break;
            default:
                print_usage();
                exit(0);
        }
    }
    if(num_workers <= 0 || max_simul_workers <= 0 || time_limit <= 0 || interval <= 0) {
        cout << "Invalid arguments" << endl;
        exit(1);
    }
}

/** @brief Checks if a string is a number.
 *  @param str The string to check.
 *  @return True if the string is a number, false otherwise.
 */
bool is_number(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) return false;
    }
    return true;
}

// Print usage information
void print_usage() {
    cout << "Usage: ./oss [-h] [-n proc] [-s simul] [-t timelimitForChildren] [-i intervalInMsToLaunchChildren]" << endl;
    cout << "[-h] Display this help message." << endl;
    cout << "[-n proc] Stands for the number of worker processes to launch." << endl;
    cout << "[-s simul] Indicates the maximum number of worker processes to allow to run simultaneously." << endl;
    cout << "[-t timelimitForChildren] Indicates the maximum time limit in seconds for each worker process." << endl;
    cout << "[-i intervalInMsToLaunchChildren] Indicates the time interval in milliseconds to launch user processes." << endl;
}

// Initialize the process table
void init_process_table() {
    for(int i = 0; i < MAX_PCB; i++) {
        pcb[i].occupied = 0;
        pcb[i].pid = 0;
        pcb[i].start_seconds = 0;
        pcb[i].start_nanoseconds = 0;
        pcb[i].message_sent = 0;
    }
}

// Print the process table
void print_process_table(Clock* clock) {
    cout << "OSS PID:" << getpid()
         << " SysClockS:" << clock->seconds
         << " SysClockNano:" << clock->nanoseconds << endl;
    cout << "Process Table:" << endl;
    cout << "Entry\tOccupied\tPID\tStartS\tStartN\tMessagesSent" << endl;
    for (int i = 0; i < MAX_PCB; i++) {
        cout << i << "\t" <<pcb[i].occupied << "\t\t" 
             << pcb[i].pid << "\t" 
             << pcb[i].start_seconds << "\t" 
             << pcb[i].start_nanoseconds << "\t"
             << pcb[i].message_sent << endl;
    }
}


/** @brief Increments the clock by a specified number of nanoseconds.
 *  @param clock Pointer to the shared clock structure.
 *  @param increment_ns Number of nanoseconds to increment.
 */
void increment_clock(Clock* clock, int increment_ns) {
    clock->nanoseconds += increment_ns;
    if (clock->nanoseconds >= ONE_BILLION) {
        clock->seconds += 1;
        clock->nanoseconds -= ONE_BILLION;
    }

    // Notify all worker processes to wake up
    for (int i = 0; i < MAX_PCB; i++) {
        if (pcb[i].occupied) {
            kill(pcb[i].pid, SIGUSR1);  // Wake up worker
        }
    }
}

// Check for terminated workers
void check_terminated_workers() {
    int status;
    pid_t pid;
    while((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        for(int i = 0; i < MAX_PCB; i++) {
            if(pcb[i].occupied && (pcb[i].pid == pid)) {
                pcb[i].occupied = 0;
                pcb[i].pid = 0;
                pcb[i].start_seconds = 0;
                pcb[i].start_nanoseconds = 0;
                cout << "OSS: Worker PID " << pid << " terminated with exit code " << WEXITSTATUS(status) << "." << endl;
                break;
            }
        }
    }
}

/** @brief Counts the number of running workers.
 *  @return The number of running workers.
 */
int count_running_workers() {
    int count = 0;
    for(int i = 0; i < MAX_PCB; i++) {
        if(pcb[i].occupied) {
            count++;
        }
    }
    return count;
}

/** @brief Converts an integer to a string.
 *  @param num The integer to convert.
 *  @return The string representation of the integer.
 */
// string to_string(const int num) {
//     ostringstream oss;
//     oss << num;
//     return oss.str();
// }

/** @brief Launches a worker process.
 *  @param clock Pointer to the shared clock structure.
 *  @param time_upper_bound The upper bound for the worker's runtime in seconds.
 *  @return True if the worker was launched successfully, false otherwise.
 */
bool launch_worker(Clock* clock, int time_upper_bound) {


    // Find a free slot in the process table.
    int slot = -1;
    for (int i = 0; i < MAX_PCB; i++) {
        if (!pcb[i].occupied) {
            slot = i;
            break;
        }
    }
    if (slot == -1) {
        return false; // No free slot available.
    }

    // Randomize worker runtime
    int worker_seconds = 1 + rand() % time_upper_bound;
    int worker_nanoseconds = rand() % ONE_BILLION;

    pid_t pid = fork();
    if (pid < 0) {
        cerr << "Fork failed" << endl;
        return false;
    } else if (pid == 0) { // Child process: execute worker
        
        // Convert seconds and nanoseconds to strings
        string sec_str = to_string(worker_seconds);
        string nano_str = to_string(worker_nanoseconds);
        // Execute the worker program with the arguments
        execlp("./worker", "./worker", sec_str.c_str(), nano_str.c_str(), (char*)NULL);
        cerr << "Exec failed" << endl;
        exit(1);
    } else { // Parent process: update process table
        pcb[slot].occupied = 1;
        pcb[slot].pid = pid;
        pcb[slot].start_seconds = clock->seconds;
        pcb[slot].start_nanoseconds = clock->nanoseconds;
        cout << "OSS: Launched worker PID " << pid << " in slot " << slot 
             << " (Runtime: " << worker_seconds << " sec, " << worker_nanoseconds << " ns)" << endl;
        return true;
    }
}


#endif // OSS_H
