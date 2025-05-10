#include "Deadlock.h"

/** @brief Initialize the matrix for deadlock detection
    @param table : The resource table containing information about resources
    @param work : The work array to be initialized with available instances of resources
 */
void init_matrix(const Resource_Table& table, int work[]) {
    for (int i = 0; i < MAX_RESOURCES; ++i) {
        work[i] = table.get_resource_info(i)->available_instances;
    }
}

/** @brief Mark all processes as unfinished
    @param finish : The finish array to be initialized with false values
 */
void mark_all_unfinished(bool finish[]) {
    for (int i = 0; i < MAX_PROCESSES; ++i) {
        finish[i] = false;
    }
}

/** @brief Check if a process can finish its execution
    @param table : The resource table containing information about resources
    @param pid_index : The index of the process to check
    @param work : The work array containing available instances of resources
    @return true if the process can finish, false otherwise
 */
bool can_finish(const Resource_Table& table, int pid_index, const int work[]) {
    for (int i = 0; i < MAX_RESOURCES; ++i) {
        int need = table.get_resource_info(i)->request[pid_index];
        if (need > work[i]) {
            return false; // Not enough resources available for the process to finish
        }
    }
    return true; // The process can finish
}

/** @brief Simulate the execution of a process
    @param table : The resource table containing information about resources
    @param work : The work array containing available instances of resources
    @param finish : The finish array to mark the process as finished
 */
void simulate_execution(const Resource_Table& table, int pid_index, int work[], bool finish[]) {
    for (int i = 0; i < MAX_RESOURCES; ++i) {
        work[i] += table.get_resource_info(i)->allocation[pid_index]; // Release allocated resources
    }
    finish[pid_index] = true; // Mark the process as finished
}

/** @brief Collect the deadlocked processes
    @param finish : The finish array containing information about finished processes
    @param deadlocked_processes : The array to store the indices of deadlocked processes
    @return The number of deadlocked processes
 */
int collect_deadlocked(const bool finish[], int deadlocked_processes[]) {
    int count = 0;
    for (int i = 0; i < MAX_PROCESSES; ++i) {
        if (!finish[i]) {
            deadlocked_processes[count++] = i;
        }
    }
    return count;
}

/** @brief Detect deadlocks in the system
    @param table : The resource table containing information about resources
    @param deadlocked_processes : The array to store the indices of deadlocked processes
    @return The number of deadlocked processes
 */
int detect_deadlock(const Resource_Table& table, int deadlocked_processes[]) {
    int work[MAX_RESOURCES];
    bool finish[MAX_PROCESSES];
    init_matrix(table, work);
    mark_all_unfinished(finish);

    bool progress_made = true;
    while (progress_made) {
        progress_made = false;
        for (int i = 0; i < MAX_PROCESSES; ++i) {
            if (!finish[i] && can_finish(table, i, work)) {
                simulate_execution(table, i, work, finish);
                progress_made = true;
            }
        }
    }

    return collect_deadlocked(finish, deadlocked_processes);
}
