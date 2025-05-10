#ifndef DEADLOCK_H
#define DEADLOCK_H

#include "Resource_Table.h"

int detect_deadlock(const Resource_Table&, int[]);

void init_matrix(const Resource_Table&, int[]); // Initialize the matrix for deadlock detection
void mark_all_unfinished(bool[]); // Mark all processes as unfinished
bool can_finish(const Resource_Table&, int, const int[]); // Check if a process can finish its execution
void simulate_execution(const Resource_Table&, int[], bool[]); // Simulate the execution of a process
int collect_deadlocked(const bool[], int[]); // Collect the deadlocked processes

#endif // DEADLOCK_H