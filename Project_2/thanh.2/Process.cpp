// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-01-31

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-14

// Process.cpp is a source file that contains the implementation of the Process class

#include "Process.h"

// Constructor
Process::Process(pid_t pid, int start_seconds, int start_nano) : pid(pid), start_seconds(start_seconds), start_nano(start_nano) { }

// Getters
pid_t Process::get_pid() const { return pid; }
int Process::get_start_seconds() const { return start_seconds; }
int Process::get_start_nano() const { return start_nano; }

// Member functions

/*  Fork and execute the process
    * @param iteration: The number of iterations for the user process
*/
void Process::launch(int process_id) {
    pid = fork(); // Fork the process

    // Fork failed
    if (pid < 0) { 
        cerr << "Error: Fork failed." << endl;
        exit(1);
    }
    // Child process executes the user program
    else if (pid == 0) { // Child process
        execl("./worker", "./worker", to_string(start_seconds).c_str(), to_string(start_nano).c_str(), NULL);
        cerr << "Error: Exec failed." << endl;
        exit(1);
    } else if (pid > 0) { // Parent process
        cout << "Process ID: " << pid << " has been created." << endl;
    }
}

// Wait for the child process to finish
void Process::wait_for_completion() {
    if (pid > 0) {
        int status; // Child process status
        waitpid(pid, &status, 0); // Wait for the child process to finish
        if(WIFEXITED(status)) {
            cout << "Worker process ID " << pid << " exited with status " << WEXITSTATUS(status) << "." << endl;
        } else {
            cerr << "User process ID " << pid << " exited abnormally." << endl;
        }
    }
}

// Check if the process is running
bool Process::is_running() const {
    if (pid <= 0) {
        return false; // Process is not running
    }

    // Send signal 0 to check if the process is running
    if (kill(pid, 0) == 0) {
        return true; // Process is running
    } else {
        return false; // Process is not running
    }
}

// Private member functions

/** Convert an integer to a string
 * @param num: The integer to convert
 * @return: The string representation of the integer
 */
string Process::to_string(const int num) {
    ostringstream oss;
    oss << num;
    return oss.str();
}
