// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-04-21

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-04-21

#include "Resource_Table.h"

// Constructor
Resource_Table::Resource_Table() {
    Initialize();
}

// Class member functions

// Initialize the resource table
void Resource_Table::Initialize() {
    for (int i = 0; i < MAX_RESOURCES; ++i) {
        resources[i].total_instances = 0;
        resources[i].available_instances = 0;
        for (int j = 0; j < MAX_PROCESSES; ++j) {
            resources[i].allocation[j] = 0;
            resources[i].request[j] = 0;
        }
    }
}

/** @brief Check if the request can be granted
    @param pid_index : The index of the process requesting the resource
    @param resource_id : The ID of the resource being requested
    @return true if the request can be granted, false otherwise
 */
bool Resource_Table::check_safety(int pid_index, int resource_id) const {
    return resources[resource_id].request[pid_index] <= resources[resource_id].available_instances;
}

/** @brief Request a resource for a process
    @param pid_index : The index of the process requesting the resource
    @param resource_id : The ID of the resource being requested
    @return true if the request is granted, false otherwise
 */
bool Resource_Table::request_resource(int pid_index, int resource_id) {
    if (check_safety(pid_index, resource_id)) {
        resources[resource_id].available_instances -= resources[resource_id].request[pid_index];
        resources[resource_id].allocation[pid_index] += resources[resource_id].request[pid_index];
        return true;
    }
    return false;
}

/** @brief Release a resource for a process
    @param pid_index : The index of the process releasing the resource
    @param resource_id : The ID of the resource being released
 */
void Resource_Table::release_resource(int pid_index, int resource_id) {
    if (resources[resource_id].allocation[pid_index] > 0) {
        resources[resource_id].available_instances += resources[resource_id].allocation[pid_index];
        resources[resource_id].allocation[pid_index] = 0;
    }
}

/** @brief Release all resources for a process
    @param pid_index : The index of the process releasing the resources
 */
void Resource_Table::release_all_resources(int pid_index) {
    for (int i = 0; i < MAX_RESOURCES; ++i) {
        if (resources[i].allocation[pid_index] > 0) {
            resources[i].available_instances += resources[i].allocation[pid_index];
            resources[i].allocation[pid_index] = 0;
        }
    }
}

/** @brief Get the resource information for a specific resource
    @param resource_id : The ID of the resource
    @return A pointer to the resource information structure
 */
const Resource_info* Resource_Table::get_resource_info(int resource_id) const {
    return &resources[resource_id];
}

/** @brief Log the state of the resource table to the output stream
    @param out : The output stream to log the resource table state 
 */
 void Resource_Table::log_resources(std::ostream& out) const {
    out << "\nRESOURCE TABLE STATE (P = process, R = resource):\n";

    // Header row
    out << std::setw(5) << " " ;
    for (int r = 0; r < MAX_RESOURCES; ++r) {
        out << "R" << r << std::setw(6);
    }
    out << "\n";

    // One row per process
    for (int p = 0; p < MAX_PROCESSES; ++p) {
        bool has_allocation = false;
        for (int r = 0; r < MAX_RESOURCES; ++r) {
            if (resources[r].allocation[p] > 0) {
                has_allocation = true;
                break;
            }
        }
        if (!has_allocation) continue; // Skip processes with zero allocation

        out << "P" << std::setw(3) << p << ": ";
        for (int r = 0; r < MAX_RESOURCES; ++r) {
            out << std::setw(4) << resources[r].allocation[p] << " ";
        }
        out << "\n";
    }

    // Print available resources at the bottom
    out << "\nAvailable Instances:\n";
    for (int r = 0; r < MAX_RESOURCES; ++r) {
        out << "R" << r << ": " << resources[r].available_instances << "  ";
    }
    out << "\n\n";
}