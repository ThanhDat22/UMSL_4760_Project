// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-03-13

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-03-13

#include "Shared_Clock.h"

#include <iostream>
#include <cstdlib>    // for atoi()
#include <unistd.h>   // for getpid(), getppid()
#include <signal.h>   // for signal(), SIGUSR1


using namespace std;

// Message structure definition
struct msg_buffer {
    long mtype; // Message type
    char str_data[100]; // Message data
    int int_data; // Integer data
}

// Function prototypes

void print_usage(); // Print usage information
void starting_message(int, int, int, int); // Print starting message
void parse_arguments(int, char**, int&, int&); // Parse command line arguments
void compute_termination_time(Clock*, int, int, int&, int&, int&, int&); // Compute termination time
void run_worker(Clock*, int, int, int, int); // Run worker process
void wake_signal_handler(int); // Signal handler for wake signal

