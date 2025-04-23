# **Resource Management**

## **Description**

- This project is about resource management inside an operating system. 
- The multiple user processes through shared memory and a message queue. 
- It tracks time using a shared clock and manages resource allocation while avoiding and resolving deadlocks. 

## **Compilation**

To compile the project, type in command line

```sh
make
```

## **Running the Project**

```sh
./oss -n <num_workers> -s <max_simultaneous_workers> -i <intervalInMsToLaunchChildren> -f <logfile>
```
Example:

```sh
./oss -n 10 -s 1 -i 10 -f logfile
```

### Cleanup

To delete compile files, type in command line:

```sh
make clean
```

## Special Considerations

### What was done

- Shared system clock using shared memory
- Process creation via fork/exec
- Interprocess communication via System V message queues
- Random resource requests and releases form users
- Wait queue for blocked request
- Deadlock detection and recovery using Banker's algorithm
- Detailed logging to logfile
- Configurable runtime and logging
- Clean up shared memory and message queue.
