#Create by Thanh Dat Nguyen on 2025-01-30
#Last update by Thanh Dat Nguyen on 2025-01-30

# Makefile for Skeleton multiple process program

# Compiler
CXX = g++ -g3  # Use g++ for C++ files
CXXFLAGS = -Wall -g3  # Enable warnings, debugging

# Targets
TARGETS = oss user

# Default rule: Compile both executables
all: $(TARGETS)

# Compile oss
oss: oss.cpp
	$(CXX) $(CXXFLAGS) -o oss oss.cpp

# Compile user
user: user.cpp
	$(CXX) $(CXXFLAGS) -o user user.cpp

# Clean up object files and executables
clean:
	rm -f $(TARGETS) *.o