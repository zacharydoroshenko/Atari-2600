# Compiler and flags
CXX      = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -MMD -MP

# --- SDL2 CONFIGURATION ---
# pkg-config automatically finds the brew paths for headers and libraries
SDL2_CFLAGS := $(shell pkg-config --cflags sdl2)
SDL2_LIBS   := $(shell pkg-config --libs sdl2)

# Add SDL2 headers to CXXFLAGS and libs to a new LDFLAGS variable
CXXFLAGS += $(SDL2_CFLAGS)
LDFLAGS  := $(SDL2_LIBS)
# --------------------------

# Sources and targets
SRCS   := main.cpp cpu.cpp debug.cpp timer.cpp tia.cpp
OBJS   := $(SRCS:.cpp=.o)
DEPS   := $(OBJS:.o=.d)
TARGET := Atari2600

# Default target
all: $(TARGET)

# Link the main program - Added $(LDFLAGS) here
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile any .cpp -> .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Extra executables - Added $(LDFLAGS) to linking steps
cpuTest: cpuTest.o cpu.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

test: test.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Cleanup
clean:
	rm -f $(OBJS) $(DEPS) $(TARGET) cpuTest test *.o *.d

# Include auto-generated dependency files
-include $(DEPS)

.PHONY: all clean