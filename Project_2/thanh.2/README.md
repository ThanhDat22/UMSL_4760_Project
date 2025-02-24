# **Introducing Clock and Process Table**

## **Description**

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

    . -n 5: Launch 5 worker processes.
    . -s 3: Run at most 3 worker processes simultaneously.
    . -t 7: Ech worker processes runs for up to 7 seconds before terminating.
    . -i 100: a new worker process is launched every 100 milliseconds. 
Program output example:

## **Special Considerations**

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

### What was done

### Gaps
