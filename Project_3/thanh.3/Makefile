# Created by Thanh Dat Nguyen on 2025-02-11

# Last updated by Thanh Dat Nguyen on 2025-03-13

# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++98 -g

# Targets and sources
TARGET = oss
WORKER = worker
OBJS = Oss.o Shared_Clock.o
WORKER_OBJS = Worker.o Shared_Clock.o

# Build the project
all: $(TARGET) $(WORKER)
	touch logfile
	chmod 666 logfile

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) -lrt

$(WORKER): $(WORKER_OBJS)
	$(CXX) $(CXXFLAGS) -o $(WORKER) $(WORKER_OBJS) -lrt

# Compile object files
Oss.o: Oss.cpp Oss.h Shared_Clock.h
	$(CXX) $(CXXFLAGS) -c Oss.cpp

Worker.o: Worker.cpp Worker.h Shared_Clock.h
	$(CXX) $(CXXFLAGS) -c Worker.cpp

Shared_Clock.o: Shared_Clock.cpp Shared_Clock.h
	$(CXX) $(CXXFLAGS) -c Shared_Clock.cpp

# Clean up generated files
clean:
	rm -f $(TARGET) $(WORKER) $(OBJS) $(WORKER_OBJS) *.txt *.o *.exe logfile

# Run the project
run: all
	touch logfile
	./$(TARGET) -n 5 -s 2 -t 10 -i 500 -f 

# Avoid conflicts with files named 'clean' or 'run'
.PHONY: all clean run


