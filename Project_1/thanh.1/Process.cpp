// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-01-31

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-01-31

// Process.cpp is a source file that contains the implementation of the Process class

#include "Process.h"

// Constructor
Process::Process(): pid(-1) {}

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
        string iteration_str = to_string(iteration);
        cout << "User (PID: " << getpid() << ") executing user ... " << endl;
        execl("./user", "./user", iteration_str.c_str(), (char*)NULL); // Execute the user program
        cerr << "Error: execl failed." << endl;
        exit(1);
    }
}

void Process::wait_for_completion() {
    if (pid > 0) {
        int status; // Child process status
        waitpid(pid, &status, 0); // Wait for the child process to finish
        if(WIFEXITED(status)) {
            cout << "User process ID " << pid << " exited with status " << WEXITSTATUS(status) << "." << endl;
        } else {
            cerr << "User process ID " << pid << " exited abnormally." << endl;
        }
    }
}

// Private member functions
string Process::to_string(const int num) {
    ostringstream oss;
    oss << num;
    return oss.str();
}
