# **Introducing Clock, Process Table, and Message Coordination**

## **Description**

- The project simulates an Operating System scheduler using a multi-level feedback to manage uer processes.
- It build on the foundation of previous projects, adding full coordination between the oss and worker processes via shared message queues.
  
## **Compilation**

To compile the project, type in command line

```sh
make
```

## **Running the Project**

### Executes oss

To executes OSS, type in command line

```sh
./oss -n <num_workers> -s <max_simultaneous_workers> -t <timelimitForChildren> -i <intervalInMsToLaunchChildren>
```

Example:

```sh
./oss -n 5 -s 3 -t 7 -i 100
```

```plaintext
. -n 5: Launch 5 worker processes.
. -s 3: Run at most 3 worker processes simultaneously.
. -t 7: Each worker processes runs for up to 7 seconds before terminating.
. -i 100: a new worker process is launched every 100 milliseconds.
```

### Help Option

To display usage instruction, type in command line -h any where after ./oss:

```sh
./oss -h
```

or

```sh
./oss -n 5 -s 3 -t 7 -i 100 -h
```

### Cleanup

To delete compile files, type in command line:

```sh
make clean
```

## Special Considerations

### What was done

1. Implemented process management using shared memory and a simulated clock to synchronize worker processes without
race conditions.
2. Used fork(), exec(), and waitpid(WNOHANG) to efficiently create, execute, and manage worker processes.
3. Implemented getopt() for command-line argument parsing, allowing flexible input options.
4. Added -h option to display usage instructions at any position in the command line and exit immediately.
5. Handled case where required arguments are missing or incorrect.
6. Implemented worker wake up using `SIGUSR1`.
7. Formatted the process table output for readability.
8. Added message-passing coordination using `msgsnd` and `msgrcv`.
9. Termination now based on shared clock and message acknowledgment.

### Gaps

- Limit testing case: the maximum of  -n and -s value is 10 due to limitation of opsys server.
- Output is only displayed in the terminal: A possible improvement is writing logs to an output file for easier debugging.
- Signal handling could be enhanced: While OSS detects terminated worker processes, unexpected worker crashes are not explicitly handled.
- Remove OOP due to small project.
