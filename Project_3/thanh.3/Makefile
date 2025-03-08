# Created by Thanh Dat Nguyen on 2025-02-11

# Last updated by Thanh Dat Nguyen on 2025-02-20

# Compiler
CXX = g++ -g3  # Use g++ for C++ files
CXXFLAGS = -Wall -g3  # Enable warnings, debugging

# Executable name
OSS_EXECUTABLE = oss
WORKER_EXECUTABLE = worker

# Source files
OSS_SRC = Oss.cpp Shared_Clock.cpp
WORKER_SRC = Worker.cpp Shared_Clock.cpp

# Object files
OSS_OBJ = $(OSS_SRC:.cpp=.o)
WORKER_OBJ = $(WORKER_SRC:.cpp=.o)

# Default target: build both executables
all: $(OSS_EXECUTABLE) $(WORKER_EXECUTABLE)

# Compile OSS executable
$(OSS_EXECUTABLE): $(OSS_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile Worker executable
$(WORKER_EXECUTABLE): $(WORKER_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile each .cpp file to .o
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run:
	./oss -n 5 -s 3 -t 2 -i 100

# Clean up
clean:
	rm -f $(OSS_OBJ) $(WORKER_OBJ) $(OSS_EXECUTABLE) $(WORKER_EXECUTABLE) *.o *.exe

