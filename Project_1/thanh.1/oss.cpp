// Created by Thanh Dat Nguyen on 2025-01-31

// Last modified by Thanh Dat Nguyen on 2025-02-01

#include "Process.h"

#include <iostream>
#include <cstdlib> // For exit()
#include <unistd.h> // For getopt()
#include <vector> // For vector

using std::cout;
using std::endl;
using std::vector;

// Function prototypes
void print_usage(); // Print usage information
void parse_arguments(int, char**, int&, int&, int&); // Parse command line arguments
void manage_processes(int, int, int); // Launch and manage user processes

// Main function
int main(int argc, char** argv) {
    int proc = -1, simul = -1, iter = -1; // Default values for command line arguments

    // Check there is no argument after ./oss
    if (argc == 1) {
        print_usage();
        exit(0);
    }

    // Check -h option present in the command line arguments
    for (int i = 1; i < argc; i++) {
        if (string(argv[i]) == "-h") {
            print_usage();
            exit(0);
        }
    }

    parse_arguments(argc, argv, proc, simul, iter); // Parse command line arguments
    manage_processes(proc, simul, iter); // Launch and manage user processes
    return 0;
}

// Function definitions

// Display usage information
void print_usage() {
    cout << "Usage: ./oss [-h] [-n proc] [-s simul] [-t iter]" << endl;
    cout << "[-h] Display this help message." << endl;
    cout << "[-n proc] Stands for the number of user processes to launch." << endl;
    cout << "[-s simul] Indicates the maximum number of user processes to allow to run simultaneously." << endl;
    cout << "[-t iter] Indicates the number of interation for each user process." << endl;

}

/*  Parse command line arguments
    * @param argc: Number of command line arguments
    * @param argv: Array of command line arguments
    * @param process: Number of user processes to launch
    * @param simultaneous: Maximum number of user processes to allow to run simultaneously
    * @param iteration: Number of iteration for each user process
 */
void parse_arguments(int argc, char** argv, int& process, int& simultaneous, int& iteration) {
    int options; // Command line options

    // Parse command line arguments
    while ((options = getopt(argc, argv, "hn:s:t:")) != -1) {
        switch (options) {
            case 'h': // Display usage information
                print_usage();
                exit(0);
            case 'n': // Number of user processes to launch
                process = atoi(optarg); // Convert optarg to integer
                break;
            case 's': // Maximum number of user processes to allow to run simultaneously
                simultaneous = atoi(optarg); // Convert optarg to integer
                break;
            case 't': // Number of iteration for each user process
                iteration = atoi(optarg); // Convert optarg to integer
                break;
            default:
                print_usage();
                exit(1);
        } // End switch
    } // End while
}

/*  Launch and manage user processes
    * @param process: Number of user processes to launch
    * @param simultaneous: Maximum number of user processes to allow to run simultaneously
    * @param iteration: Number of iteration for each user process
*/
void manage_processes(int process, int simultaneous, int iteration) {
    cout << "OSS launching " << process << " user processes, maximum " << simultaneous << " at a time,, each doing " << iteration << " iterations." << endl;

    int running = 0; // Number of running processes
    vector<Process> processes; // Store user processes

    for (int i = 0; i < process; i++) {
        if (running >= simultaneous) {
            processes.front().wait_for_completion(); // Wait for a user process to complete
            processes.erase(processes.begin()); // Remove the completed user process
            running--; // Decrement the number of running processes
        }

        Process temp_process; // Create a new user process
        temp_process.launch(iteration); // Launch the user process
        processes.push_back(temp_process); // Add the user process to the vector
        running++; // Increment the number of running processes

        sleep(1); // Delay for 1 second to avoid process flooding
    } 

    // Wait for all user processes to complete
    for (size_t i = 0; i < processes.size(); i++) {
        processes[i].wait_for_completion();
    }

    cout << "OSS finished launching and waiting for all user processes. " << endl;
}

