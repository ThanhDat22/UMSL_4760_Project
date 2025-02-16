// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-13

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-16

#ifndef OSS_H
#define OSS_H

#include "Process_Manager.h"
#include "Shared_Clock.h"

class Oss {
public:
    // Constructor:
    Oss(int, int); // The default constructor

    // Class member functions:
    void run(); // Run the OSS execution
    void wait_for_worker(); // Wait for the all workers to finish
    void parse_arguments(int, char**); // Parse the command line arguments

private:
    // Data fields:
    int num_processes; // Total number of worker processes to create
    int max_simultaneous; // Maximum number of worker processes that can run simultaneously
    Shared_Clock clock; // Shared memory clock
    Process_Manager process_manager; // Process manager to manage the worker processes

}





#endif // OSS_H
