// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-03-13

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-03-13
#ifndef WORKER_H
#define WORKER_H

#include "Shared_Clock.h"
#include "Message.h"   

#include <iostream>
#include <cstdlib>    // for atoi()
#include <unistd.h>   // for getpid(), getppid()
#include <signal.h>   // for signal(), SIGUSR1
#include <sys/msg.h>
#include <sys/ipc.h>
#include <cstring>
#include <cstdio>


using namespace std;


// Function prototypes

void print_usage(); // Print usage information
void starting_message(int, int, int, int); // Print starting message
void parse_arguments(int, char**, int&, int&); // Parse command line arguments
void compute_termination_time(Clock*, int, int, int&, int&, int&, int&); // Compute termination time
void run_worker(Clock*, int, int, int, int); // Run worker process
void wake_signal_handler(int); // Signal handler for wake signal

int simulate_full_quantum();
int simulate_io_block();
int simulate_termination();
void send_worker_response(int, int, Clock*);


#endif // WORKER_H

