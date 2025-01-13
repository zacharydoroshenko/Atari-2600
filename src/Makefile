# Compiler and flags
CXX = gcc
CXXFLAGS = -Wall -Wextra -02 -std=c++17

#Files and dependencies
TARGET = main
OBJS = main.o cpu.o

#Default target
all: $(TARGET)

#Rule to build the target executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

#Rule to compile main.cpp
main.o: main.cpp cpu.h
	$(CXX) $(CXXFLAGS) -c main.cpp

#Rule to compile cpu.cpp
cpu.o: cpu.cpp cpu.h
	$(CXX) $(CXXFLAGS) -c cpu.cpp

#Clean up generated files
clean:
	rm -f $(OBJS) $(TARGET)

