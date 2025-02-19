// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-14

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-16

// Process_Manager.cpp is a source file that contains the implementation of the Process_Manager class

#include "Process_Manager.h"

// Constructor:
Process_Manager::Process_Manager(Shared_Clock &clock) : clock(clock) {}

// Class member functions:

/** @brief Creates a worker process and stores its data in the process table.
 *  @return first available slot in the process table
 */
int Process_Manager::find_empty_slot() {
    for (size_t i = 0; i < MAX_SLOT; i++) {
        if (process_table[i].occupied == 0) {
            return i; // Returns first available slot
        }
    }
    return -1; // No empty slot found
}

/** @brief Stores the worker data in the process table.
 *  @param slot The slot in the process table to store the data.
 *  @param worker The worker process to store the data.
 */
void Process_Manager::store_worker_data(int slot, Process &worker) {
    process_table[slot].occupied = 1; // Mark the slot as occupied
    process_table[slot].pid = worker.get_pid(); // Store the PID of the worker process
    process_table[slot].start_seconds = clock.get_seconds(); // Store the start time in seconds
    process_table[slot].start_nano = clock.get_nano(); // Store the start time in nanoseconds
}

// Creates a worker process and stores its data in the process table.
void Process_Manager::create_worker(int time_limit) {
    int slot = find_empty_slot(); 
    
    // Check if there is an empty slot in the process table
    if (slot == -1) {
        cerr << "Error: No empty slot found in the process table." << endl;
        return;
    }

    int terminate_sec = rand() % time_limit; // Random seconds from 0 to time_limit
    int terminate_nano = rand() % ONE_BILLION; // Random nanoseconds from 0 to 1 billion - 1

    Process worker; // Create a new worker process
    worker.launch(terminate_sec, terminate_nano); // Launch the worker process with the termination time
    store_worker_data(slot, worker); // Store the worker data in the process table
}

// Checks for terminated worker processes and updates the process table accordingly.
void Process_Manager::check_terminated_worker() {
    int status; // Status of the terminated process
    pid_t pid; // PID of the terminated process

    // Check for terminated processes
    while((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        for (size_t i = 0; i < MAX_SLOT; i++) {
            if (process_table[i].occupied && process_table[i].pid == pid) { // Find the terminated process in the process table
                cout << "Process Worker PID " << pid << " terminated." << endl;
                process_table[i].occupied = 0; // Mark the slot as free
                process_table[i].pid = -1; // Reset the PID
                break;
            }
        }
    }
}

// Prints the process table.
void Process_Manager::print_process_table() {
    cout << "\nOSS PID: " << getpid() << " SysClockS: " << clock.get_seconds() << " SysClockNS: " << clock.get_nano() << endl;
    cout << "Process Table:" << endl;
    cout << "Entry \t\t| Occupied \t\t| PID StartS \t\t| StartN" << endl;

    for(size_t i = 0; i < MAX_SLOT; i++) {
        cout << i << "\t\t" << process_table[i].occupied << "\t\t\t" << process_table[i].pid << "\t\t" << process_table[i].start_seconds << "\t\t" << process_table[i].start_nano << endl;
    }
    cout << string(30, '-') << endl;
}

/** @brief Terminates a process with the given PID.
 *  @param pid The PID of the process to terminate.
 */
void Process_Manager::terminate_process(int pid) {
    // Check if the process is in the process table
    bool found = false;

    for (size_t i = 0; i < MAX_SLOT; i++) {
        if (process_table[i].occupied && process_table[i].pid == pid) { // Find the process in the process table
            found = true; // Mark the process as found
            kill(pid, SIGTERM); // Send termination signal to the process
            int status; // Status of the terminated process
            waitpid(pid, &status, 0); // Wait for the process to terminate
            cout << "Process Worker PID" << pid << " terminated." << endl;
            process_table[i].occupied = 0; // Mark the slot as free
            process_table[i].pid = -1; // Reset the PID
            break;
        }
    }
    if (!found) {
        cout << "Process Worker PID" << pid << " not found." << endl;
    }
    
}

// Terminates all processes in the process table.
void Process_Manager::terminate_all() {
    bool found = false; // Check if any worker is found

    for (size_t i = 0; i < MAX_SLOT; i++) {
        if (process_table[i].occupied) { // Check if the slot is occupied
            found = true; // Mark the process as found
            kill(process_table[i].pid, SIGTERM); // Terminate the process
            int status; // Status of the terminated process
            waitpid(process_table[i].pid, &status, 0); // Wait for the process to terminate
            cout << "Process Worker PID" << process_table[i].pid << " terminated." << endl;
            process_table[i].occupied = 0; // Mark the slot as free
            process_table[i].pid = -1; // Reset the PID
        }
    }
    if (!found) {
        cout << "No active worker found." << endl;
    }
}

