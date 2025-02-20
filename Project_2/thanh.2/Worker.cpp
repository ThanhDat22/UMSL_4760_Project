// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-13

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-20

// worker.cpp is a source file that contains the implementation of the Worker class

#include "Worker.h"

// Constructor:
Worker::Worker(int seconds, int nano) : terminate_seconds(seconds), terminate_nano(nano) {
    cout << "[Worker " << getpid() << "] Just Starting" << endl;
    cout << "[Worker " << getpid() << "] PPID: " << getppid()
         << " SysClockS: " << clock.get_second()
         << " SysClockNano: " << clock.get_nano_second()
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

        // Print periodic message when seconds increase
        if(current_seconds != last_check_seconds) {
            last_check_seconds = current_seconds; // Update last check time
            cout << "[Worker " << getpid() << "] SysClockS: " << current_seconds
                 << " SysClockNano: " << current_nano
                 << " -- " << (current_seconds - terminate_seconds) << " seconds have passed since starting"
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