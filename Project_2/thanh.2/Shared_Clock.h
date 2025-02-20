// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-14

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-16


// shared_clock.h is a header file that contains the Shared_Clock class declaration

#ifndef SHARED_CLOCK_H
#define SHARED_CLOCK_H

#include <sys/ipc.h>
#include <sys/shm.h>

#include <iostream>

using namespace std;

const int SHMKEY = 859047; // The key for the shared memory segment
const int ONE_BILLION = 1000000000; // One billion


struct Clock {
    int seconds;
    int nanoseconds;
};

class Shared_Clock {
public:
    // Constructor:
    Shared_Clock(key_t = 0, bool = false); // Constructor to initialize the shared clock
    ~Shared_Clock(); // Destructor to clean up the shared clock

    // Getters:
    Clock* get_clock(); // Get the pointer to the shared clock structure

    // Class member functions:
    void remove_segment(); // Remove the shared memory segment
private:
    // Data fields:
    key_t shm_key; // The key for the shared memory segment
    int shm_id; // The ID of the shared memory segment
    Clock* clock; // Pointer to the shared clock structure

    // Private member functions:
    int obtain_shm_id(bool); // Obtain the shared memory ID
    void attach_shm(); // Attach the shared memory segment
    void initialize_clock(); // Initialize the shared clock

};


#endif // SHARED_CLOCK_H