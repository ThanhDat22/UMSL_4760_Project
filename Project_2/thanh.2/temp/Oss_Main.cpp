// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-13

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-20

#include "Oss.h"
#include "Worker.h"

#include <string.h>

int main(int argc, char** argv) {
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