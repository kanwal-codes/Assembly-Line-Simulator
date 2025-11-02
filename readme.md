Assembly Line Simulator

## Overview
This repository hosts a comprehensive C++ project developed during an advanced Object-Oriented Programming course. The project simulates an assembly line, highlighting intricate OOP concepts and demonstrating proficiency in C++ programming.

**🚀 Enhanced Version**: This project has been expanded with professional features including logging, configuration management, custom exception handling, and a complete main executable. See `IMPLEMENTATION_STATUS.md` for current features and `EXPANSION_PLAN.md` for future enhancements.

## Project Description
The simulation models a manufacturing assembly line where customer orders are processed sequentially through various workstations. Each workstation in the assembly line is responsible for adding a specific component to an order, mimicking real-world factory production lines.

## How It Works
The simulation orchestrates the processing of customer orders through a series of workstations, each responsible for a specific task. Orders move dynamically through the assembly line, with workstations processing their respective parts based on inventory availability.

### Key Modules and Functionalities
1. **Utilities Module**:
   - `extractToken`: Parses input data, extracting tokens for initializing other objects.
   - `setFieldWidth`: Adjusts the field width for displaying station information.

2. **Station Class**:
   - Represents an individual station in the assembly line.
   - Manages inventory and serial number generation for items processed at the station.
   - Key Functions:
     - `fillOrder`: Processes an order, if the station's item is required.
     - `nextSerialNumber`: Provides the next serial number for an item processed at the station.

3. **CustomerOrder Class**:
   - Manages the details of a customer order, tracking the list of items and their fulfillment status.
   - Key Functions:
     - `fillItem`: Checks and fills an item in the order if the current workstation has it.
     - `isOrderFilled`: Verifies if all items in the order are filled.

4. **Workstation Class** (Inherits from Station):
   - Represents a working unit in the assembly line, processing specific parts of orders.
   - Manages the queue of orders and links to the next workstation.
   - Key Functions:
     - `attemptToMoveOrder`: Moves the order to the next workstation if processing is complete.

5. **LineManager Class**:
   - Orchestrates the entire assembly line, linking workstations and managing the flow of orders.
   - Key Functions:
     - `run`: Executes one cycle of the line, processing and moving orders through workstations.

## New Features (Enhanced Version)

### Logger System
- Thread-safe singleton logger
- Multiple log levels (DEBUG, INFO, WARN, ERROR)
- Console and file output support
- Timestamped entries
- Configurable via config file

### Configuration Management
- Key-value configuration file parser
- Support for strings, integers, booleans, doubles
- Default value support
- Configuration validation

### Exception Handling
- Hierarchical exception structure
- Specialized exceptions for different error types
- Better error messages and debugging

## Building

```bash
# Build release version
make release

# Run the simulation
./build/assembly_line data/Stations1.txt data/Stations2.txt data/CustomerOrders.txt data/AssemblyLine.txt

# Run tests
make test3
```

See `docs/guides/QUICK_START.md` for detailed instructions and `docs/planning/EXPANSION_PLAN.md` for future enhancements.
