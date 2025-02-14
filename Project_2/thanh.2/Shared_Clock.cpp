// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-14

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-14

#include "Shared_Clock.h"

// Constructor:
Shared_Clock::Shared_Clock() {
    // Create shared memory segment
    shared_memory_id = shmget(SHARED_CLOCK_KEY, sizeof(int) * 2, IPC_CREAT | 0666); // Allocate memory for 2 integers (seconds and nanoseconds)

    // Check if shared memory creation was successful
    if (shared_memory_id < 0) {
        cerr << "[ERROR] Failed to create shared memory segment." << endl;
        exit(1);
    }

    // Attach shared memory segment to process's address space
    shared_memory_clock = (int*) shmat(shared_memory_id, NULL, 0);
    if(shared_memory_clock == (int*) -1) {
        cerr << "[ERROR] Failed to attach shared memory segment." << endl;
        exit(1);
    }

    // Initialize shared memory clock value:
    shared_memory_clock[0] = 0; // seconds
    shared_memory_clock[1] = 0; // nanoseconds
}

// Destructor:
Shared_Clock::~Shared_Clock() {
    shmdt(shared_memory_clock); // Detach shared memory segment
    shmctl(shared_memory_id, IPC_RMID, NULL); // Remove shared memory segment
}

// Class member functions:

// Return the current seconds
int Shared_Clock::get_second() {
    return shared_memory_clock[0]; 
}

// Return the current nanoseconds
int Shared_Clock::get_nano_second() {
    return shared_memory_clock[1]; 
}

/** Increment the seconds
 *  @param seconds: the number of seconds to increment
 */
void Shared_Clock::increment_second(int seconds) {
    shared_memory_clock[0] += seconds; // Increment the seconds
}

/** Increment the nanoseconds
 *  @param nano_seconds: the number of nanoseconds to increment
 */
void Shared_Clock::increment_nano_second(int nano_seconds) {
    shared_memory_clock[1] += nano_seconds;
    if (shared_memory_clock[1] >= 1000000000) { // If nanoseconds exceed 1 second
        int extra_seconds = shared_memory_clock[1] / 1000000000; // Calculate extra seconds
        shared_memory_clock[0] += extra_seconds; // Increment seconds
        shared_memory_clock[1] %= 1000000000; // Update nanoseconds
    }
}

/** Manually set the time
 *  @param seconds: the number of seconds to set
 *  @param nano_seconds: the number of nanoseconds to set
 */
void Shared_Clock::set_time(int seconds, int nano_seconds) {
    shared_memory_clock[0] = seconds; // Set the seconds
    shared_memory_clock[1] = nano_seconds; // Set the nanoseconds
}

// Detach the shared memory segment
void Shared_Clock::detach_memory() {
    shmdt(shared_memory_clock); // Detach shared memory segment
}