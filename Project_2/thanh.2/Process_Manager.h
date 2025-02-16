// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-14

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-16

// Process_Manager.h is a header file that contains the Process_Manager class declaration

#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include "Shared_Clock.h"

#include <iostream>
#include <sys/types.h>
#include <string>

using namespace std;


const int MAX_SLOT = 20; // Maximum number of processes

// Structure for the Process Control Block (PCB)
struct Process_Control_Block {
    int occupied; // 1 if occupied, 0 if not occupied
    pid_t pid; // Process ID of worker process
    int start_seconds; // Start time in seconds
    int start_nano; // Start time in nanoseconds

    // Constructor
    Process_Control_Block() : occupied(0), pid(-1), start_seconds(0), start_nano(0) { }
};

/**  @brief The Process_Manager class is used to manage the worker processes.
 * It is responsible for creating, terminating, and storing the worker processes.
 */
class Process_Manager {
public:
    // Constructor
    Process_Manager(Shared_Clock &); // Initialize the process manager with a reference to the shared clock

    // Member functions
    void create_worker(); // Create a worker process
    void check_terminated_worker(); // Check for terminated worker processes
    void terminate_process(int); // Terminate a process
    void terminate_all(); // Terminate all processes
    void print_process_table(); // Print the process table

private:
    // Data fields:
    Process_Control_Block process_table[MAX_SLOT]; // Array of PCBs
    Shared_Clock &clock; // Reference to the shared clock

    // Private member functions:
    int find_empty_slot(); // Find an empty slot in the process table
    void store_worker_data(int, Process &); // Store the worker data in the process table

};

#endif // PROCESS_MANAGER_H