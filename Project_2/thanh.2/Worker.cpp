// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-13

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-20

// worker.cpp is a source file that contains the implementation of the Worker class

#include "Worker.h"

// Constructor:
Worker::Worker(int seconds, int nano) : terminate_seconds(seconds), terminate_nano(nano) {
    clock = Shared_Clock(); // Initialize the shared clock
    start_seconds = clock.get_second(); // Get the start time in seconds
    start_nano = clock.get_nano_second(); // Get the start time in nanoseconds

    cout << "[Worker " << getpid() << "] Just Starting" << endl;
    cout << "[Worker " << getpid() << "] PPID: " << getppid()
         << " SysClockS: " << start_seconds
         << " SysClockNano: " << start_nano
         << " TermTimeS: " << terminate_seconds
         << " TermTimeNano: " << terminate_nano
         << endl;    
}

// Class member functions:
void Worker::run() {
    int last_check_seconds = -1;

    while(true) {
        int current_seconds = clock.get_second();
        int current_nano = clock.get_nano_second();

        int elapsed_time = current_seconds - start_seconds;

        // Print periodic message when seconds increase
        if(current_seconds != last_check_seconds) {
            last_check_seconds = current_seconds; // Update last check time
            cout << "[Worker " << getpid() << "] SysClockS: " << current_seconds
                 << " SysClockNano: " << current_nano
                 << " -- " << (elapsed_time) << " seconds have passed since starting"
                 << endl;          
        }

        // Terminate the worker when termination time is reached
        if(check_termination()) {
            cout << "[Worker " << getpid() << "] SysClockS: " << current_seconds
                 << " SysClockNano: " << current_nano << " -- Terminating" << endl;
            break;    
        }
    }
    clock.detach_memory(); // Detach the shared memory before exiting
}

// Check if the worker should terminate
bool Worker::check_termination() {
    int current_seconds = clock.get_second();
    int current_nano = clock.get_nano_second();

    // Check if termination time has been reached
    if(current_seconds > terminate_seconds || 
       (current_seconds == terminate_seconds && current_nano >= terminate_nano)) {
        return true;
    }
    return false;
}