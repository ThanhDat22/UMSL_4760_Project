// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-03-14

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-03-15

#ifndef MESSAGE_H
#define MESSAGE_H

#include "Shared_Clock.h"
#include "Pcb.h"         // For Process Control Block (PCB) structure
#include "Oss.h"          // For PCB structure and process table

#include <sys/ipc.h>    // For IPC key
#include <sys/msg.h>   // For message queue
#include <sys/types.h>   // For pid_t
#include <iostream>      


using namespace std;

#define MSG_TYPE_TO_WORKER 1 // Message type for worker processes
#define MSG_TYPE_FROM_WORKER 2 // Message type for OSS to receive from worker processes

// Define the message structure
#pragma pack(push, 1) // Ensure proper packing of the structure
struct Message {
    long mtype; // Message type
    int worker_id; // Worker ID
    int command; // Command or signal
    int seconds; // Current seconds from the worker
    int nanoseconds; // Current nanoseconds from the worker
};
#pragma pack(pop) // Ensure proper packing of the structure

extern int msg_queue_id; // Message queue ID

// Function prototypes
void create_message_queue(); // Create the message queue
void receive_message(); // Receive a message from the queue
void send_message(int, int); // Send a message to a worker
void cleanup_message_queue(); // Cleanup the message queue

#endif // MESSAGE_H