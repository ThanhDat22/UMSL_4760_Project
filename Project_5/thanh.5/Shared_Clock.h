// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-14

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-21


// shared_clock.h is a header file that contains the Shared_Clock declaration

#ifndef SHARED_CLOCK_H
#define SHARED_CLOCK_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

using namespace std;

#define SHM_KEY_CLOCK 2031974


typedef struct Clock {
    int seconds;
    int nanoseconds;
} Shared_Clock; // Structure to represent the clock

Shared_Clock* attach_clock(int*, int); // Attach to the shared clock
void detach_clock(Shared_Clock*); // Detach from the shared clock
void destroy_clock(int); // Destroy the shared clock


#endif // SHARED_CLOCK_H