// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-13

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-19

// main.cpp is a source file that contains the main program

#include "Oss.h"
#include "worker.h"

int main(int argc, char** argv) {

    // Check if the program is running as worker
    if((argc == 3) && (strcmp(argv[0], "./worker") == 0)) {
        int seconds = atoi(argv[1]);
        int nano = atoi(argv[2]);
        Worker worker(seconds, nano);
        worker.run();
        return 0;
    } 
    
    // Otherwise it is running as oss

    // Set the default values for the command line arguments
    int num_processes = 0;
    int max_simultaneous = 0;
    int time_limit = 0;
    int interval_in_ms = 0;

    // Create oss object
    Oss oss(num_processes, max_simultaneous, time_limit, interval_in_ms);
    oss.parse_arguments(argc, argv); // Parse the command line arguments
    oss.run(); // Run the oss execution
    return 0;

}