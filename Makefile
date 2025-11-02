# Assembly Line Simulator Makefile
# Supports Linux, macOS, and Windows (MinGW)

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic
DEBUGFLAGS = -g -O0 -DDEBUG -fsanitize=address -fsanitize=undefined
RELEASEFLAGS = -O3 -DNDEBUG

# Libraries
LIBS = -lsqlite3

# Directories
SRCDIR = src
COREDIR = $(SRCDIR)/core
INFRADIR = $(SRCDIR)/infrastructure
INCLUDEDIR = include
TESTDIR = tests
DATADIR = data
BUILDDIR = build
OBJDIR = $(BUILDDIR)/obj

# Source files (organized by module)
CORE_SOURCES = $(COREDIR)/Station.cpp \
               $(COREDIR)/Workstation.cpp \
               $(COREDIR)/CustomerOrder.cpp \
               $(COREDIR)/LineManager.cpp \
               $(COREDIR)/Utilities.cpp

INFRA_SOURCES = $(INFRADIR)/Logger.cpp \
                $(INFRADIR)/Config.cpp \
                $(INFRADIR)/Database.cpp

SOURCES = $(CORE_SOURCES) $(INFRA_SOURCES) $(SRCDIR)/main.cpp
OBJECTS = $(patsubst $(COREDIR)/%.cpp,$(OBJDIR)/core_%.o,$(CORE_SOURCES)) \
          $(patsubst $(INFRADIR)/%.cpp,$(OBJDIR)/infra_%.o,$(INFRA_SOURCES)) \
          $(OBJDIR)/main.o
HEADERS = $(wildcard $(INCLUDEDIR)/seneca/*.h)

# Test files
TEST_SOURCES = $(wildcard $(TESTDIR)/*.cpp)
TEST_OBJECTS = $(TEST_SOURCES:$(TESTDIR)/%.cpp=$(OBJDIR)/%.o)

# Targets
TARGET = assembly_line
DEBUG_TARGET = $(BUILDDIR)/$(TARGET)_debug
RELEASE_TARGET = $(BUILDDIR)/$(TARGET)

# Data files
DATA_FILES = $(DATADIR)/Stations1.txt $(DATADIR)/Stations2.txt $(DATADIR)/CustomerOrders.txt $(DATADIR)/AssemblyLine.txt

# Default target
.PHONY: all clean debug release test help run

all: release

# Create directories
$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(BUILDDIR):
	@mkdir -p $(BUILDDIR)

# Release build
release: CXXFLAGS += $(RELEASEFLAGS)
release: $(BUILDDIR) $(OBJDIR) $(RELEASE_TARGET)

$(RELEASE_TARGET): $(OBJECTS)
	@echo "Linking release build..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)
	@echo "Release build complete: $@"

# Debug build  
debug: CXXFLAGS += $(DEBUGFLAGS)
debug: $(BUILDDIR) $(OBJDIR) $(DEBUG_TARGET)

$(DEBUG_TARGET): $(OBJECTS)
	@echo "Linking debug build..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)
	@echo "Debug build complete: $@"

# Object file compilation
$(OBJDIR)/core_%.o: $(COREDIR)/%.cpp $(HEADERS) | $(OBJDIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -I$(INCLUDEDIR) -c $< -o $@

$(OBJDIR)/infra_%.o: $(INFRADIR)/%.cpp $(HEADERS) | $(OBJDIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -I$(INCLUDEDIR) -c $< -o $@

$(OBJDIR)/main.o: $(SRCDIR)/main.cpp $(HEADERS) | $(OBJDIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -I$(INCLUDEDIR) -c $< -o $@

# Test targets
test: test1 test2 test3

test1: $(BUILDDIR) $(OBJDIR)
	@echo "Building test 1 (Station and Utilities)..."
	$(CXX) $(CXXFLAGS) $(RELEASEFLAGS) -o $(BUILDDIR)/test1 $(TESTDIR)/tester_1.cpp $(SOURCES) -I$(INCLUDEDIR)
	@echo "Running test 1..."
	cd $(BUILDDIR) && ./test1 ../$(DATADIR)/Stations1.txt ../$(DATADIR)/Stations2.txt

test2: $(BUILDDIR) $(OBJDIR)
	@echo "Building test 2 (CustomerOrder)..."
	$(CXX) $(CXXFLAGS) $(RELEASEFLAGS) -o $(BUILDDIR)/test2 $(TESTDIR)/tester_2.cpp $(SOURCES) -I$(INCLUDEDIR)
	@echo "Running test 2..."
	cd $(BUILDDIR) && ./test2 ../$(DATADIR)/Stations1.txt ../$(DATADIR)/Stations2.txt ../$(DATADIR)/CustomerOrders.txt

test3: $(BUILDDIR) $(OBJDIR)
	@echo "Building test 3 (Full System)..."
	$(CXX) $(CXXFLAGS) $(RELEASEFLAGS) -o $(BUILDDIR)/test3 $(TESTDIR)/tester_3.cpp $(SOURCES) -I$(INCLUDEDIR)
	@echo "Running test 3..."
	cd $(BUILDDIR) && ./test3 ../$(DATADIR)/Stations1.txt ../$(DATADIR)/Stations2.txt ../$(DATADIR)/CustomerOrders.txt ../$(DATADIR)/AssemblyLine.txt

# Run the simulation
run: release
	@echo "Running assembly line simulation..."
	cd $(BUILDDIR) && ./$(TARGET) ../$(DATADIR)/Stations1.txt ../$(DATADIR)/Stations2.txt ../$(DATADIR)/CustomerOrders.txt ../$(DATADIR)/AssemblyLine.txt

# Memory check (Linux/macOS only)
memcheck: debug
	@echo "Running memory check with Valgrind..."
	cd $(BUILDDIR) && valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)_debug ../$(DATADIR)/Stations1.txt ../$(DATADIR)/Stations2.txt ../$(DATADIR)/CustomerOrders.txt ../$(DATADIR)/AssemblyLine.txt

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(BUILDDIR)
	@echo "Clean complete."

# Install (copy to system directories)
install: release
	@echo "Installing assembly line simulator..."
	sudo cp $(RELEASE_TARGET) /usr/local/bin/$(TARGET)
	sudo mkdir -p /usr/local/share/assembly_line
	sudo cp -r $(DATADIR) /usr/local/share/assembly_line/
	@echo "Installation complete. Run with: $(TARGET)"

# Uninstall
uninstall:
	@echo "Uninstalling assembly line simulator..."
	sudo rm -f /usr/local/bin/$(TARGET)
	sudo rm -rf /usr/local/share/assembly_line
	@echo "Uninstall complete."

# Code formatting (requires clang-format)
format:
	@echo "Formatting code..."
	find $(SRCDIR) -name "*.cpp" -o -name "*.h" | xargs clang-format -i -style=Google
	@echo "Code formatting complete."

# Static analysis (requires clang-tidy)
analyze:
	@echo "Running static analysis..."
	clang-tidy $(SOURCES) -- -std=c++17 -I$(SRCDIR)

# Show help
help:
	@echo "Available targets:"
	@echo "  all       - Build release version (default)"
	@echo "  release   - Build optimized release version"
	@echo "  debug     - Build debug version with sanitizers"
	@echo "  test      - Run all test suites"
	@echo "  test1     - Run Station and Utilities tests"
	@echo "  test2     - Run CustomerOrder tests"  
	@echo "  test3     - Run full system tests"
	@echo "  run       - Build and run the simulation"
	@echo "  memcheck  - Run with Valgrind (Linux/macOS)"
	@echo "  clean     - Remove build artifacts"
	@echo "  install   - Install to system (requires sudo)"
	@echo "  uninstall - Remove from system (requires sudo)"
	@echo "  format    - Format code with clang-format"
	@echo "  analyze   - Run static analysis with clang-tidy"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Build configurations:"
	@echo "  CXX       - C++ compiler (default: g++)"
	@echo "  CXXFLAGS  - Additional compiler flags"
	@echo ""
	@echo "Example: make CXX=clang++ release"

# Dependencies
$(OBJECTS): $(HEADERS)