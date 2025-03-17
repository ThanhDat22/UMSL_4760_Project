#include "Message.h"

int msg_queue_id; // Message queue ID
extern PCB pcb[MAX_PCB]; // Process Control Block array
// Function definitions

// Create a message queue
void create_message_queue() {
    key_t key = ftok(".", 'M');
    msg_queue_id = msgget(key, IPC_CREAT | 0666);
    if (msg_queue_id < 0) {
        perror("msgget failed");
        exit(1);
    }
    cout << "Message queue created with ID: " << msg_queue_id << endl;
}

// Receive a message from the queue
void receive_message() {
    Message msg = {0}; 

    if (msgrcv(msg_queue_id, &msg, sizeof(Message), MSG_TYPE_FROM_WORKER, 0) == -1) {
        perror("msgrcv failed");
    } else {
        cout << "OSS: Received response from worker " << msg.worker_id 
                  << " with status " << msg.command << endl;

        pcb[msg.worker_id].messages_received++; // Increment the received message count for the worker

        int elapsed_seconds = msg.seconds - pcb[msg.worker_id].start_seconds;
        int elapsed_nanoseconds = msg.nanoseconds - pcb[msg.worker_id].start_nanoseconds;

        // Handle overflow of nanoseconds
        if (elapsed_nanoseconds < 0) {
            elapsed_seconds -= 1;
            elapsed_nanoseconds += ONE_BILLION;
        }

        pcb[msg.worker_id].total_runtime_sec += elapsed_seconds;
        pcb[msg.worker_id].total_runtime_ns += elapsed_nanoseconds;
    }
}

// Send a message to a worker
void send_message(int worker_id, int command) {
    Message msg;
    msg.mtype = MSG_TYPE_TO_WORKER;
    msg.worker_id = worker_id;
    msg.command = command;

    msg.seconds = 0;
    msg.nanoseconds = 0;

    if (msgsnd(msg_queue_id, &msg, sizeof(msg) - sizeof(msg.mtype), 0) == -1) {
        perror("msgsnd failed");
    } else {
        cout << "OSS: Sent message to worker " << worker_id 
                  << " with command " << command << endl;
        pcb[worker_id].messages_sent++; // Increment the sent message count for the worker
    }
}

// Remove the message queue
void cleanup_message_queue() {
    if (msgctl(msg_queue_id, IPC_RMID, NULL) == -1) {
        perror("msgctl failed");
    } else {
        cout << "OSS: Message queue removed." << endl;
    }
}