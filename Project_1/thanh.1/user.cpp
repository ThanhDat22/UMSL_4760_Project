// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-01-30

// Last edited by Thanh Dat Nguyen (tnrbf@ymsystem.edu) on 2025-02-01 

// user.cpp is a program that takes a number of iterations as a command line argument and prints out the process ID, parent process ID, and iteration number before and after sleeping for 1 second.

#include <iostream>
#include <unistd.h> // For getpid(), getppid(), sleep()
#include <cstdlib> // For atoi()

using std::cerr;
using std::endl;
using std::cout;
using std::atoi;


// Function prototypes
void print_usage(); // Display usage message
int get_iterations(int, char**); // Get the number of iterations from the command line
void run_iterations(int); // Execute the iterations

int main(int argc, char** argv) {
    int num_of_iterations = get_iterations(argc, argv);
    run_iterations(num_of_iterations);
    return 0;
}

// Function definitions

// Display usage message
void print_usage() {
    cerr << "Usage: ./user <number of iterations>" << endl;
}

/*  Get the number of iterations from the command line
    * @param argc The number of command line arguments
    * @param argv The command line arguments
    * @return The number of iterations
*/
int get_iterations(int argc, char** argv) {
    if (argc != 2) {
        print_usage();
        exit(1);
    }
    
    int num_of_iterations = atoi(argv[1]); // Convert the string to an integer

    // Check if the number of iterations is valid
    if (num_of_iterations <= 0) { 
        print_usage();
        cerr << "The number of iterations must be greater than 0." << endl;
        exit(1);
    }

    return num_of_iterations;
}

/*  Execute the iterations
    * @param num_of_iterations The number of iterations
*/
void run_iterations(int num_of_iterations) {
    pid_t pid = getpid(); // Get the process ID
    pid_t ppid = getppid(); // Get the parent process ID

    for (int i = 1; i <= num_of_iterations; i++) {
        cout << "User PID:" << pid << " PPID:" << ppid << " Iteration:" << i << " before sleeping" << endl;
        sleep(1);
        cout << "User PID:" << pid << " PPID:" << ppid << " Iteration:" << i << " after sleeping" << endl;
    }
}