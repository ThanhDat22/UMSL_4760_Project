// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-13

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-03-13

#include "Worker.h"


// Main function
int main(int argc, char** argv) {

    if (argc < 3) {
        print_usage();
        return 1;
    }

    int max_seconds = 0;
    int max_nanoseconds = 0;
    parse_arguments(argc, argv, max_seconds, max_nanoseconds);

    // Attach to the shared clock
    Shared_Clock shared_clock(SHMKEY, false); // Create the shared clock
    Clock* clock = shared_clock.get_clock();
    if (clock == NULL) {
        cerr << "Failed to attach to shared clock." << endl;
        return 1;
    }

    key_t key = ftok("oss.cpp", 65);
    int msqid = msgget(key, 0644);

    msg_buffer buf; // Message buffer

    if (msgrcv(msqid, &buf, sizeof(buf) - sizeof(long), getpid(), 0) != -1) {
        cout << "Worker received: " << buf.str_data << ", Data: " << buf.int_data << endl;

        buf.mtype = getppid();
        strcpy(buf.str_data, "Worker done");
        buf.int_data = 0;

        if (msgsnd(msqid, &buf, sizeof(buf) - sizeof(long), 0) == -1) {
            perror("msgsnd");
        }
    }

    return 0;

}

