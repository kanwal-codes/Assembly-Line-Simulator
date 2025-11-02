# Quick Start Guide - Assembly Line Simulator

## Prerequisites

- C++17 compatible compiler (g++, clang++)
- Make (or CMake)
- Standard C++ libraries

## Building the Project

### Using Make (Recommended)
```bash
# Build release version
make release

# Build debug version (with sanitizers)
make debug

# Run all tests
make test

# Run specific test suites
make test1  # Station and Utilities tests
make test2  # CustomerOrder tests  
make test3  # Full system integration tests

# Clean build artifacts
make clean
```

### Using CMake
```bash
mkdir build && cd build
cmake ..
make

# Run tests
ctest --output-on-failure
```

## Running Tests

The project includes three comprehensive test suites:

1. **Test 1** (`tester_1.cpp`): Tests `Station` and `Utilities` classes
   ```bash
   make test1
   ```

2. **Test 2** (`tester_2.cpp`): Tests `CustomerOrder` with move semantics
   ```bash
   make test2
   ```

3. **Test 3** (`tester_3.cpp`): Full system integration test
   ```bash
   make test3
   ```

## Project Structure

```
Assembly-Line-Simulator/
├── src/                    # Source code
│   ├── Station.h/cpp       # Base station class
│   ├── Workstation.h/cpp   # Workstation (inherits Station)
│   ├── CustomerOrder.h/cpp # Order management
│   ├── LineManager.h/cpp   # Assembly line orchestrator
│   ├── Utilities.h/cpp     # Parsing utilities
│   └── main.cpp            # (Currently empty)
├── data/                   # Input data files
│   ├── Stations1.txt       # Station config (comma-delimited)
│   ├── Stations2.txt       # Station config (pipe-delimited)
│   ├── CustomerOrders.txt  # Order data
│   └── AssemblyLine.txt    # Line configuration
├── tests/                  # Test files
├── Makefile                # Build configuration
└── CMakeLists.txt          # CMake configuration
```

## Data Format

### Stations File
```
ItemName, SerialNumber, Quantity, Description
```
Example: `Armchair, 654321, 10, Upholstered Wing Chair`

### Customer Orders File
```
CustomerName | Product | Item1|Item2|Item3...
```
Example: `Cornel B. | 1-Room Home Office | Office Chair|Desk|Bookcase`

### Assembly Line File
```
StationName | NextStationName
```
Example: `Nighttable | Desk` (Nighttable is followed by Desk)

## Understanding the Simulation

1. **Stations** are loaded from data files with inventory and serial numbers
2. **Customer Orders** are parsed and added to the pending queue
3. **LineManager** creates the assembly line by linking workstations
4. **Simulation runs** in cycles:
   - Orders move from `g_pending` to first station
   - Each station processes front order in its queue
   - Orders move to next station when item is filled or station is empty
   - Completed orders go to `g_completed` or `g_incomplete`

## Current Features

✅ Modular OOP design with inheritance  
✅ Move semantics for efficient order transfer  
✅ Dynamic memory management  
✅ STL containers (deque, vector)  
✅ Multiple delimiter support (comma, pipe)  
✅ Comprehensive test suite  

## Next Steps

See `EXPANSION_PLAN.md` for detailed enhancement roadmap to make this project LinkedIn-worthy!
