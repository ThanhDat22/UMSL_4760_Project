// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-03-14

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-04-21

#ifndef MESSAGE_H
#define MESSAGE_H

const int MAX_RESOURCES = 5;
const int MAX_INSTANCES = 10;
const int MAX_PROCESSES = 18;


// Define the message structure
struct Message {
    long mtype; // Message type
    pid_t pid; // Process ID of the sender
    int action; //MSG_TYPE
    int resource_id; 
};


#endif // MESSAGE_H