#ifndef USER_H
#define USER_H

#include "Shared_Clock.h"
#include "Message.h"
#include "Frame_Table.h"

#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;

class User {
public:
    User();
    void run(); // main execution loop for the user process

private:
    int pid_index;              // index in the process table (0–17)
    int msg_queue_id;           // message queue ID
    int shm_id;                 // shared memory ID
    Shared_Clock* clock;        // pointer to shared system clock
    int termination_sec;        // time (sec) when process should terminate
    int termination_ns;         // time (ns) when process should terminate

    void init();                // setup shared memory, queue, termination time
    bool check_termination();   // check if current time >= termination time
    void request_memory(int, bool);
    void send_termination();    // notify oss of termination
    void wait_for_grant();      // block until granted by oss
    int generate_random_address(); // generate random memory address
};


#endif // USER_H