// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-01-31

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-16

// Process.h is a header file that contains the Process class declaration

#ifndef PROCESS_H
#define PROCESS_H

#include <sys/types.h> // For pid_t
#include <iostream> // For cerr
#include <string> // For string
#include <sstream> // For ostringstream
#include <unistd.h> // For fork(), exec(), getpid(), getppid(), sleep()
#include <sys/wait.h> // For waitpid()

using namespace std;

/** @brief The Process class is used to manage the worker process.
 * It is responsible for launching the worker process and waiting for it to finish.
 */
class Process {
    public:
    // Constructor
    Process(pid_t = 0, int = 0, int = 0); // The default constructor

    // Getters
    pid_t get_pid() const; // Get the process ID
    int get_start_seconds() const; // Get the start time in seconds
    int get_start_nano() const; // Get the start time in nanoseconds

    // Member functions
    void launch(int); // Fork and execute the process
    void wait_for_completion(); // Wait for the child process to finish
    bool is_running() const; // Check if the process is running


    private:
    // Data fields
    pid_t pid; // Process ID
    int start_seconds; // start time in seconds
    int start_nano; // start time in nanoseconds

    // Private member functions
    static string to_string(const int); // Convert an integer to a string
};
#endif // PROCESS_H