// Created by Thanh Dat Nguyen on 2025-01-31
// Last modified by Thanh Dat Nguyen on 2025-01-31

#include <iostream>
#include <cstdlib> // For exit()
#include <unistd.h> // For getopt()

using std::cout;
using std::endl;

// Function prototypes
void print_usage(); // Print usage information
void parse_arguments(int, char**, int&, int&, int&); // Parse command line arguments

// Main function

// Function definitions

// Display usage information
void print_usage() {
    cout << "Usage: ./oss [-h] [-n proc] [-s simul] [-t iter]" << endl;
    cout << "[-h] Display this help message." << endl;
    cout << "[-n proc] Stands for the number of user processes to launch." << endl;
    cout << "[-s simul] Indicates the maximum number of user processes to allow to run simultaneously." << endl;
    cout << "[-t iter] Indicates the number of interation for each user process." << endl;

}

/*  Parse command line arguments
    * @param argc: Number of command line arguments
    * @param argv: Array of command line arguments
    * @param process: Number of user processes to launch
    * @param simultaneous: Maximum number of user processes to allow to run simultaneously
    * @param iteration: Number of iteration for each user process
 */
void parse_arguments(int argc, char** argv, int& process, int& simultaneous, int& iteration) {
    int options; // Command line options

    // Parse command line arguments
    while ((options = getopt(argc, argv, "hn:s:t:")) != -1) {
        switch (options) {
            case 'h': // Display usage information
                print_usage();
                exit(0);
            case 'n': // Number of user processes to launch
                process = atoi(optarg); // Convert optarg to integer
                break;
            case 's': // Maximum number of user processes to allow to run simultaneously
                simultaneous = atoi(optarg); // Convert optarg to integer
                break;
            case 't': // Number of iteration for each user process
                iteration = atoi(optarg); // Convert optarg to integer
                break;
            default:
                print_usage();
                exit(1);
        } // End switch
    } // End while
}