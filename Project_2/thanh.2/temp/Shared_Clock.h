// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-14

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-16


// shared_clock.h is a header file that contains the Shared_Clock class declaration

#ifndef SHARED_CLOCK_H
#define SHARED_CLOCK_H

/**
 * @brief The Shared_Clock class is used to manage the shared clock between the server and the client.
 *  create shared memory
 *  store the simulated clock
 *  allow reading and updating the clock
 *  allow detaching the shared memory
 */

#include <iostream>
#include <sys/ipc.h> // For IPC_CREAT
#include <cstdlib> // For exit()
#include <sys/shm.h> // For shmget(), shmat(), shmdt(), shmctl()


using std::cerr;
using std::endl;

const key_t SHARED_CLOCK_KEY = 2031974; // Key for the shared memory segment
const int ONE_BILLION = 1000000000; // One billion for nanoseconds

class Shared_Clock {

public:
    // Constructor
    Shared_Clock();

    // Destructor
    ~Shared_Clock();

    // Member functions
    int get_second(); // Get the current seconds
    int get_nano_second(); // Get the current nanoseconds

    void increment_second(int); // Increment the seconds
    void increment_nano_second(int); // Increment the nanoseconds

    void set_time(int, int); // Manually set the time
    void detach_memory(); // Detach the shared memory

private:
    // Data fields
    int shared_memory_id; // ID of the shared memory segment
    int* shared_memory_clock; // Pointer to the shared memory clock

};


#endif // SHARED_CLOCK_H