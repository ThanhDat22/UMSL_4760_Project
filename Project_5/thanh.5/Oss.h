#ifndef OSS_H
#define OSS_H

#include "Shared_Clock.h"
#include "Resource_Table.h"
#include "Message.h"

#include <vector>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>

const int MAX_USER = 40; // Total number of user created over lifetime
const int MAX_ACTIVE = 18; // Maximum number of active user at any time

using namespace std;

class Oss {
public:
    // Constructor
    Oss(int = 0, int = 0, int = 0, const string& = ""); 
    
    ~Oss(); // Destructor

    void run(); // Main function to run the OSS

private:
    // Data fields
    int msg_queue_id; // Message queue ID
    int shm_id; // Shared memory ID
    Shared_Clock* clock; // Pointer to shared clock

    ofstream log_file;
    string log_file_name; // Log file name
    int max_user; // Maximum number of users
    int max_simulation_seconds; // Maximum simulation time in seconds
    unsigned long long launch_interval_ns; // Launch interval in nanoseconds

    Resource_Table resource_table; 
    vector<pid_t> active_users; // Vector to store active user PIDs
    vector<int> wait_queue[MAX_RESOURCES]; // Wait queues for each resource

    // Private methods
    void setup_ipc();
    void launch_user();
    void handle_message();
    void check_deadlock();
    void log_state();
    process_wait_queue();
    void cleanup();
};

#endif // OSS_H