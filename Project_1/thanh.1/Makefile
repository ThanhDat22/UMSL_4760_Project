#Created by Thanh Dat Nguyen on 2025-01-30

#Last updated by Thanh Dat Nguyen on 2025-02-11

# Makefile for Skeleton multiple process program

# Compiler
CXX = g++ -g3  # Use g++ for C++ files
CXXFLAGS = -Wall -g3  # Enable warnings, debugging

# Targets
TARGETS = oss user

# Default rule: Compile both executables
all: $(TARGETS)

# Compile oss
oss: oss.cpp Process.o
	$(CXX) $(CXXFLAGS) -o oss oss.cpp Process.o

# Compile user
user: user.cpp
	$(CXX) $(CXXFLAGS) -o user user.cpp

# Compile Process
Process.o: Process.cpp Process.h
	$(CXX) $(CXXFLAGS) -c Process.cpp

# Clean up object files and executables
clean:
	rm -f $(TARGETS) *.o