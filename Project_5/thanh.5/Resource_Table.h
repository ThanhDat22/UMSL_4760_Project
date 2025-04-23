// Created by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-04-21

// Last edited by Thanh Dat Nguyen (tnrbf@umsystem.edu) on 2025-04-21

#ifndef RESOURCE_TABLE_H
#define RESOURCE_TABLE_H

#include "Message.h"

#include <cstdlib>
#include <iomanip>



struct Resource_info {
    int total_instances; // Total number of instances of the resource
    int available_instances; // Number of available instances of the resource
    int allocation[MAX_PROCESSES]; // Number of instances allocated to each process
    int request[MAX_PROCESSES]; // Number of instances requested by each process
};

class Resource_Table {
public:
    // Constructor
    Resource_Table();

    // Class member functions
    void Initialize(); // Initialize the resource table
    bool request_resource(int, int);
    void release_resource(int, int);
    void release_all_resources(int);
    bool check_safety(int, int) const;
    const resource_info* get_resource_info(int) const;
    void log_resources(ostream&) const;



private:
    Resource_info resources[MAX_RESOURCES]; // Array of resources
};

#endif // RESOURCE_TABLE_H