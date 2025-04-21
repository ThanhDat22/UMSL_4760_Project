#include "User.h"



User::User() : clock(nullptr), shm_id(-1), msg_queue_id(-1) {
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

    cout << "USER " << getpid() << " initialized. Terminate at "
              << termination_sec << "s " << termination_ns << "ns\n";
}

bool User::check_termination() {
    return (clock->seconds > termination_sec) ||
           (clock->seconds == termination_sec && clock->nanoseconds >= termination_ns);
}

void User::request_resource() {
    Message msg;
    msg.mtype = 1;
    msg.pid = getpid();
    msg.action = 1; // MSG_TYPE_REQUEST
    msg.resource_id = rand() % MAX_RESOURCES;

    msgsnd(msg_queue_id, &msg, sizeof(Message) - sizeof(long), 0);
    wait_for_grant();
}

void User::release_resource() {
    Message msg;
    msg.mtype = 1;
    msg.pid = getpid();
    msg.action = 2; // MSG_TYPE_RELEASE
    msg.resource_id = rand() % MAX_RESOURCES;

    msgsnd(msg_queue_id, &msg, sizeof(Message) - sizeof(long), 0);
}

void User::send_termination() {
    Message msg;
    msg.mtype = 1;
    msg.pid = getpid();
    msg.action = 3; // MSG_TYPE_TERMINATE
    msg.resource_id = -1;

    msgsnd(msg_queue_id, &msg, sizeof(Message) - sizeof(long), 0);
}

void User::wait_for_grant() {
    Message response;
    msgrcv(msg_queue_id, &response, sizeof(Message) - sizeof(long), getpid(), 0);
    // Optionally check response.action
}

void User::run() {
    while (true) {
        usleep(10000); // short delay to simulate computation

        if (check_termination()) {
            send_termination();
            break;
        }

        int decision = rand() % 10;
        if (decision < 5) {
            request_resource();
        } else if (decision < 8) {
            release_resource();
        } // else do nothing this round
    }

    detach_clock(clock);
}

