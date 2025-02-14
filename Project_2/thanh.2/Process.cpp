// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-01-31

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-14

// Process.cpp is a source file that contains the implementation of the Process class

#include "Process.h"

// Constructor
Process::Process(pid_t pid, int terminate_seconds, int terminate_nano_seconds) : pid(pid), terminate_seconds(terminate_seconds), terminate_nano_seconds(terminate_nano_seconds) { }

// Getters
pid_t Process::get_pid() const { return pid; }

// Setters
void Process::set_pid(pid_t pid) { this->pid = pid; }

// Member functions

/*  Fork and execute the process
    * @param iteration: The number of iterations for the user process
*/
void Process::launch(int iteration) {
    pid = fork(); // Fork the process

    // Fork failed
    if (pid < 0) { 
        cerr << "Error: Fork failed." << endl;
        exit(1);
    }
    // Child process executes the user program
    else if (pid == 0) { 
        string sec_str = to_string(terminate_seconds); // Convert termination time to string
        string nano_str = to_string(terminate_nano_seconds); // Convert termination time to string

        cout << "WORKER (PID: " << getpid() << ") executing worker process ... " << endl;
        execl("./worker", "./worker", sec_str.c_str(), nano_str.c_str(), (char*)NULL); // Execute the worker program
        cerr << "Error: execl failed." << endl;
        exit(1);
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
