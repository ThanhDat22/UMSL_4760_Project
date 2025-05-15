#include "User.h"



User::User() : clock(NULL), shm_id(-1), msg_queue_id(-1) {
    srand(getpid() + time(NULL));
    pid_index = getpid() % MAX_PROCESSES; // map PID to process table index (simple)
    init();
}

void User::init() {
    clock = attach_clock(&shm_id, 0); // attach to existing shared clock
    if (!clock) {
        cerr << "User failed to attach to clock\n";
        exit(1);
    }

    //std::cout << "[DEBUG] User attached to shared clock at address: " << clock << "\n";

    // Ensure key file exists for ftok
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

    msg_queue_id = msgget(key, 0666);
    if (msg_queue_id == -1) {
        perror("msgget failed");
        exit(1);
    }

    // set random termination time (1–5 seconds from now)
    termination_sec = clock->seconds + (rand() % 5 + 1);
    termination_ns = clock->nanoseconds + (rand() % 1000000);
    if (termination_ns >= 1000000000) {
        termination_sec++;
        termination_ns -= 1000000000;
    }

    std::cout << "[DEBUG] User " << getpid() << " will terminate at: " 
          << termination_sec << "s " << termination_ns << "ns\n";

    cout << "USER " << getpid() << " initialized. Terminate at "
              << termination_sec << "s " << termination_ns << "ns\n";
}

bool User::check_termination() {
    bool ter = (clock->seconds > termination_sec) ||
                            (clock->seconds == termination_sec && clock->nanoseconds >= termination_ns);

    std::cout << "[DEBUG] User " << getpid() << " is checking termination at time "
              << clock->seconds << ":" << clock->nanoseconds 
              << " | Terminate at: " << termination_sec << ":" << termination_ns << " -> " 
              << (ter ? "Yes" : "No") << std::endl;
    
    return ter;
}

void User::request_memory(int address, bool is_write) {
    Message msg;
    msg.mtype = 1;
    msg.pid = getpid();
    msg.action = 1;                  // MSG_TYPE_REQUEST
    msg.memory_address = address;    // Requested address
    msg.operation = is_write ? 1 : 0;

    cout << "USER " << getpid() << " requesting " 
         << (is_write ? "WRITE" : "READ") 
         << " access to address " << address << "\n";

    // Send the request to the message queue
    msgsnd(msg_queue_id, &msg, sizeof(Message) - sizeof(long), 0);
    wait_for_grant();
}


void User::wait_for_grant() {
    Message response;
    msgrcv(msg_queue_id, &response, sizeof(Message) - sizeof(long), getpid(), 0);
    cout << "USER " << getpid() << " received grant for address "
         << response.memory_address << endl;
}

void User::run() {
    while (true) {
        usleep(10000); // short delay to simulate computation

        if (check_termination()) {
            std::cout << "[DEBUG] User " << getpid() << " is sending termination message.\n";
            send_termination();
            break;
        }

        // Generate a random address and a random operation
        int address = generate_random_address();
        bool is_write = (rand() % 2) == 1;

        // Request memory access (READ or WRITE)
        request_memory(address, is_write);
    }

    detach_clock(clock);
}

void User::send_termination() {
    Message msg;
    msg.mtype = 1;
    msg.pid = getpid();
    msg.action = 3; // MSG_TYPE_TERMINATE
    msg.memory_address = -1;

   if (msgsnd(msg_queue_id, &msg, sizeof(Message) - sizeof(long), 0) == -1) {
        perror("msgsnd failed");
        exit(1);
    }
    std::cout << "[DEBUG] User " << getpid() << " sent termination message.\n";
}

int User::generate_random_address() {
    int max_address = TOTAL_FRAMES * PAGE_SIZE - PAGE_SIZE;
    int address = (rand() % max_address);

    int page_aligned_address = (address / PAGE_SIZE) * PAGE_SIZE;
    
    //std::cout << "[DEBUG] Generated Address: " << page_aligned_address << " (Max allowed: " << max_address << ")\n";

    return page_aligned_address;
}

int main() {
    User user;
    user.run();
    return 0;
}