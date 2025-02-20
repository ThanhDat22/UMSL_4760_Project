// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-13

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-19

// oss.cpp is a source file that contains the Oss class implementation

#include "Oss.h"

// Constructor:
Oss::Oss(int num_processes, int max_simultaneous, int time_limit):
    num_processes(num_processes),
    max_simultaneous(max_simultaneous),
    time_limit(time_limit),
    process_manager(clock) {}

// Class member functions:

// Run the OSS execution
void Oss::run() {
    cout << "OSS: Launching " << num_processes << "worker processes, with a maximum of "
        << max_simultaneous << " running at a time and time limit " 
        << time_limit << " seconds." << endl;

    magage_workers(); // Manage the worker processes

    wait_for_workers(); // Wait for the all workers to finish 
}

// Wait for the all workers to finish
void Oss::wait_for_workers() {
    cout << "OSS: Waiting for all workers to finish..." << endl;

    while (!process_manager.terminate_all()) {
        update_clock();
    }

    cout << "OSS: All workers have finished." << endl;
}

// Private member functions:

void Oss::update_clock() {
    for (size_t i = 0; i < 5; i++ ) {
        clock.increment_nano_second(ONE_BILLION);
        process_manager.check_terminated_worker();
        process_manager.print_process_table();
    }
}

void Oss::magage_workers() {
    cout << "OSS: Managing workers..." << endl;

    int created_workers = 0;
    int running_workers = 0;

    while(created_workers < num_processes) {
        if (running_workers < max_simultaneous) {
            process_manager.create_worker(time_limit);
            created_workers++;
            running_workers++;
        }
        else {
            update_clock();
            running_workers = process_manager.get_active_workers(); // Count active workers
        }
    }
}

// Display usage information
void print_usage() {
    cout << "Usage: ./oss [-h] [-n proc] [-s simul] [-t iter] [-i interval_in_ms_to_launch_children]" << endl;
    cout << "[-h] Display this help message." << endl;
    cout << "[-n proc] Stands for the number of worker processes to launch." << endl;
    cout << "[-s simul] Indicates the maximum number of worker processes to allow to run simultaneously." << endl;
    cout << "[-t time_limit_for_children] Stand for the time limit for the worker processes." << endl;
    cout << "[-i interval_in_ms_to_launch_children] Stands for the interval in milliseconds to launch children." << endl;
}
