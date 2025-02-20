// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-13

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-19

// oss.h is a header file that contains the Oss class declaration

#ifndef OSS_H
#define OSS_H

#include "Process_Manager.h"
#include "Shared_Clock.h"
#include "Worker.h"

/**
 * @brief The Oss class is used to manage the operating system simulation.
 * It is responsible for creating and managing the worker processes.
 */
class Oss {
public:
    // Constructor:
    Oss(int = 0, int = 0, int = 0, int = 0); // The default constructor

    // Class member functions:
    void run(); // Run the OSS execution
    void wait_for_workers(); // Wait for the all workers to finish
    void parse_arguments(int, char**); // Parse the command line arguments

private:
    // Data fields:
    int num_processes; // Total number of worker processes to create
    int max_simultaneous; // Maximum number of worker processes that can run simultaneously
    int time_limit; // Time limit for the worker processes (-t flag)
    int interval_in_ms; // Interval in milliseconds for the OSS to check the worker processes (-i flag)

    Shared_Clock clock; // Shared memory clock
    Process_Manager process_manager; // Process manager to manage the worker processes
    
    // Private member functions:
    void manage_workers(); // Manage the worker processes
    void update_clock(); // Update the shared clock
    void print_usage(); // Print the usage of the program
};





#endif // OSS_H
