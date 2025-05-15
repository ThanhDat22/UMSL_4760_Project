# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -g -o

# Executables
TARGETS = oss user

# Object files
OSS_OBJECTS = Oss.o Shared_Clock.o Frame_Table.o Message.o
USER_OBJECTS = User.o Shared_Clock.o Message.o

# Build all targets
all: $(TARGETS)

# OSS Executable
oss: $(OSS_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OSS_OBJECTS) -lrt

# User Executable
user: $(USER_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(USER_OBJECTS) -lrt

# Object file rules
Oss.o: Oss.cpp Oss.h Frame_Table.h Message.h Shared_Clock.h
	$(CXX) $(CXXFLAGS) -c $<

User.o: User.cpp User.h Message.h Shared_Clock.h
	$(CXX) $(CXXFLAGS) -c $<

Shared_Clock.o: Shared_Clock.cpp Shared_Clock.h
	$(CXX) $(CXXFLAGS) -c $<

Message.o: Message.cpp Message.h
	$(CXX) $(CXXFLAGS) -c $<

Frame_Table.o: Frame_Table.cpp Frame_Table.h
	$(CXX) $(CXXFLAGS) -c $<

# Clean object files and executables
clean:
	rm -f *.o $(TARGETS) logfile
	rm -f msgqueue.key
	rm -f *.out
	rm -f *.log

# Run the main executable
run:
	./oss -n 5 -s 20 -i 500 -f logfile.log

