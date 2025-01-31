// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-01-31

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-01-31

// Process.h is a header file that contains the Process class declaration
#ifndef PROCESS_H
#define PROCESS_H

#include <sys/types.h> // For pid_t
#include <iostream> // For cerr
#include <string> // For string
#include <sstream> // For ostringstream
#include <unistd.h> // For fork(), exec(), getpid(), getppid(), sleep()
#include <sys/wait.h> // For waitpid()

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::ostringstream;

// Process class: Represents a process that can be forked and executed by the operating system simulator (oss)
class Process {
    public:
    // Constructor
    Process();

    // Getters
    pid_t get_pid() const; // Get the process ID

    // Setters
    void set_pid(pid_t); // Set the process ID

    // Member functions
    void launch(int); // Fork and execute the process
    void wait_for_completion(); // Wait for the child process to finish


    private:
    // Data fields
    pid_t pid; // Process ID

    // Private member functions
    string to_string(const int); // Convert an integer to a string
};
#endif // PROCESS_H