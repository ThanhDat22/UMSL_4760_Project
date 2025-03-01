// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-13

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-20

#include "Shared_Clock.h"

#include <iostream>
#include <cstdlib>    // for atoi()
#include <unistd.h>   // for getpid(), getppid()
#include <signal.h>   // for signal(), SIGUSR1

using namespace std;

// Function prototypes

void print_usage(); // Print usage information
void starting_message(int, int, int, int); // Print starting message
void parse_arguments(int, char**, int&, int&); // Parse command line arguments
void compute_termination_time(Clock*, int, int, int&, int&, int&, int&); // Compute termination time
void run_worker(Clock*, int, int, int, int); // Run worker process
void wake_signal_handler(int); // Signal handler for wake signal

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

    int start_seconds, start_nanoseconds, termination_seconds, termination_nanoseconds;
    compute_termination_time(clock, max_seconds, max_nanoseconds, start_seconds, start_nanoseconds, termination_seconds, termination_nanoseconds);

    // Print starting message
    starting_message(start_seconds, start_nanoseconds, termination_seconds, termination_nanoseconds);

    // Run the busy-waiting loop
    run_worker(clock, start_seconds, start_nanoseconds, termination_seconds, termination_nanoseconds);

    return 0;


}

// Function definitions

// Print usage information
void print_usage() {
    cout << "Usage: " << "./worker" << " <max_seconds> <max_nanoseconds>" << endl;
    cout << "  <max_seconds>: Maximum number of seconds to wait" << endl;
    cout << "  <max_nanoseconds>: Maximum number of nanoseconds to wait" << endl;
}

// Print starting message
void starting_message(int start_seconds, int start_nanoseconds, int termination_seconds, int termination_nanoseconds) {
    cout << "WORKER PID:" << getpid()
         << " PPID:" << getppid()
         << " SysClockS:" << start_seconds
         << " SysclockNano:" << start_nanoseconds
         << " TermTimeS:" << termination_seconds
         << " TermTimeNano:" << termination_nanoseconds
         << "\n--Just Starting" << endl;   
}

/** @brief Parses command line arguments to get the maximum seconds and nanoseconds.
 *  @param argc The number of command line arguments.
 *  @param argv The array of command line arguments.
 *  @param max_seconds Reference to an integer to store the maximum seconds.
 *  @param max_nanoseconds Reference to an integer to store the maximum nanoseconds.
 */
void parse_arguments(int argc, char** argv, int& max_seconds, int& max_nanoseconds) {
    max_seconds = atoi(argv[1]);
    max_nanoseconds = atoi(argv[2]);
}

/** @brief Computes the termination time based on the current clock time and the maximum seconds and nanoseconds.
 *  @param clock Pointer to the shared clock structure.
 *  @param max_seconds Maximum number of seconds to wait.
 *  @param max_nanoseconds Maximum number of nanoseconds to wait.
 *  @param start_seconds Reference to an integer to store the starting seconds.
 *  @param start_nanoseconds Reference to an integer to store the starting nanoseconds.
 *  @param termination_seconds Reference to an integer to store the termination seconds.
 *  @param termination_nanoseconds Reference to an integer to store the termination nanoseconds.
 */
void compute_termination_time(Clock* clock, int max_seconds, int max_nanoseconds, int& start_seconds, int& start_nanoseconds, int& terminate_seconds, int& terminate_nanoseconds) {
    start_seconds = clock->seconds;
    start_nanoseconds = clock->nanoseconds;

    terminate_seconds = start_seconds + max_seconds;
    terminate_nanoseconds = start_nanoseconds + max_nanoseconds;

    // Handle overflow of nanoseconds
    if (terminate_nanoseconds >= ONE_BILLION) {
        terminate_seconds += terminate_nanoseconds / ONE_BILLION;
        terminate_nanoseconds %= ONE_BILLION;
    }
    // Debugging output to check computed termination time
    cout << "DEBUG: Worker PID " << getpid() << " Termination Time -> "
         << "Seconds: " << terminate_seconds << " Nanoseconds: " << terminate_nanoseconds << endl;
}

/** @brief Runs the worker process in a busy-waiting loop until the termination time is reached.
 *  @param clock Pointer to the shared clock structure.
 *  @param start_seconds The starting seconds.
 *  @param start_nanoseconds The starting nanoseconds.
 *  @param terminate_seconds The termination seconds.
 *  @param terminate_nanoseconds The termination nanoseconds.
 */
void run_worker(Clock* clock, int start_seconds, int start_nanoseconds, int terminate_seconds, int terminate_nanoseconds) {
   struct sigaction sa;
    sa.sa_handler = wake_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL); // Set up signal handler for SIGUSR1
    
    while(true) {
        pause(); // Wait for wake signal

        int current_seconds = clock->seconds;
        int current_nanoseconds = clock->nanoseconds;

        // Debugging: Ensure worker sees updated clock values
        cout << "DEBUG: Worker " << getpid() << " sees clock update: "
             << current_seconds << "s " << current_nanoseconds << "ns" << endl;

        // Check if the termination time has been reached
        if((current_seconds > terminate_seconds) ||
           (current_seconds == terminate_seconds && current_nanoseconds >= terminate_nanoseconds)) {
            cout << "WORKER PID:" << getpid()
                 << " PPID:" << getppid()
                 << " SysClockS:" << current_seconds
                 << " SysclockNano:" << current_nanoseconds
                 << " TermTimeS:" << terminate_seconds
                 << " TermTimeNano:" << terminate_nanoseconds
                 << "\n--Terminating" << endl;
            exit(0);
        }
    }
}

// Signal handler for wake signal
void wake_signal_handler(int signum) {
    return; // Do nothing
}