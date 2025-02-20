// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-13

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-20

#include "Oss.h"


PCB pcb[MAX_PCB]; // Array of PCB structures
volatile sig_atomic_t timeout_flag = 0; // Flag to indicate timeout
volatile sig_atomic_t timer_tick = 0; // Flag to indicate timer tick



// Main function

int main(int argc, char** argv) {
    // Variables declaration
    int num_workers; // Number of workers
    int max_simul_workers; // Maximum number of simultaneous workers
    int time_limit; // Time limit for each worker
    int interval; // Interval to launch workers

    // Parse command line arguments
    parse_arguments(argc, argv, num_workers, max_simul_workers, time_limit, interval);

    signal(SIGALRM, signal_handler); // Set up signal handler for SIGALRM
    signal(SIGTERM, signal_handler); // Set up signal handler for SIGTERM
    alarm(60); // Set up alarm for 60 seconds

    setup_timer(interval); // Set up the timer

    srand(time(NULL)); // Seed the random number generator
    init_process_table(); // Initialize the process table

    Shared_Clock shared_clock(SHMKEY, true); // Create the shared clock
    Clock* clock = shared_clock.get_clock(); // Get the pointer to the shared clock structure
    if(clock == NULL) {
        cerr << "Failed to create shared clock." << endl;
        return 1;
    }

    cout << "OSS: Starting OSS loop..." << endl;

    int workers_launched = 0; // Number of workers launched
    int last_print_sec = clock->seconds; // Last printed second

    while(true) {
        if(timer_tick) {
            timer_tick = 0; // Reset the timer tick flag
            increment_clock(clock, INCREMENT_NS); // Increment the clock

            if(((clock->seconds - last_print_sec) >= 0) || (clock->nanoseconds >= (ONE_BILLION - INCREMENT_NS))) {
                print_process_table(clock); // Print the process table
                last_print_sec = clock->seconds; // Update the last printed second
            }

            check_terminated_workers(); // Check for terminated workers

            if(workers_launched < num_workers && count_running_workers() < max_simul_workers) {
                if(launch_worker(clock, time_limit)) {
                    workers_launched++; // Increment the number of workers launched
                }
            }

            if(workers_launched >= num_workers && count_running_workers() == 0) {
                cout << "OSS: All workers finished. Exiting..." << endl;
                break;
            }

            if(timeout_flag) {
                cout << "OSS: Timeout reached. Terminating remaining workers..." << endl;
                for(int i = 0; i < MAX_PCB; i++) {
                    if(pcb[i].occupied) {
                        kill(pcb[i].pid, SIGTERM);
                    }
                }
                break;
            } 

        }
    }

    shared_clock.remove_segment(); // Remove the shared clock
    cout << "OSS: All workers finished. Exiting OSS" << endl;
    return 0;

}

// Function definitions

/** @brief Signal handler for SIGUSR1 signal.
 *  @param signum The signal number.
 */
void signal_handler(int sig) {
    if(sig == SIGALRM) {
        timer_tick = 1;
    } else if(sig == SIGTERM) {
        timeout_flag = 1;
    }
}

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
                exit(1);
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
    clock->nanoseconds += increment_ns;
    if (clock->nanoseconds >= ONE_BILLION) {
        clock->seconds += clock->nanoseconds / ONE_BILLION;
        clock->nanoseconds %= ONE_BILLION;
    }
}

// Check for terminated workers
void check_terminated_workers() {
    int status;
    pid_t pid;
    while((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        for(int i = 0; i < MAX_PCB; i++) {
            if(pcb[i].occupied && (pcb[i].pid == pid)) {
                pcb[i].occupied = 0;
                pcb[i].pid = 0;
                pcb[i].start_seconds = 0;
                pcb[i].start_nanoseconds = 0;
                cout << "OSS: Worker PID " << pid << " terminated with exit code " << WEXITSTATUS(status) << "." << endl;
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
// string to_string(const int num) {
//     ostringstream oss;
//     oss << num;
//     return oss.str();
// }

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
        string sec_str = to_string(worker_seconds);
        string nano_str = to_string(worker_nanoseconds);
        // Execute the worker program with the arguments
        execlp("./worker", "./worker", sec_str.c_str(), nano_str.c_str(), (char*)NULL);
        cerr << "Exec failed" << endl;
        exit(1);
    } else { // Parent process: update process table
        pcb[slot].occupied = 1;
        pcb[slot].pid = pid;
        pcb[slot].start_seconds = clock->seconds;
        pcb[slot].start_nanoseconds = clock->nanoseconds;
        cout << "OSS: Launched worker PID " << pid << " in slot " << slot 
             << " (Runtime: " << worker_seconds << " sec, " << worker_nanoseconds << " ns)" << endl;
        return true;
    }
}

