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

// Note: g_pending, g_completed, g_incomplete are declared in Workstation.h
// and defined in Workstation.cpp

template<typename T>
static void loadFromFile(const char* filename, std::vector<T>& theCollection);
template<typename T>
static void loadFromFile(const char* filename, std::vector<T*>& theCollection);

int main(int argc, char** argv)
{
    try
    {
        // Initialize logger
        Logger& logger = Logger::getInstance();
        
        // Load configuration
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

        // Initialize database after config is loaded
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

        std::vector<Workstation*> theStations;
        std::vector<CustomerOrder> theOrders;

        // Load stations
        LOG_INFO("Loading stations from: " + std::string(argv[1]) + " and " + std::string(argv[2]));
        Utilities::setDelimiter(',');
        loadFromFile(argv[1], theStations);
        Utilities::setDelimiter('|');
        loadFromFile(argv[2], theStations);
        LOG_INFO("Loaded " + std::to_string(theStations.size()) + " stations");

        // Load customer orders
        LOG_INFO("Loading customer orders from: " + std::string(argv[3]));
        loadFromFile<CustomerOrder>(argv[3], theOrders);
        LOG_INFO("Loaded " + std::to_string(theOrders.size()) + " customer orders");

        // Initialize pending queue
        for (auto& order : theOrders)
        {
            g_pending.push_back(std::move(order));
        }

        // Create and run line manager
        LOG_INFO("Initializing assembly line from: " + std::string(argv[4]));
        LineManager lm(argv[4], theStations);
        
        LOG_INFO("Starting simulation...");
        while (!lm.run(std::cout))
        {
            // Continue until all orders processed
        }

        LOG_INFO("=== Simulation Complete ===");
        LOG_INFO("Completed orders: " + std::to_string(g_completed.size()));
        LOG_INFO("Incomplete orders: " + std::to_string(g_incomplete.size()));

        // Save orders to database
        if (db.isInitialized())
        {
            LOG_INFO("Saving orders to database...");
            size_t savedCount = 0;
            size_t skippedCount = 0;
            
            for (const auto& order : g_completed)
            {
                if (db.saveOrderCompletion(
                    order.getCustomerName(),
                    order.getProduct(),
                    true,
                    order.getFilledItemCount(),
                    order.getItemCount()
                )) {
                    savedCount++;
                } else {
                    skippedCount++;
                    LOG_DEBUG("Failed to save order: " + order.getCustomerName() + " - " + order.getProduct());
                }
            }
            for (const auto& order : g_incomplete)
            {
                if (db.saveOrderCompletion(
                    order.getCustomerName(),
                    order.getProduct(),
                    false,
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
            T elem(record);
            theCollection.push_back(std::move(elem));
        }
    }
    file.close();
}

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
            theCollection.push_back(new T(record));
        }
    }
    file.close();
}

