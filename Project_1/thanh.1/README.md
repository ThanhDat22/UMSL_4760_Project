# Project 1: Launching Child Process

## Description
The project simulates an Operating System (OSS) that manages multiple user processes:
    * Launch user processes.
    * Executes them.
    * Waits for them to complete.
The number of process and their simultaneous execution are controlled via command line.

## Compilation
To compile the project, type in command line: 
```sh
make
```

## Running the Project

### Executes oss
To executes OSS, type in command line:
```sh
./oss -n <num_users> -s <max_simultaneous_users> -t <iterations>
```
Example:
```sh
./oss -n 5 -s 3 -t 7
```
    . -n 5: Launch 5 user processes.
    . -n 3: Run at most 3 user processes simultaneously.
    . -t 7: Ech user process perform 7 iterations.

### Help Option
To display usage instruction, type in command line:
```sh
./oss -h
```