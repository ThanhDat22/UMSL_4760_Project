// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-13

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-20

#include "Oss.h"

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
        exit(1);
    }

    cout << "OSS: Starting OSS loop..." << endl;

    int workers_launched = 0; // Number of workers launched
    int last_print_sec = clock->seconds; // Last printed second
    int last_print_ns = clock->nanoseconds; // Last printed nanosecond

    while(true) {
        if(timer_tick) {
            timer_tick = 0; // Reset the timer tick flag
            increment_clock(clock, INCREMENT_NS); // Increment the clock

            if((clock->seconds - last_print_sec) > 0 || (clock->nanoseconds - last_print_ns) >= (500 * INCREMENT_NS)) {
                print_process_table(clock); // Print the process table
                last_print_sec = clock->seconds; // Update the last printed second
                last_print_ns = clock->nanoseconds; // Update the last printed nanosecond
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
                        waitpid(pcb[i].pid, NULL, 0); // Wait for the worker to terminate
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



