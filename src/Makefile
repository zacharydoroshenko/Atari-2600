# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

# Source files
SRCS = main.cpp cpu.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Output executable
TARGET = Atari2600

# Default target
all: $(TARGET)

# Rule to build the target executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to build object files
%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to build cpuTest
cpuTest: cpuTest.cpp cpu.o
	$(CXX) $(CXXFLAGS) -o $@ $^

# Cleanup
clean:
	rm -f $(OBJS) $(TARGET) cpuTest

# Phony targets
.PHONY: all clean
