# Created by Thanh Dat Nguyen on 2025-02-11

# Last updated by Thanh Dat Nguyen on 2025-02-20

# Compiler
CXX = g++ -g3  # Use g++ for C++ files
CXXFLAGS = -Wall -g3  # Enable warnings, debugging

# Executable name
OSS_EXECUTABLE = oss
WORKER_EXECUTABLE = worker

# Source files
OSS_SRC = Oss.cpp Process.cpp Process_Manager.cpp Shared_Clock.cpp Worker.cpp main.cpp
WORKER_SRC = Worker.cpp Shared_Clock.cpp

# Object files
OSS_OBJ = $(OSS_SRC:.cpp=.o)
WORKER_OBJ = $(WORKER_SRC:.cpp=.o)

# Header files
HEADERS = Oss.h Process.h Process_Manager.h Shared_Clock.h Worker.h

# Default target: build both executables
all: $(OSS_EXECUTABLE) $(WORKER_EXECUTABLE)

# Compile OSS executable
$(OSS_EXECUTABLE): $(OSS_OBJ)
	$(CXX) $(CXXFLAGS) -o $(OSS_EXECUTABLE) $(OSS_OBJ)

# Compile Worker executable
$(WORKER_EXECUTABLE): $(WORKER_OBJ)
	$(CXX) $(CXXFLAGS) -o $(WORKER_EXECUTABLE) $(WORKER_OBJ)

# Compile each .cpp file to .o
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OSS_OBJ) $(WORKER_OBJ) $(OSS_EXECUTABLE) $(WORKER_EXECUTABLE) *.o *.exe

