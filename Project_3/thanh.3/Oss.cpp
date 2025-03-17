// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-13

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-03-12

#include "Oss.h"
#include "Pcb.h"         
#include "Message.h"       

volatile sig_atomic_t timeout_flag = 0; // Flag to indicate timeout
volatile sig_atomic_t timer_tick = 0; // Flag to indicate timer tick
Message buf; // Message buffer
ofstream fout; // Log file
string log_file = "logfile";
//PCB pcb[MAX_PCB]; // Array of PCB structures

int next_worker_index = 0;

int scheduling_queue[MAX_PCB]; // Queue for scheduling workers
int queue_size = 0; // Size of the scheduling queue
int queue_front = 0; // Front of the queue
int queue_rear = 0; // Rear of the queue

int priority_queue[MAX_PCB]; // Queue for worker priorities
int priority_queue_size = 0; // Size of the priority queue
Shared_Clock shared_clock(SHMKEY, true);

// Main function

int main(int argc, char** argv) {
    // Variables declaration
    int num_workers; // Number of workers
    int max_simul_workers; // Maximum number of simultaneous workers
    int time_limit; // Time limit for each worker
    int interval; // Interval to launch workers

    // Parse command line arguments
    parse_arguments(argc, argv, num_workers, max_simul_workers, time_limit, interval);

    // Register signal handlers
    signal(SIGALRM, signal_handler); // Set up signal handler for SIGALRM
    signal(SIGINT, signal_handler); // Handle Ctrl + C

    alarm(60); // Set up alarm for 60 seconds

    // Initialize the shared clock
    Shared_Clock shared_clock(SHMKEY, true); // Create the shared clock
    Clock* clock = shared_clock.get_clock(); // Get the pointer to the shared clock structure
    if(clock == NULL) {
        cerr << "Failed to create shared clock." << endl;
        exit(1);
    }

    init_process_table(); // Initialize the process table
    create_message_queue(); // Create the message queue

    srand(time(NULL)); // Seed the random number generator


    setup_timer(interval); // Set up the timer

    cout << "OSS: Starting OSS loop..." << endl;

    int workers_launched = 0; // Number of workers launched
    int last_print_sec = clock->seconds; // Last printed second
    int last_print_ns = clock->nanoseconds; // Last printed nanosecond

    // Main loop
    while (true) {
        increment_clock(clock, 250000000);

        if ((clock->nanoseconds - last_print_ns) >= (500 * INCREMENT_NS) || 
            (clock->seconds - last_print_sec) > 0) {
            print_worker_stats();
            log_worker_stats();
            last_print_sec = clock->seconds;
            last_print_ns = clock->nanoseconds;
        }

        check_terminated_workers();

        // Launch workers based on scheduling
        if (workers_launched < num_workers && count_running_workers() < max_simul_workers) {
            if (launch_worker(clock, time_limit)) {
                workers_launched++;
            }
        }

        if (priority_queue_size > 0) {
            schedule_workers();
        }

        if (workers_launched >= num_workers && count_running_workers() == 0) {
            cout << "OSS: All workers finished. Exiting...\n";
            break;
        }

        if (timeout_flag) {
            cout << "OSS: Timeout reached. Terminating remaining workers...\n";
            kill_workers();
            break;
        }
    }

    cleanup_and_exit(); // Clean up and exit the program
    return 0;

}


// Function definitions

/** @brief Sets up a timer to send SIGALRM signal at specified intervals.
 *  @param interval_ms The time interval in milliseconds.
 */
void setup_timer(int interval_ms) {
    struct itimerval timer;
    timer.it_interval.tv_sec = interval_ms / 1000;
    timer.it_interval.tv_usec = (interval_ms % 1000) * 1000;
    timer.it_value = timer.it_interval;
    setitimer(ITIMER_REAL, &timer, NULL);
}

/** @brief Parses command line arguments.
 *  @param argc The number of command line arguments.
 *  @param argv The array of command line arguments.
 *  @param num_workers The number of worker processes to launch.
 *  @param max_simul_workers The maximum number of worker processes to allow to run simultaneously.
 *  @param time_limit The maximum time limit in seconds for each worker process.
 *  @param interval The time interval in milliseconds to launch user processes.
 */
void parse_arguments(int argc, char** argv, int& num_workers, int& max_simul_workers, int& time_limit, int& interval) {
    int options;
    num_workers = 0;
    max_simul_workers = 0;
    time_limit = 0;
    interval = 0;

    // Check for -h anywhere in the arguments
    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "-h") == 0) {
            print_usage();
            exit(0);
        }
    }

    while((options = getopt(argc, argv, "hn:s:t:i:")) != -1) {
        switch(options) {
            case 'n':              
                if(!is_number(optarg)) {
                    cout << "Invalid number of workers" << endl;
                    exit(1);
                }
                num_workers = atoi(optarg);
                break;
            case 's':
                if(!is_number(optarg)) {
                    cout << "Invalid number of simultaneous workers" << endl;
                    exit(1);
                }
                max_simul_workers = atoi(optarg);
                break;
            case 't':
                if(!is_number(optarg)) {
                    cout << "Invalid time limit" << endl;
                    exit(1);
                }
                time_limit = atoi(optarg);
                break;
            case 'i':
                if(!is_number(optarg)) {
                    cout << "Invalid interval" << endl;
                    exit(1);
                }
                interval = atoi(optarg);
                break;
            default:
                print_usage();
                exit(0);
        }
    }
    if(num_workers <= 0 || max_simul_workers <= 0 || time_limit <= 0 || interval <= 0) {
        cout << "Invalid arguments" << endl;
        exit(1);
    }
}

/** @brief Checks if a string is a number.
 *  @param str The string to check.
 *  @return True if the string is a number, false otherwise.
 */
bool is_number(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) return false;
    }
    return true;
}

// Print usage information
void print_usage() {
    cout << "Usage: ./oss [-h] [-n proc] [-s simul] [-t timelimitForChildren] [-i intervalInMsToLaunchChildren]" << endl;
    cout << "[-h] Display this help message." << endl;
    cout << "[-n proc] Stands for the number of worker processes to launch." << endl;
    cout << "[-s simul] Indicates the maximum number of worker processes to allow to run simultaneously." << endl;
    cout << "[-t timelimitForChildren] Indicates the maximum time limit in seconds for each worker process." << endl;
    cout << "[-i intervalInMsToLaunchChildren] Indicates the time interval in milliseconds to launch user processes." << endl;
}

// Initialize the process table
void init_process_table() {
    for(int i = 0; i < MAX_PCB; i++) {
        pcb[i].occupied = 0;
        pcb[i].pid = 0;
        pcb[i].start_seconds = 0;
        pcb[i].start_nanoseconds = 0;
    }
}

// Print the process table
void print_process_table(Clock* clock) {
    cout << "OSS PID:" << getpid()
         << " SysClockS:" << clock->seconds
         << " SysClockNano:" << clock->nanoseconds << endl;
    cout << "Process Table:" << endl;
    cout << "Entry\tOccupied\tPID\tStartS\tStartN" << endl;
    for (int i = 0; i < MAX_PCB; i++) {
        cout << i << "\t" <<pcb[i].occupied << "\t\t" 
             << pcb[i].pid << "\t" 
             << pcb[i].start_seconds << "\t" 
             << pcb[i].start_nanoseconds << endl;
    }
}


/** @brief Increments the clock by a specified number of nanoseconds.
 *  @param clock Pointer to the shared clock structure.
 *  @param increment_ns Number of nanoseconds to increment.
 */
void increment_clock(Clock* clock, int increment_ns) {
    int running_workers = count_running_workers();
    int time_increment = (running_workers > 0) ? (250000000 / running_workers) : 250000000;
    
    clock->nanoseconds += time_increment;
    if (clock->nanoseconds >= ONE_BILLION) {
        clock->seconds++;
        clock->nanoseconds -= ONE_BILLION;
    }
}

// Check for terminated workers
void check_terminated_workers() {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        for (int i = 0; i < MAX_PCB; i++) {
            if (pcb[i].occupied && pcb[i].pid == pid) {
                cout << "OSS: Worker " << pid << " terminated.\n";
                pcb[i].occupied = 0;
                pcb[i].pid = 0;
                pcb[i].messages_sent = 0;
                pcb[i].messages_received = 0;
                break;
            }
        }
    }
}

/** @brief Counts the number of running workers.
 *  @return The number of running workers.
 */
int count_running_workers() {
    int count = 0;
    for(int i = 0; i < MAX_PCB; i++) {
        if(pcb[i].occupied) {
            count++;
        }
    }
    return count;
}

/** @brief Converts an integer to a string.
 *  @param num The integer to convert.
 *  @return The string representation of the integer.
 */
string int_to_string(const int num) {
    ostringstream oss;
    oss << num;
    return oss.str();
}

/** @brief Launches a worker process.
 *  @param clock Pointer to the shared clock structure.
 *  @param time_upper_bound The upper bound for the worker's runtime in seconds.
 *  @return True if the worker was launched successfully, false otherwise.
 */
bool launch_worker(Clock* clock, int time_upper_bound) {


    // Find a free slot in the process table.
    int slot = -1;
    for (int i = 0; i < MAX_PCB; i++) {
        if (!pcb[i].occupied) {
            slot = i;
            break;
        }
    }
    if (slot == -1) {
        return false; // No free slot available.
    }

    // Randomize worker runtime
    int worker_seconds = 1 + rand() % time_upper_bound;
    int worker_nanoseconds = rand() % ONE_BILLION;

    pid_t pid = fork();
    if (pid < 0) {
        cerr << "Fork failed" << endl;
        return false;
    } else if (pid == 0) { // Child process: execute worker
        
        // Convert seconds and nanoseconds to strings
        string sec_str = int_to_string(worker_seconds);
        string nano_str = int_to_string(worker_nanoseconds);
        // Execute the worker program with the arguments
        execlp("./worker", "./worker", sec_str.c_str(), nano_str.c_str(), (char*)NULL);
        cerr << "Exec failed" << endl;
        exit(1);
    } else { // Parent process: update process table
        pcb[slot].occupied = 1;
        pcb[slot].pid = pid;
        pcb[slot].start_seconds = clock->seconds;
        pcb[slot].start_nanoseconds = clock->nanoseconds;

        pcb[slot].messages_sent = 0;
        pcb[slot].messages_received = 0;
        pcb[slot].total_runtime_sec = 0;
        pcb[slot].total_runtime_ns = 0;

        pcb[slot].priority = rand() % 5 + 1;
        insert_into_priority_queue(slot);

        cout << "OSS: Launched worker PID " << pid << " in slot " << slot 
             << " (Runtime: " << worker_seconds << " sec, " << worker_nanoseconds 
             << " ns, Priority: " << pcb[slot].priority << ")" << endl;
        return true;
    }
}

/** @brief Enqueues a worker process to the process table.
 *  @param pid The PID of the worker process to enqueue.
 */
 void enqueue_worker(int worker_id) {
    if (queue_size < MAX_PCB) {
        scheduling_queue[queue_rear] = worker_id;
        queue_rear = (queue_rear + 1) % MAX_PCB; // Circular increment
        queue_size++;
        cout << "OSS: Enqueued worker PID " << worker_id << " to the scheduling queue." << endl;
    }
 }

// @brief Dequeues a worker process from the scheduling queue.
 int dequeue_worker() {
    if (queue_size > 0) {
        int worker_id = scheduling_queue[queue_front];
        queue_front = (queue_front + 1) % MAX_PCB; // Circular increment
        queue_size--;
        return worker_id;
    } else {
        return -1; // Queue is empty
    }
 }

/** @brief Peeks at the next worker in the scheduling queue without removing it.
 *  @return The PID of the next worker in the queue, or -1 if the queue is empty.
 */
 int peek_worker() {
    if (queue_size > 0) {
        return scheduling_queue[queue_front]; // Return the next worker's PID
    } else {
        return -1; // Queue is empty
    }
 }

//Schedules workers based on the scheduling queue.
void schedule_workers() {
    if (priority_queue_size > 0) {
        int worker_id = pop_from_priority_queue();

        send_message(worker_id, 1); // Send work command
        receive_message();

        if (pcb[worker_id].occupied) {
            insert_into_priority_queue(worker_id);
        }
    }
}

// Prints the statistics of all workers in the process table.
void print_worker_stats() {
    cout << "\n=== Worker Stats ===\n";
    cout << "ID\tPriority\tMessages Sent\tMessages Received\tTotal Runtime (s.ns)\n";
    for (int i = 0; i < MAX_PCB; i++) {
        if (pcb[i].occupied) {
            std::cout << i << "\t"
                      << pcb[i].priority << "\t\t"
                      << pcb[i].messages_sent << "\t\t"
                      << pcb[i].messages_received << "\t\t"
                      << pcb[i].total_runtime_sec << "."
                      << pcb[i].total_runtime_ns << "\n";
        }
    }
    cout << "====================\n";
}

void log_worker_stats() {

    ofstream log(log_file.c_str(), std::ios::app);

    if (log.is_open()) {
        log << "\n=== Worker Stats ===\n";
        log << "ID\tPriority\tMessages Sent\tMessages Received\tTotal Runtime (s.ns)\n";
        for (int i = 0; i < MAX_PCB; i++) {
            if (pcb[i].occupied) {
                log << i << "\t"
                    << pcb[i].priority << "\t\t"
                    << pcb[i].messages_sent << "\t\t"
                    << pcb[i].messages_received << "\t\t"
                    << pcb[i].total_runtime_sec << "."
                    << pcb[i].total_runtime_ns << "\n";
            }
        }
        log.close();
    }
}

/** @brief Signal handler for SIGALRM and SIGTERM signals.
 *  @param sig The signal number.
 */
void signal_handler(int sig) {
    if (sig == SIGALRM) {
        timer_tick = 1; // Set the timer tick flag
    } else if (sig == SIGTERM || sig == SIGINT) {
        cout << "\nOSS: Caught termination signal. Cleaning up...\n";
        cleanup_and_exit(); // Clean up and exit
        exit(0);
    }
}

// End worker processes
void kill_workers() {
    for (int i = 0; i < MAX_PCB; i++) {
        if (pcb[i].occupied) {
            cout << "OSS: Killing worker PID " << pcb[i].pid << endl;
            kill(pcb[i].pid, SIGTERM);
            waitpid(pcb[i].pid, NULL, 0); // Clean up zombie processes
            pcb[i].occupied = 0;
        }
    }
}

// Clean up and exit the program
void cleanup_and_exit() {
    cout << "OSS: Cleaning up..." << endl;
    kill_workers();
    shared_clock.remove_segment(); // Remove the shared clock
    cleanup_message_queue(); // Clean up the message queue
    print_worker_stats(); // Print final worker stats
    log_worker_stats(); // Log final worker stats
    cout << "OSS: Cleaning up and exiting..." << endl;
}

// Insert a worker into the priority queue based on its priority
void insert_into_priority_queue(int worker_id) {
    if (priority_queue_size < MAX_PCB) {
        int i = priority_queue_size;

        // Insert at the end and bubble up based on priority
        while (i > 0 && pcb[priority_queue[(i - 1) / 2]].priority > pcb[worker_id].priority) {
            priority_queue[i] = priority_queue[(i - 1) / 2];
            i = (i - 1) / 2;
        }

        priority_queue[i] = worker_id;
        priority_queue_size++;
    }
}

// Pop the worker with the highest priority from the priority queue
int pop_from_priority_queue() {
    if (priority_queue_size == 0) {
        return -1; // Queue is empty
    }

    int result = priority_queue[0];
    priority_queue_size--;

    // Restore heap order
    int last = priority_queue[priority_queue_size];
    int i = 0;
    while (i * 2 + 1 < priority_queue_size) {
        int child = i * 2 + 1;
        if (child + 1 < priority_queue_size && 
            pcb[priority_queue[child + 1]].priority < pcb[priority_queue[child]].priority) {
            child++;
        }
        if (pcb[last].priority <= pcb[priority_queue[child]].priority) {
            break;
        }
        priority_queue[i] = priority_queue[child];
        i = child;
    }

    priority_queue[i] = last;
    return result;
}

// Peek at the worker with the highest priority in the priority queue without removing it
int peek_priority_queue() {
    if (priority_queue_size == 0) {
        return -1;
    }
    return priority_queue[0];
}


