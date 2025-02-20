// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-13

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-119

// worker.h is a header file that contains the Worker class declaration

#ifndef WORKER_H
#define WORKER_H

#include "Shared_Clock.h"

using namespace std;

/**
 * @brief The Worker class is used to manage the worker process.
 * It is responsible for running the worker process and checking if it should terminate.
 */
class Worker {
public:
    // Constructor:
    Worker(int = 0, int = 0); // The default constructor

    // Class member functions:
    void run(); // Run the worker

private:
    // Data fields:
    Shared_Clock clock;

    int terminate_seconds; // The seconds to terminate the worker
    int terminate_nano; // The nanoseconds to terminate the worker

    // Private member functions:
    bool check_termination(); // Check if the worker should terminate
};




#endif // WORKER_H