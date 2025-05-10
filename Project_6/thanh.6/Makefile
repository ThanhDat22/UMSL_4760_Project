# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -g

# Source files
OSS_SRC = Oss.cpp Shared_Clock.cpp Resource_Table.cpp Deadlock.cpp
USER_SRC = User.cpp Shared_Clock.cpp

# Object files
OSS_OBJ = $(OSS_SRC:.cpp=.o)
USER_OBJ = $(USER_SRC:.cpp=.o)

# Executables
all: oss user

oss: $(OSS_OBJ)
	$(CXX) $(CXXFLAGS) -o oss $(OSS_OBJ)

user: $(USER_OBJ)
	$(CXX) $(CXXFLAGS) -o user $(USER_OBJ)

clean:
	rm -f *.o oss user logfile msgqueue.key logfile
