// Created by Thanh Dat Nguyen on 2025-01-31
// Last modified by Thanh Dat Nguyen on 2025-01-31

#include <iostream>

using std::cout;
using std::endl;

// Function prototypes
void print_usage(); // Print usage information

// Main function

// Function definitions

void print_usage() {
    cout << "Usage: ./oss [-h] [-n proc] [-s simul] [-t iter]" << endl;
    cout << "[-h] Display this help message." << endl;
    cout << "[-n proc] Stands for the number of user processes to launch." << endl;
    cout << "[-s simul] Indicates the maximum number of user processes to allow to run simultaneously." << endl;
    cout << "[-t iter] Indicates the number of interation for each user process." << endl;

}