# **Paging**

## **Description**

- This project is about resource management inside an operating system. 
- The multiple user processes through shared memory and a message queue. 
- Multiple user processes interact through shared memory and message queues to request 
  - read and write access to memory pages
  - page fault handling for memeory not currently loaded
  - page eviction strategy when frames are full
  - frame table management
  

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

- Shared System Clock: Global time synchronization using shared memory.
- Process Creation and Management: User processes are spawned and managed with fork/exec.
- Interprocess Communication: System V Message Queues for signaling between OSS and users.
- Frame Table Management: Efficient page tracking and mapping to physical frames.
- Page Fault Handling: Triggers when a requested page is not currently in memory.
- Page Replacement Strategy: Uses LRU (Least Recently Used) for eviction when memory is full.
- Memory Access Logging: Detailed logs of:
    . Memory requests (READ or WRITE)
    . Page faults
    . Evictions from memory
    . Frame allocations
- Configurable Runtime: You can adjust the number of users, concurrency, and interval of launches.
- Cleanup Logic: Ensures all shared memory, message queues, and process states are cleaned up on exit.
