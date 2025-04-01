// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-03-17

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-0-01

// Struct for the Process Control Block (PCB)

#ifndef PCB_H
#define PCB_H

#include <sys/types.h>   // For pid_t

#define MAX_PCB 20

struct PCB {
    int occupied; // 1 if occupied, 0 if not
    pid_t pid; // Process ID
    int start_seconds; // Start time in seconds
    int start_nanoseconds; // Start time in nanoseconds

    int messages_sent;
    int messages_received;
    int total_runtime_sec;
    int total_runtime_ns;

    int priority;

    int queue_level;         // 0 = high, 1 = medium, 2 = low
    int blocked;             // 0 = ready, 1 = blocked

    int serviceTimeSeconds;  // Total scheduled time (optional for stats)
    int serviceTimeNano;

    int eventWaitSec;        // When I/O wait ends (used if blocked)
    int eventWaitNano;
};

extern PCB pcb[MAX_PCB]; // Array of PCB structures

#endif