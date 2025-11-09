/**
 * @file main.cpp
 * @brief Main entry point for the Assembly Line Simulator
 * 
 * PURPOSE:
 * This is the executable entry point that orchestrates the entire simulation.
 * It initializes all systems, loads data, runs the simulation, and saves results.
 * 
 * FLOW:
 * 1. Initialize infrastructure (Logger, Config, Database)
 * 2. Load station and order data from files
 * 3. Create and run LineManager to process orders
 * 4. Save results to database (if enabled)
 * 5. Display results to console
 * 
 * CONNECTIONS:
 * - Uses Logger singleton for all logging operations
 * - Uses Config singleton to load configuration from config/config.txt
 * - Uses Database singleton to persist simulation results
 * - Creates Workstation objects from data files
 * - Creates CustomerOrder objects from data files
 * - Uses LineManager to orchestrate the assembly line
 * 
 * TRIGGERS:
 * - When executed, this program runs a complete simulation cycle
 * - Database is populated with order and station data
 * - Console output shows completed and incomplete orders
 * 
 * USAGE:
 * ./build/assembly_line Stations1.txt Stations2.txt CustomerOrders.txt AssemblyLine.txt
 */

#include <iostream>
#include <vector>
#include "seneca/Station.h"
#include "seneca/Workstation.h"
#include "seneca/CustomerOrder.h"
#include "seneca/LineManager.h"
#include "seneca/Utilities.h"
#include "seneca/Logger.h"
#include "seneca/Config.h"
#include "seneca/Database.h"
#include "seneca/Exceptions.h"

using namespace seneca;
using seneca::StationRecord;

/**
 * Global order queues - declared in Workstation.h, defined in Workstation.cpp
 * These are shared across all workstations to track order flow:
 * - g_pending: Orders waiting to enter the assembly line
 * - g_completed: Orders that finished successfully
 * - g_incomplete: Orders that couldn't be completed (inventory shortage)
 */

template<typename T>
static void loadFromFile(const char* filename, std::vector<T>& theCollection);
template<typename T>
static void loadFromFile(const char* filename, std::vector<T*>& theCollection);

/**
 * @brief Main function - Entry point for the simulation
 * 
 * EXECUTION FLOW:
 * 1. Initialize infrastructure (Logger, Config, Database)
 * 2. Validate command line arguments (need 4 data files)
 * 3. Load stations from two files (different delimiters)
 * 4. Load customer orders from file
 * 5. Initialize pending queue with all orders
 * 6. Create LineManager and run simulation until complete
 * 7. Save results to database
 * 8. Display results to console
 * 
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments
 * @return 0 on success, 1 on error, 2 on AssemblyLineException
 */
int main(int argc, char** argv)
{
    try
    {
        // ====================================================================
        // STEP 1: Initialize Infrastructure
        // ====================================================================
        // Logger: Thread-safe singleton for all logging operations
        // - Used throughout the program for DEBUG, INFO, WARN, ERROR messages
        // - Can output to console and/or file based on configuration
        // - Log level can be set via config file
        Logger& logger = Logger::getInstance();
        
        // Config: Singleton for application configuration
        // - Loads key-value pairs from config/config.txt
        // - Provides typed access (getString, getInt, getBool, etc.)
        // - Used to configure Logger and Database settings
        // - If config file doesn't exist, uses sensible defaults
        Config& config = Config::getInstance();
        if (config.loadFromFile("config/config.txt"))
        {
            // Configure logger from config
            std::string logLevel = config.getString("log_level", "INFO");
            if (logLevel == "DEBUG") logger.setLogLevel(LogLevel::DEBUG);
            else if (logLevel == "WARN") logger.setLogLevel(LogLevel::WARN);
            else if (logLevel == "ERROR") logger.setLogLevel(LogLevel::ERROR);
            else logger.setLogLevel(LogLevel::INFO);
            
            logger.setLogFile(config.getString("log_file", "logs/assembly_line.log"));
            logger.enableConsoleOutput(config.getBool("log_console", true));
            logger.enableFileOutput(config.getBool("log_file_enabled", false));
        }
        else
        {
            // Use defaults
            logger.setLogLevel(LogLevel::INFO);
            logger.enableConsoleOutput(true);
        }

        // Database: Singleton for SQLite database operations
        // - Stores simulation results (orders, stations) for API access
        // - Creates tables automatically on first initialization
        // - Used by Python API (api/main.py) to serve data to frontend
        // - If initialization fails, simulation continues without database
        // - Database path is resolved relative to project root
        Database& db = Database::getInstance();
        if (config.getBool("enable_database", true))
        {
            std::string dbPath = config.getString("database_path", "database/assembly_line.db");
            if (!db.initialize(dbPath))
            {
                LOG_WARN("Database initialization failed, continuing without database");
            }
            else
            {
                LOG_INFO("Database initialized successfully");
            }
        }
        else
        {
            LOG_INFO("Database disabled in configuration");
        }

        LOG_INFO("=== Assembly Line Simulator Starting ===");
        LOG_INFO("Command Line: " + std::string(argv[0]));
        for (int i = 1; i < argc; ++i)
        {
            LOG_DEBUG("Argument " + std::to_string(i) + ": " + argv[i]);
        }

        if (argc != 5)
        {
            LOG_ERROR("Incorrect number of arguments. Expected 4 data files.");
            std::cerr << "Usage: " << argv[0] << " <Stations1.txt> <Stations2.txt> <CustomerOrders.txt> <AssemblyLine.txt>" << std::endl;
            return 1;
        }

        // ====================================================================
        // STEP 2: Load Data Files
        // ====================================================================
        std::vector<Workstation*> theStations;  // All workstations in the assembly line
        std::vector<CustomerOrder> theOrders;   // All customer orders to process

        // Load stations from two files with different delimiters
        // - Stations1.txt uses comma (,) delimiter
        // - Stations2.txt uses pipe (|) delimiter
        // - Each station represents a processing point in the assembly line
        // - Workstation objects are created on the heap (pointers)
        LOG_INFO("Loading stations from: " + std::string(argv[1]) + " and " + std::string(argv[2]));
        Utilities::setDelimiter(',');  // Set delimiter for first file
        loadFromFile(argv[1], theStations);
        Utilities::setDelimiter('|');  // Change delimiter for second file
        loadFromFile(argv[2], theStations);
        LOG_INFO("Loaded " + std::to_string(theStations.size()) + " stations");

        // Load customer orders from file
        // - Each order specifies a customer, product, and list of items needed
        // - Orders are stored by value (not pointers) in the vector
        LOG_INFO("Loading customer orders from: " + std::string(argv[3]));
        loadFromFile<CustomerOrder>(argv[3], theOrders);
        LOG_INFO("Loaded " + std::to_string(theOrders.size()) + " customer orders");

        // ====================================================================
        // STEP 3: Initialize Order Queue
        // ====================================================================
        // Move all orders into the global pending queue
        // - g_pending is accessed by LineManager and Workstations
        // - Orders are moved (not copied) for efficiency
        // - This queue is processed by the assembly line
        for (auto& order : theOrders)
        {
            g_pending.push_back(std::move(order));
        }

        // ====================================================================
        // STEP 4: Run Simulation
        // ====================================================================
        // LineManager orchestrates the entire assembly line
        // - Links workstations together based on AssemblyLine.txt
        // - Processes orders through the line until all are done
        // - run() returns true when simulation is complete
        // - Each call to run() processes one cycle (one order movement per station)
        LOG_INFO("Initializing assembly line from: " + std::string(argv[4]));
        LineManager lm(argv[4], theStations);
        
        LOG_INFO("Starting simulation...");
        while (!lm.run(std::cout))  // Continue until run() returns true (simulation complete)
        {
            // Each iteration processes one cycle of the assembly line
            // Orders move through stations, get processed, and eventually complete or fail
        }

        LOG_INFO("=== Simulation Complete ===");
        LOG_INFO("Completed orders: " + std::to_string(g_completed.size()));
        LOG_INFO("Incomplete orders: " + std::to_string(g_incomplete.size()));

        // ====================================================================
        // STEP 5: Save Results to Database
        // ====================================================================
        // Database persistence enables the Python API to serve data to frontend
        // - Completed orders: Successfully processed orders
        // - Incomplete orders: Orders that couldn't be finished (inventory shortage)
        // - Station data: Current state of each workstation
        // - This data is then accessible via REST API endpoints
        if (db.isInitialized())
        {
            LOG_INFO("Saving orders to database...");
            size_t savedCount = 0;
            size_t skippedCount = 0;
            
            // Save completed orders
            // - These orders finished successfully through the assembly line
            // - Data is used by API endpoint GET /orders/completed
            // - Frontend displays these in the Orders page
            for (const auto& order : g_completed)
            {
                if (db.saveOrderCompletion(
                    order.getCustomerName(),
                    order.getProduct(),
                    true,  // is_completed = true
                    order.getFilledItemCount(),
                    order.getItemCount()
                )) {
                    savedCount++;
                } else {
                    skippedCount++;
                    LOG_DEBUG("Failed to save order: " + order.getCustomerName() + " - " + order.getProduct());
                }
            }
            
            // Save incomplete orders
            // - These orders couldn't be completed due to inventory shortage
            // - Data is used by API endpoint GET /orders/incomplete
            // - Frontend can filter to show only incomplete orders
            for (const auto& order : g_incomplete)
            {
                if (db.saveOrderCompletion(
                    order.getCustomerName(),
                    order.getProduct(),
                    false,  // is_completed = false
                    order.getFilledItemCount(),
                    order.getItemCount()
                )) {
                    savedCount++;
                } else {
                    skippedCount++;
                    LOG_DEBUG("Failed to save order: " + order.getCustomerName() + " - " + order.getProduct());
                }
            }
            
            LOG_INFO("Saved " + std::to_string(savedCount) + " orders, skipped " + std::to_string(skippedCount));
            
            // Save station data to database
            // - Station inventory and status information
            // - Used by API endpoint GET /stations
            // - Frontend displays station data in Stations page
            LOG_INFO("Saving station data to database...");
            size_t stationsSaved = 0;
            for (auto* station : theStations)
            {
                StationRecord stationRecord;
                stationRecord.stationName = station->getItemName();
                stationRecord.itemsProcessed = 0; // TODO: Track items processed during simulation
                stationRecord.inventoryRemaining = station->getQuantity();
                stationRecord.timestamp = ""; // Will be auto-generated by saveStationStatus
                
                if (db.saveStationStatus(stationRecord))
                {
                    stationsSaved++;
                    LOG_DEBUG("Saved station: " + stationRecord.stationName + 
                             " (inventory: " + std::to_string(stationRecord.inventoryRemaining) + ")");
                }
                else
                {
                    LOG_WARN("Failed to save station: " + stationRecord.stationName + 
                            " - " + db.getLastError());
                }
            }
            LOG_INFO("Saved " + std::to_string(stationsSaved) + " stations");
            
            // Display database statistics
            // - Shows total orders processed across all simulation runs
            // - Completion rate is calculated from database data
            // - This data is also available via API endpoint GET /stats
            size_t total = db.getTotalOrdersProcessed();
            double rate = db.getCompletionRate();
            LOG_INFO("Database Statistics - Total: " + std::to_string(total) + 
                     ", Completion Rate: " + std::to_string(rate) + "%");
        }

        // Display results
        std::cout << "\n========================================" << std::endl;
        std::cout << "=      Processed Orders (complete)     =" << std::endl;
        std::cout << "========================================" << std::endl;
        for (const auto& o : g_completed)
        {
            o.display(std::cout);
        }

        std::cout << "\n========================================" << std::endl;
        std::cout << "=     Processed Orders (incomplete)    =" << std::endl;
        std::cout << "========================================" << std::endl;
        for (const auto& o : g_incomplete)
        {
            o.display(std::cout);
        }

        // Cleanup
        for (auto station : theStations)
        {
            delete station;
        }
    }
    catch (const AssemblyLineException& e)
    {
        LOG_ERROR("Assembly Line Exception: " + std::string(e.what()));
        std::cerr << e.what() << std::endl;
        return 2;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Standard Exception: " + std::string(e.what()));
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

/**
 * @brief Template function to load objects from a file into a vector (by value)
 * 
 * PURPOSE:
 * Generic file loader that creates objects from file records and stores them in a vector.
 * Used for loading CustomerOrder objects (stored by value).
 * 
 * HOW IT WORKS:
 * 1. Opens the specified file
 * 2. Reads each line as a record
 * 3. Creates an object of type T from the record (calls T(record) constructor)
 * 4. Moves the object into the collection vector
 * 
 * TRIGGERS:
 * - Called to load CustomerOrder objects from CustomerOrders.txt
 * - Each line becomes one CustomerOrder object
 * 
 * @tparam T Type of object to create (must have constructor T(const std::string&))
 * @param filename Path to the data file
 * @param theCollection Vector to populate with loaded objects
 * @throws FileException if file cannot be opened or filename is null
 */
template<typename T>
static void loadFromFile(const char* filename, std::vector<T>& theCollection)
{
    if (!filename)
    {
        throw FileException("No filename provided");
    }
    
    std::ifstream file(filename);
    if (!file)
    {
        throw FileException("Unable to open file: " + std::string(filename));
    }

    std::string record;
    while (!file.eof())
    {
        std::getline(file, record);
        if (!record.empty())
        {
            T elem(record);  // Create object from record string
            theCollection.push_back(std::move(elem));  // Move into vector (efficient)
        }
    }
    file.close();
}

/**
 * @brief Template function to load objects from a file into a vector (by pointer)
 * 
 * PURPOSE:
 * Generic file loader that creates objects on the heap and stores pointers in a vector.
 * Used for loading Workstation objects (stored as pointers, managed manually).
 * 
 * HOW IT WORKS:
 * 1. Opens the specified file
 * 2. Reads each line as a record
 * 3. Creates an object of type T on the heap (new T(record))
 * 4. Stores the pointer in the collection vector
 * 
 * TRIGGERS:
 * - Called to load Workstation objects from Stations1.txt and Stations2.txt
 * - Each line becomes one Workstation object on the heap
 * - Caller is responsible for deleting these objects (done in main cleanup)
 * 
 * @tparam T Type of object to create (must have constructor T(const std::string&))
 * @param filename Path to the data file
 * @param theCollection Vector to populate with object pointers
 * @throws FileException if file cannot be opened or filename is null
 */
template<typename T>
static void loadFromFile(const char* filename, std::vector<T*>& theCollection)
{
    if (!filename)
    {
        throw FileException("No filename provided");
    }
    
    std::ifstream file(filename);
    if (!file)
    {
        throw FileException("Unable to open file: " + std::string(filename));
    }

    std::string record;
    while (!file.eof())
    {
        std::getline(file, record);
        if (!record.empty())
        {
            theCollection.push_back(new T(record));  // Create on heap, store pointer
        }
    }
    file.close();
}

