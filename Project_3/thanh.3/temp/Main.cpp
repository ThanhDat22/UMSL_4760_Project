#include "Process.h"
#include <iostream>
#include <unistd.h>

using namespace std;

int main() {
    std::cout << "Starting Process Test..." << std::endl;

    // Create a Process object
    Process process;

    // Fork a child process
    if (process.launch(5, 500000)) {
        std::cout << "Child process started with PID: " << process.get_pid() << std::endl;
    } else {
        std::cerr << "Failed to launch child process." << std::endl;
        return 1;  // Exit with error
    }

    // Wait a few seconds to simulate work
    sleep(3);

    // Check if the process is still running
    if (process.is_running()) {
        std::cout << "Process " << process.get_pid() << " is still running..." << std::endl;
    } else {
        std::cout << "Process " << process.get_pid() << " has already terminated." << std::endl;
    }

    // Wait for process to finish
    process.wait_for_completion();
    std::cout << "Process test completed." << std::endl;

    return 0;
}