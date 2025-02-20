#include "Process.h"
#include <iostream>
#include <unistd.h>

using namespace std;

int main() {
    cout << "Starting process test...\n";

    // Create a process that should terminate in 5 seconds and 500000 nanoseconds
    Process p1;
    p1.launch(5, 500000);

    cout << "Process launched with PID: " << p1.get_pid() << endl;

    // Check if process is running
    while (p1.is_running()) {
        cout << "Process " << p1.get_pid() << " is still running...\n";
        sleep(1);
    }

    // Wait for the process to finish
    p1.wait_for_completion();
    
    cout << "Process test complete.\n";
    return 0;
}