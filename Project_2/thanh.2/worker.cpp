// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-13

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-14

// worker.cpp is a source file that contains the implementation of the Worker class
#include "Worker.h"

// Constructor:
Worker::Worker(int seconds, int nano) : terminate_seconds(seconds), terminate_nano(nano) {
    cout << "[Worker " << getpid() << "] will terminate at " << terminate_seconds << " seconds " << terminate_nano << " nanoseconds." << endl;
}

// Class member functions:
void Worker::run() {
    int last_check_seconds = -1;

    while(true) {
        int current_seconds = clock.get_second();
        int current_nano = clock.get_nano_second();

        if(current_seconds != last_check_seconds) {
            last_check_seconds = current_seconds; // Update last check time
            if(check_terminate()) {
                cout << "[Worker " << getpid() << "] terminating at " << current_seconds << " seconds " << current_nano << " nanoseconds." << endl;
                break;
            }          
        }
    }
    clock.detach_memory(); // Detach the shared memory before exiting
}

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