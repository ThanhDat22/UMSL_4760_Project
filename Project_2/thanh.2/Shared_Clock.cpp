// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-14

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-16

// shared_clock.cpp is a source file that contains the implementation of the Shared_Clock class

#include "Shared_Clock.h"

// Constructor:
Shared_Clock::Shared_Clock(key_t key, bool create): shm_key(key), shm_id(-1), clock(NULL) {
    shm_id = obtain_shm_id(create); // Obtain the shared memory ID
    attach_shm(); // Attach the shared memory segment
    if(create) { // If creating a new segment, initialize the clock
        initialize_clock();
    }
}

// Destructor:
Shared_Clock::~Shared_Clock() {
    if(clock) {
        shmdt(clock);
        clock = NULL; // Set the pointer to NULL
    }
}

// Getters:
Clock* Shared_Clock::get_clock() {
    return clock; // Return the pointer to the shared clock structure
}

// Class member functions:
void Shared_Clock::remove_segment() {
    if(shm_id >= 0) {
        shmctl(shm_id, IPC_RMID, NULL); // Remove the shared memory segment
        shm_id = -1; // Set the ID to -1
    }
}

// Private member functions:

int Shared_Clock::obtain_shm_id(bool create) {
    // Base permissions
    int permissions = 0666; // Read and write permissions for owner, group, and others
    if (create) { permissions |= IPC_CREAT; } // Add create permission if needed

    // Obtain the shared memory ID
    int id = shmget(shm_key, sizeof(Clock), permissions);
    if(id < 0) {
        cerr << ("shmget failed") << endl;
        exit(1);
    }
    return id;
}

void Shared_Clock::attach_shm() {
    // Attach the shared memory segment
    void* ptr = shmat(shm_id, NULL, 0);
    if(ptr == (void*)-1) {
        cerr << ("shmat failed") << endl;
        exit(1);
    }
    clock = (Clock*)ptr; // Cast the pointer to Clock type
}

void Shared_Clock::initialize_clock() {
    clock->seconds = 0; // Initialize seconds to 0
    clock->nanoseconds = 0; // Initialize nanoseconds to 0
}