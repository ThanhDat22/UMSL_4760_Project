// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-14

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-02-21

// shared_clock.cpp is a source file that contains the implementation of the Shared_Clock

#include "Shared_Clock.h"

/** @brief Detach from the shared clock 
    @param shm_id : The shared memory ID
    @param create : The flag to create a new clock or not
    @return Shared_Clock* : A pointer to the shared clock structure
 */
Shared_Clock* attach_clock(int* shm_id, int create) {
    int flags = 0666;
    
    // Create a new shared memory segment or attach to an existing one
    if (create) {
        flags |= IPC_CREAT;
    }

    // Get the shared memory segment ID
    *shm_id = shmget(SHM_KEY_CLOCK, sizeof(Shared_Clock), flags);
    if (*shm_id == -1) {
        perror("shmget failed");
        return NULL;
    }

    // Attach to the shared memory segment
    Shared_Clock* clock = (Shared_Clock*)shmat(*shm_id, NULL, 0);
    if (clock == (void*) -1) {
        perror("shmat failed");
        return NULL;    
    }

    if (create) {
        // Initialize the clock if it was created
        clock->seconds = 0;
        clock->nanoseconds = 0;
    }
    return clock;
}

/** @brief Detach from the shared clock 
    @param clock : A pointer to the shared clock structure
 */
void detach_clock(Shared_Clock* clock) {
    if (shmdt(clock) == -1) {
        perror("shmdt failed");
    }
}

/** @brief Destroy the shared clock 
    @param shm_id : The shared memory ID
 */
void destroy_clock(int shm_id) {
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("shmctl failed");
    }
}

