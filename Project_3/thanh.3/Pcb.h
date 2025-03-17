// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-03-17

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-03-17

// Struct for the Process Control Block (PCB)

#ifndef PCB_H
#define PCB_H

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
};

extern PCB pcb[MAX_PCB]; // Array of PCB structures

#endif