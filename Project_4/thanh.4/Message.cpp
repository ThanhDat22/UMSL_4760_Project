#include "Message.h"

int msg_queue_id; // Message queue ID
extern PCB pcb[MAX_PCB]; // Process Control Block array
// Function definitions

// Create a message queue
void create_message_queue() {
    key_t key = ftok("msgq.txt", 1);
    msg_queue_id = msgget(key, IPC_CREAT | 0666);
    if (msg_queue_id < 0) {
        perror("msgget failed");
        exit(1);
    }
    cout << "Message queue created with ID: " << msg_queue_id << endl;
}

// Receive a message from the queue
void receive_message() {
    Message msg; 
    memset(&msg, 0, sizeof(msg));
    
    if (msgrcv(msg_queue_id, &msg, sizeof(Message), MSG_TYPE_FROM_WORKER, 0) == -1) {
        perror("msgrcv failed");
    } else {
        cout << "OSS: Received response from worker " << msg.worker_id 
                  << " with status " << msg.command << endl;
    }

}

// Send a message to a worker
void send_message(int worker_id, int command) {
    Message msg;
    memset(&msg, 0, sizeof(msg)); // Initialize the message structure to zero
    msg.mtype = MSG_TYPE_TO_WORKER;
    msg.worker_id = pcb[worker_id].pid;
    msg.command = command;

    msg.seconds = 0;
    msg.nanoseconds = 0;

    while (true) {
        if (msgsnd(msg_queue_id, &msg, sizeof(msg) - sizeof(msg.mtype), 0) == -1) {
            if (errno == EINTR) {
                continue; // Retry if interrupted by a signal
            }
            perror("msgsnd failed");
            break; // Exit the loop on other errors
        } else {
            cout << "OSS: Sent message to worker " << worker_id 
                 << " with command " << command << endl;
            pcb[worker_id].messages_sent++;
            break; // Success – exit the loop
        }
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