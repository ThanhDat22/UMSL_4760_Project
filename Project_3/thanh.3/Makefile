# Created by Thanh Dat Nguyen on 2025-02-11

# Last updated by Thanh Dat Nguyen on 2025-03-13

# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -g -fpack-struct=1

# Source files
OSS_SRC = Oss.cpp Shared_Clock.cpp Message.cpp Pcb.cpp
WORKER_SRC = Worker.cpp Shared_Clock.cpp Message.cpp Pcb.cpp

# Targets and sources
OSS = oss
WORKER = worker

# Object files
OSS_OBJ = $(OSS_SRC:.cpp=.o)
WORKER_OBJ = $(WORKER_SRC:.cpp=.o)

# Build the project
all: $(OSS) $(WORKER)
	touch logfile
	chmod 666 logfile

# Build OSS
$(OSS): $(OSS_OBJ)
	$(CXX) $(CXXFLAGS) -o $(OSS) $(OSS_OBJ)

# Build Worker
$(WORKER): $(WORKER_OBJ) Oss.o
	$(CXX) $(CXXFLAGS) -o $(WORKER) $(WORKER_OBJ) Oss.o

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean object files and executables
clean:
	rm -f $(OSS_OBJ) $(WORKER_OBJ) $(OSS) $(WORKER) logfile

# Run OSS with example parameters
run:
	./oss -n 5 -s 3 -t 10 -i 100 -f logfile

# Avoid conflicts with files named 'clean' or 'run'
.PHONY: all clean run

# Dependencies
Oss.o: Oss.h Shared_Clock.h Message.h
Worker.o: Worker.h Shared_Clock.h Message.h Oss.h
Shared_Clock.o: Shared_Clock.h
Message.o: Message.h
Pcb.o: Pcb.h
