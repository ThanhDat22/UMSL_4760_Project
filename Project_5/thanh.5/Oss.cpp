#include "Oss.h"
#include "Deadlock.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <csignal>
#include <algorithm>
#include <fcntl.h>



Oss::Oss(int max_users, int max_seconds, int launch_interval, const std::string& logfile_name)
    : msg_queue_id(-1), shm_id(-1), clock(nullptr),
      max_user(max_users), max_simulation_seconds(max_seconds),
      launch_interval_ns(launch_interval * 1000000ULL), log_file_name(logfile_name) {
    srand(time(NULL));
    setup_ipc();
}

Oss::~Oss() {
    cleanup();
    if (log_file.is_open()) { 
        log_file.close(); 
    }
}

// Set up IPC mechanisms
void Oss::setup_ipc() {
    // Attach shared clock
    clock = attach_clock(&shm_id, 1); // attach to existing shared clock
    if (!clock) {
        cerr << "OSS failed to attach to clock\n";
        exit(1);
    }

    // Create message queue
    int fd = open(MSG_QUEUE_KEY_PATH, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("Failed to create key file");
        exit(1);
    }
    close(fd);

    key_t key = ftok(MSG_QUEUE_KEY_PATH, MSG_QUEUE_KEY_ID);
    if (key == -1) {
        perror("ftok failed");
        exit(1);
    }

    msg_queue_id = msgget(key, IPC_CREAT | 0666);
    if (msg_queue_id == -1) {
        perror("msgget failed");
        exit(1);
    }

    srand(time(NULL));

    log_file.open("logfile", std::ios::out | std::ios::trunc);
    if (!log_file) {
        cerr << "Failed to open logfile\n";
        exit(1);
    }
}


void Oss::launch_user() {
    // Fork a new user process
    if (active_users.size() >= MAX_ACTIVE) {
        cerr << "Maximum active users reached. Cannot launch more.\n";
        return;
    }
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child process
        execlp("./user", "user", NULL);
        perror("execlp failed");
        exit(1);
    } else {
        // Parent process
        active_users.push_back(pid);
        cout << "Launched user with PID: " << pid << endl;
    }
}

void Oss::handle_message() {
    Message msg;
    while (msgrcv(msg_queue_id, &msg, sizeof(Message) - sizeof(long), 1, IPC_NOWAIT) > 0) {
        int index = msg.pid % MAX_PROCESSES;
        int rid = msg.resource_id;

        switch (msg.action) {
            case 1: { // request
                log_file << "Master has detected Process P" << index
                         << " requesting R" << rid << " at time "
                         << clock->seconds << ":" << clock->nanoseconds << "\n";

                if (resource_table.request_resource(index, rid)) {
                    log_file << "Master granting P" << index
                             << " request R" << rid << " at time "
                             << clock->seconds << ":" << clock->nanoseconds << "\n";
                } else {
                    log_file << "Master: no instances of R" << rid
                             << " available, P" << index
                             << " added to wait queue at time "
                             << clock->seconds << ":" << clock->nanoseconds << "\n";
                    wait_queue[rid].push_back(index);
                }
                break;
            }

            case 2: { // release
                resource_table.release_resource(index, rid);
                log_file << "Master has acknowledged Process P" << index
                         << " releasing R" << rid << " at time "
                         << clock->seconds << ":" << clock->nanoseconds << "\n";
                log_file << "  Resources released : R" << rid << ":1\n";
                process_wait_queue(); // Check if any processes can be unblocked
                break;
            }

            case 3: { // terminate
                log_file << "Process P" << index << " terminated\n";
                log_file << "  Resources released: ";
                for (int r = 0; r < MAX_RESOURCES; ++r) {
                    int released = resource_table.get_resource_info(r)->allocation[index];
                    if (released > 0)
                        log_file << "R" << r << ":" << released << " ";
                }
                log_file << "\n";

                resource_table.release_all_resources(index);
                active_users.erase(std::remove(active_users.begin(), active_users.end(), msg.pid), active_users.end()); 
                process_wait_queue(); // Check if any processes can be unblocked
                break;
            }
        }

        // Unblock user
        msg.mtype = msg.pid;
        msgsnd(msg_queue_id, &msg, sizeof(Message) - sizeof(long), 0);
    }
}

void Oss::check_deadlock() {
    int deadlocked[MAX_PROCESSES];
    int count = detect_deadlock(resource_table, deadlocked);

    for (int i = 0; i < count; ++i) {
        pid_t victim = active_users[deadlocked[i]];
        kill(victim, SIGTERM);
        resource_table.release_all_resources(deadlocked[i]);
        active_users.erase(remove(active_users.begin(), active_users.end(), victim), active_users.end());

        cout << "Deadlock: Killed user with PID " << victim << "\n";
    }
}

void Oss::log_state() {
    log_file << "OSS State at " << clock->seconds << "s " << clock->nanoseconds << "ns\n";
    log_file << "Active Users: ";
    for (pid_t pid : active_users) {
        log_file << pid << " ";
    }
    log_file << "\n";
    resource_table.log_resources(log_file);
}



void Oss::run() {
    int total_spawned = 0;
    unsigned long long next_launch_ns = 0;
    unsigned long long next_deadlock_ns = 1000000000;
    unsigned long long next_log_ns = 500000000;

    while (total_spawned < MAX_USER || !active_users.empty()) {
        // Advance clock
        clock->nanoseconds += 10000;
        if (clock->nanoseconds >= 1000000000) {
            clock->seconds++;
            clock->nanoseconds -= 1000000000;
        }

        handle_message();

        unsigned long long current_ns = clock->seconds * 1000000000ULL + clock->nanoseconds;

        if (total_spawned < MAX_USER && active_users.size() < MAX_ACTIVE && current_ns >= next_launch_ns) {
            launch_user();
            total_spawned++;
            next_launch_ns = current_ns + 50000000; // next launch in 50ms
        }

        if (current_ns >= next_log_ns) {
            log_state();
            next_log_ns += 500000000;
        }

        if (current_ns >= next_deadlock_ns) {
            check_deadlock();
            next_deadlock_ns += 1000000000;
        }

        // Clean up any zombie children
        int status;
        while (waitpid(-1, &status, WNOHANG) > 0);

        usleep(10000); // sleep for realism
    }
}

void Oss::process_wait_queues() {
    for (int r = 0; r < MAX_RESOURCES; ++r) {
        auto& queue = wait_queue[r];
        for (auto it = queue.begin(); it != queue.end();) {
            int pid_index = *it;
            if (resource_table.request_resource(pid_index, r)) {
                log_file << "Master granting P" << pid_index
                         << " previously blocked request R" << r
                         << " at time " << clock->seconds << ":" << clock->nanoseconds << "\n";

                // Send grant message to unblock the user
                Message msg;
                msg.mtype = pid_index;  // mtype = pid_index to match msgrcv in user
                msg.pid = pid_index;
                msg.action = 1; // request granted
                msg.resource_id = r;
                msgsnd(msg_queue_id, &msg, sizeof(Message) - sizeof(long), 0);

                it = queue.erase(it); // remove from wait queue
            } else {
                ++it; // still cannot grant
            }
        }
    }
}

int main(int argc, char* argv[]) {
    int n_proc = 40;
    int max_seconds = 20;
    int launch_interval_ms = 500;
    std::string logfile_name = "logfile";
    
    int opt;
    while ((opt = getopt(argc, argv, "hn:s:i:f:")) != -1) {
        switch (opt) {
            case 'h':
                std::cout << "Usage: oss [-h] [-n proc] [-s simul] [-i intervalMs] [-f logfile]\n";
                return 0;
            case 'n': n_proc = atoi(optarg); break;
            case 's': max_seconds = atoi(optarg); break;
            case 'i': launch_interval_ms = atoi(optarg); break;
            case 'f': logfile_name = optarg; break;
            default:
                std::cerr << "Unknown option. Use -h for help.\n";
                return 1;
        }
    }

    Oss oss(n_proc, max_seconds, launch_interval_ms, logfile_name);
    oss.run();
    return 0;
}