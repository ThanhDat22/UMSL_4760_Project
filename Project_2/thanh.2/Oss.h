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

const int MAX_PCB = 20; // Maximum number of processes
const int INCREMENT_NS = 1000000; // Increment in nanoseconds per loop

struct PCB {
    int occupied; // 1 if occupied, 0 if not
    pid_t pid; // Process ID
    int start_seconds; // Start time in seconds
    int start_nanoseconds; // Start time in nanoseconds
};






#endif // OSS_H
