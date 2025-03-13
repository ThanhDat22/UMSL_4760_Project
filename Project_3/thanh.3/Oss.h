// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-13

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-03-12


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
#include <sys/msg.h>     // For message queue
#include <cstdio>        // For perror()
#include <fstream>       // For file I/O
#include <signal.h>

// Constants
const int MAX_PCB = 20; // Maximum number of processes


// Struct for the Process Control Block (PCB)
struct PCB {
    int occupied; // 1 if occupied, 0 if not
    pid_t pid; // Process ID
    int start_seconds; // Start time in seconds
    int start_nanoseconds; // Start time in nanoseconds
    int message_sent; // track the number of messages sent to the worker
};

extern PCB pcb[MAX_PCB]; // Array of PCB structures

// Message structure definition
struct msg_buffer {
    long mtype; // Message type
    char str_data[100]; // Message data
    int int_data; // Integer data
}

// Global variables declaration
extern volatile sig_atomic_t timeout_flag;
extern volatile sig_atomic_t timer_tick;
extern int msqid;
extern ofstream fout;

//Function prototypes
//void signal_handler(int); // Signal handler
void print_usage(); // Print usage information
void init_process_table(); // Initialize the process table
void print_process_table(Clock*); // Print the process table
void increment_clock(Clock*, int); // Increment the clock
void check_terminated_workers(); // Check for terminated workers
int count_running_workers(); // Count the number of running workers
bool launch_worker(Clock*, int); // Launch a worker process
void parse_arguments(int, char**, int&, int&, int&, int&, string &); // Parse command line arguments
void setup_timer(int); // Setup the timer
bool is_number(const char*); // Check if a string is a number




#endif // OSS_H
