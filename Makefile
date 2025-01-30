# Compiler
CXX = g++  # Use g++ for C++ files
CXXFLAGS = -Wall -g -std=c++98  # Enable warnings, debugging, and C++98 standard

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