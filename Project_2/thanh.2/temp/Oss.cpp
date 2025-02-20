// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-13

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-19

// oss.cpp is a source file that contains the Oss class implementation

#include "Oss.h"

// Constructor:
Oss::Oss(int num_processes, int max_simultaneous, int time_limit, int interval_in_ms):
    num_processes(num_processes),
    max_simultaneous(max_simultaneous),
    time_limit(time_limit), interval_in_ms(interval_in_ms),
    process_manager(clock) {}

// Class member functions:

// Run the OSS execution
void Oss::run() {
    cout << "OSS: Launching " << num_processes << "worker processes, with a maximum of "
        << max_simultaneous << " running at a time and time limit " 
        << time_limit << " seconds." << endl;

    manage_workers(); // Manage the worker processes

    wait_for_workers(); // Wait for the all workers to finish 
}

// Wait for the all workers to finish
void Oss::wait_for_workers() {
    cout << "OSS: Waiting for all workers to finish..." << endl;

    while (!process_manager.all_workers_terminated()) {
        update_clock();
    }

    cout << "OSS: All workers have finished." << endl;
}

// Private member functions:

void Oss::update_clock() {
    for (size_t i = 0; i < 5; i++ ) {
        clock.increment_nano_second(ONE_BILLION / 10); // Update time by 100ms
        process_manager.check_terminated_worker();
        process_manager.print_process_table();
    }
}

void Oss::manage_workers() {
    cout << "OSS: Managing workers..." << endl;

    int created_workers = 0;
    int last_launch_sec = clock.get_second();
    int last_launch_nano = clock.get_nano_second();

    while(created_workers < num_processes) {
        int current_sec = clock.get_second();
        int current_nano = clock.get_nano_second();

        int interval_nano = interval_in_ms * 1000000; // Convert milliseconds to nanoseconds

        bool time_to_launch = (current_sec * ONE_BILLION + current_nano) - (last_launch_sec * ONE_BILLION + last_launch_nano) >= interval_nano;

        if(process_manager.get_active_workers() < max_simultaneous && time_to_launch) {
            process_manager.create_worker(time_limit);
            created_workers++;
            last_launch_sec = current_sec;
            last_launch_nano = current_nano;
        }
        update_clock(); // Update the clock
    }
}

// Display usage information
void Oss::print_usage() {
    cout << "Usage: ./oss [-h] [-n proc] [-s simul] [-t iter] [-i interval_in_ms_to_launch_children]" << endl;
    cout << "[-h] Display this help message." << endl;
    cout << "[-n proc] Stands for the number of worker processes to launch." << endl;
    cout << "[-s simul] Indicates the maximum number of worker processes to allow to run simultaneously." << endl;
    cout << "[-t time_limit_for_children] Stand for the time limit for the worker processes." << endl;
    cout << "[-i interval_in_ms_to_launch_children] Stands for the interval in milliseconds to launch children." << endl;
}

/** @brief Parse the command line arguments.
 *  @param argc The number of command line arguments.
 *  @param argv The command line arguments.
 */
void Oss::parse_arguments(int argc, char** argv) {
    int options;

    while((options = getopt(argc, argv, "hn:s:t:i:")) != -1) {
        switch(options) {
            case 'h':
                print_usage();
                exit(0);
            case 'n':
                num_processes = atoi(optarg);
                break;
            case 's':
                max_simultaneous = atoi(optarg);
                break;
            case 't':
                time_limit = atoi(optarg);
                break;
            case 'i':
                interval_in_ms = atoi(optarg);
                break;
            default:
                print_usage();
                exit(1);
        }
    }
}