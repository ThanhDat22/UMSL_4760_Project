# CS 4760 Operating Systems – Project Collection

## 📌 Course Overview
This repository contains **six major programming projects** completed for **CS 4760: Operating Systems (Spring 2024–2025, Prof. Hauschild)**.  
Each project builds on the previous one, progressively implementing an **Operating System Simulator (oss)** with user/worker processes, IPC mechanisms, scheduling, resource management, and memory management.

All projects are written in **C** and executed in the **Linux opsys environment** provided for the course.  
Each project uses:
- **Makefile** for compilation
- **Git** for version control
- **README.md** for documentation

---

## 📂 Projects

### **Project 1 – Multiple Processes Skeleton**
- **Goal:** Warm-up project to get familiar with process creation and Makefiles【64†source】.
- **Key Features:**
  - `oss` launches multiple `user` processes with parameters for number of children, iterations, and simultaneous processes.
  - Demonstrates **fork()**, **exec()**, **wait()**, and command-line argument parsing.
  - Makefile compiles both executables.
- **Run Example:**
  ```bash
  ./oss -n 5 -s 3 -t 7
  ```

---

### **Project 2 – Shared Memory Clock & Process Table**
- **Goal:** Add a **simulated system clock** and **process table** in shared memory【65†source】.
- **Key Features:**
  - `oss` maintains a clock (seconds + nanoseconds) in shared memory.
  - Worker processes run for a random duration and terminate when their simulated time expires.
  - Process Control Block (PCB) array tracks PIDs, start times, and occupancy.
  - Command-line options include child launch interval and time limits.
- **Run Example:**
  ```bash
  ./oss -n 10 -s 5 -t 7 -i 100
  ```

---

### **Project 3 – Message Queues & Coordinated Execution**
- **Goal:** Introduce **tight coordination** between `oss` and workers using **message queues**【66†source】.
- **Key Features:**
  - `oss` alternates sending messages to workers, waiting for responses before proceeding.
  - Workers loop until their termination time, replying to `oss` each iteration.
  - Logging to both screen and file (`-f logfile`).
  - Enhanced command-line options: `-n`, `-s`, `-t`, `-i`, `-f`.
- **Run Example:**
  ```bash
  ./oss -n 12 -s 4 -t 8 -i 200 -f output.log
  ```

---

### **Project 4 – Process Scheduling (MLFQ)**
- **Goal:** Implement a **Multi-Level Feedback Queue (MLFQ) Scheduler**【67†source】.
- **Key Features:**
  - `oss` simulates CPU scheduling with a logical clock.
  - PCBs track CPU usage, system time, blocked states.
  - Three priority queues with different quantum sizes.
  - Scheduling via message queues; user processes simulate execution, I/O blocking, or termination.
  - Statistics: average wait time, CPU utilization, blocked time.
- **Run Example:**
  ```bash
  ./oss -n 15 -s 5 -f schedule.log
  ```

---

### **Project 5 – Resource Management with Deadlock Detection**
- **Goal:** Add a **Resource Management Module** with **deadlock detection and recovery**【68†source】.
- **Key Features:**
  - `oss` manages 5 resources, each with 10 instances, tracked in resource descriptors.
  - Workers randomly request/release resources, possibly leading to deadlock.
  - Deadlock detection runs every simulated second; processes terminated to resolve deadlock.
  - Logging includes resource allocation tables, requests, and recovery actions.
- **Run Example:**
  ```bash
  ./oss -n 20 -s 6 -i 250 -f resource.log
  ```

---

### **Project 6 – Memory Management with LRU Paging**
- **Goal:** Implement **paging and page replacement** with **Least Recently Used (LRU)**【69†source】.
- **Key Features:**
  - System memory: 128KB, divided into 256 frames (1KB each).
  - Each process has a page table (32 entries, 32KB address space).
  - User processes generate random memory read/write requests.
  - On page faults, frames selected by **LRU policy**; dirty-bit optimization included.
  - `oss` logs memory map, frame table, and page tables periodically.
- **Run Example:**
  ```bash
  ./oss -n 25 -s 8 -i 300 -f memory.log
  ```

---

## ⚙️ Compilation
Each project has its own `Makefile`:
```bash
make        # builds oss and user/worker executables
make clean  # removes executables and object files
```

---

## 👨‍💻 Author
- **Thanh Dat Nguyen**  
  University of Missouri–St. Louis  
  Course: CS 4760 – Operating Systems (Spring 2024–2025)  
  Instructor: Prof. Hauschild

---
