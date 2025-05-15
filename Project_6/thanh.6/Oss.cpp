#include "Oss.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <csignal>
#include <algorithm>
#include <fcntl.h>

Shared_Clock* shared_clock = nullptr;

Oss::Oss(int max_users, int max_seconds, int launch_interval, const string& logfile_name)
    : msg_q_id(-1), shm_id(-1), shared_clock(NULL),
      max_user(max_users), max_simulation_seconds(max_seconds),
      launch_interval_ns(launch_interval * 1000000ULL), log_file_name(logfile_name) {

    // Open log file
    log_file.open(log_file_name.c_str(), ios::out | ios::trunc);
    if (!log_file.is_open()) {
        cerr << "Failed to open log file: " << log_file_name << endl;
        exit(1);
    }  
    

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
    shared_clock = attach_clock(&shm_id, 1); // attach to existing shared clock
    if (!shared_clock) {
        cerr << "OSS failed to attach to clock\n";
        exit(1);
    } else {
        std::cout << "[DEBUG] OSS attached to shared clock at address: " << shared_clock << "\n";
    }

    frame_table.set_shared_clock(shared_clock); // Link frame table to shared clock

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

    msg_q_id = msgget(key, IPC_CREAT | 0666);
    if (msg_q_id == -1) {
        perror("msgget failed");
        exit(1);
    }

    srand(time(NULL));

    
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
    ssize_t result = msgrcv(msg_q_id, &msg, sizeof(Message) - sizeof(long), 1, IPC_NOWAIT);

    if (result == -1) {
        if (errno == ENOMSG) {
            // No message in queue — not an error
            return;
        } else {
            perror("msgrcv failed");
            exit(1);
        }
    }

    int index = msg.pid % MAX_PROCESSES;  // User index
    int page_number = msg.memory_address / PAGE_SIZE;  // Calculate the page number
    bool is_write = (msg.operation == 1); // 1 = write, 0 = read

    std::cout << "[DEBUG] OSS received address: " << msg.memory_address << "\n";
    std::cout << "[DEBUG] Calculated page number: " << page_number << "\n";

    if (msg.memory_address < 0 || msg.memory_address >= TOTAL_FRAMES * PAGE_SIZE) {
        std::cerr << "ERROR: Memory address " << msg.memory_address << " is out of bounds!\n";
        exit(1);
    }

    // Log the memory access request
    log_file << "Master has detected Process P" << index
             << " requesting " << (is_write ? "WRITE" : "READ")
             << " access to page " << page_number
             << " at time " << shared_clock->seconds << ":" 
             << setw(9) << setfill('0') << shared_clock->nanoseconds << endl;

    // Request a frame from the Frame Table (handles LRU if needed)
    int frame = frame_table.request_frame(index, page_number, is_write);

    if (frame != -1) {
        // Log successful allocation
        log_file << "Master granted access to P" << index
                 << " for page " << page_number << " at frame " << frame
                 << " at time " << shared_clock->seconds << ":"
                 << setw(9) << setfill('0') << shared_clock->nanoseconds << endl;
    } else {
        // If no frame available, we have a serious memory issue
        log_file << "ERROR: Master could not allocate frame for P" << index
                 << " requesting page " << page_number
                 << " at time " << shared_clock->seconds << ":"
                 << setw(9) << setfill('0') << shared_clock->nanoseconds << endl;;
    }

    // Prepare the response message
    if (frame_table.evict_occurred()) {
        auto evicted = frame_table.get_last_eviction();
        log_file << "Page Eviction: Evicted Process P" << evicted.pid
                 << ", Page " << evicted.page_number
                 << " from Frame " << evicted.frame_index
                 << " at time " << shared_clock->seconds << ":"
                 << setw(9) << setfill('0') << shared_clock->nanoseconds << "\n";
    }

    msg.mtype = msg.pid;
    msg.memory_address = frame * PAGE_SIZE;
    msg.operation = is_write ? 1 : 0;

    if (msgsnd(msg_q_id, &msg, sizeof(Message) - sizeof(long), 0) == -1) {
        perror("msgsnd failed");
        exit(1);
    }
}


void Oss::log_state() {
    log_file << "================== OSS State ==================\n";
    log_file << "Time: " << shared_clock->seconds << "s " << shared_clock->nanoseconds << "ns\n";

    // Display Active Users
    log_file << "Active Users: ";
    for (pid_t pid : active_users) {
        log_file << pid << " ";
    }
    log_file << "\n";

    // Display the Frame Table status
    log_file << "------------- Frame Table State --------------\n";
    for (int i = 0; i < TOTAL_FRAMES; ++i) {
        const Frame& frame = frame_table.get_frame(i);
        if (frame.pid != -1) {
            log_file << "Frame " << i 
                     << " | PID: " << frame.pid 
                     << " | Page: " << frame.page_number 
                     << " | Dirty: " << frame.dirty_bit 
                     << " | Last Access: " << frame.last_access_time << "\n";
        }
    }
    log_file << "=============================================\n\n";
}



void Oss::run() {
    int total_spawned = 0;
    unsigned long long next_launch_ns = 0;
    unsigned long long next_log_ns = 500000000; // Log every 0.5s

    while (total_spawned < MAX_USER || !active_users.empty()) {

        shared_clock->nanoseconds += 10000;
        if (shared_clock->nanoseconds >= 1000000000) {
            shared_clock->seconds++;
            shared_clock->nanoseconds -= 1000000000;
        }

        handle_message();

        unsigned long long current_ns = shared_clock->seconds * 1000000000ULL + shared_clock->nanoseconds;

        if (total_spawned < MAX_USER && active_users.size() < MAX_ACTIVE && current_ns >= next_launch_ns) {
            launch_user();
            total_spawned++;
            next_launch_ns = current_ns + 50000000; // Launch next user in 50ms
        }

        if (current_ns >= next_log_ns) {
            log_state();
            frame_table.display_frame_table(); // Display the current frame table
            next_log_ns += 500000000;
        }

        int status;
        while (waitpid(-1, &status, WNOHANG) > 0);

        usleep(10000); // sleep for realism
    }

    cleanup();
}

void Oss::process_wait_queues() {
    for (int p = 0; p < MAX_PROCESSES; ++p) {
        auto& queue = wait_queue[p];

        for (auto it = queue.begin(); it != queue.end();) {
            bool is_write = it->first;        // First value in the pair is the write flag
            int memory_address = it->second;  // Second value in the pair is the memory address
            int page_number = memory_address / PAGE_SIZE;

            // Try to allocate a frame for this process
            int frame = frame_table.request_frame(p, page_number, is_write);

            if (frame != -1) {
                // Log successful allocation
                log_file << "Master granting P" << p
                         << " access to page " << page_number 
                         << " at frame " << frame
                         << " at time " << shared_clock->seconds << ":"
                         << std::setw(9) << std::setfill('0') << shared_clock->nanoseconds << "\n";

                // Send grant message to unblock the user
                Message msg;
                msg.mtype = p;  // mtype = pid_index to match msgrcv in user
                msg.pid = p;
                msg.action = 1; // request granted
                msg.memory_address = page_number * PAGE_SIZE;
                msg.operation = is_write ? 1 : 0;

                // Send the message back to the user
                if (msgsnd(msg_q_id, &msg, sizeof(Message) - sizeof(long), 0) == -1) {
                    perror("msgsnd failed");
                    exit(1);
                }

                it = queue.erase(it); // Remove from wait queue
            } else {
                ++it; // Still cannot grant, move to next
            }
        }
    }
}

int main(int argc, char* argv[]) {
    int n_proc = 40;
    int max_seconds = 20;
    int launch_interval_ms = 500;
    string logfile_name = "logfile";
    
    int opt;
    while ((opt = getopt(argc, argv, "hn:s:i:f:")) != -1) {
        switch (opt) {
            case 'h':
                cout << "Usage: oss [-h] [-n proc] [-s simul] [-i intervalMs] [-f logfile]\n";
                return 0;
            case 'n': n_proc = atoi(optarg); break;
            case 's': max_seconds = atoi(optarg); break;
            case 'i': launch_interval_ms = atoi(optarg); break;
            case 'f': logfile_name = optarg; break;
            default:
                cerr << "Unknown option. Use -h for help.\n";
                return 1;
        }
    }

    Oss oss(n_proc, max_seconds, launch_interval_ms, logfile_name);
    oss.run();
    return 0;
}

void Oss::cleanup() {
    // Detach shared clock
    if (shared_clock) {
        detach_clock(shared_clock);
        shared_clock = NULL;
    }

    // Remove shared memory segment
    if (shm_id != -1) {
        shmctl(shm_id, IPC_RMID, NULL);
        shm_id = -1;
    }

    // Remove message queue
    if (msg_q_id != -1) {
        msgctl(msg_q_id, IPC_RMID, NULL);
        msg_q_id = -1;
    }

    // Terminate any still-active user processes
    for (pid_t pid : active_users) {
        kill(pid, SIGTERM);
        waitpid(pid, NULL, 0); // Clean up
    }
    active_users.clear();

    log_file << "OSS cleanup complete at time "
             << shared_clock->seconds << ":" << shared_clock->nanoseconds << "\n";
}