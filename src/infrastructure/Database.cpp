#include "seneca/Database.h"
#include "seneca/Logger.h"
#include "seneca/Exceptions.h"
#include <sqlite3.h>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <algorithm>
#include <unistd.h>
#include <limits.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

namespace seneca
{
    std::unique_ptr<Database> Database::s_instance = nullptr;
    std::mutex Database::s_mutex;

    Database::Database()
        : m_db(nullptr)
        , m_dbPath("")  // Will be set by initialize
        , m_initialized(false)
    {
    }

    Database::~Database()
    {
        close();
    }

    Database& Database::getInstance()
    {
        std::lock_guard<std::mutex> lock(s_mutex);
        if (s_instance == nullptr)
        {
            s_instance = std::unique_ptr<Database>(new Database());
        }
        return *s_instance;
    }

    bool Database::initialize(const std::string& dbPath)
    {
        std::lock_guard<std::mutex> lock(s_mutex);
        
        if (m_initialized)
        {
            close();
        }

        // Convert relative path to absolute path from project root
        std::string finalPath = dbPath;
        
        if (finalPath.empty())
        {
            finalPath = "database/assembly_line.db";
        }
        
        // If relative path, resolve it relative to project root
        // The executable runs from build/, so we need to go up one level for project root
        if (finalPath[0] != '/')
        {
            // Check if database exists in current directory
            if (access(finalPath.c_str(), F_OK) != 0)
            {
                // Try one level up (from build/ to project root)
                std::string upLevel = "../" + finalPath;
                if (access(upLevel.c_str(), F_OK) == 0)
                {
                    finalPath = upLevel;
                }
                // If still doesn't exist, we'll create it at the specified path
            }
            
            // Convert to absolute path
            char resolved[PATH_MAX];
            if (realpath(finalPath.c_str(), resolved) != nullptr)
            {
                finalPath = resolved;
            }
            else
            {
                // Path doesn't exist yet, resolve from current directory
                char cwd[PATH_MAX];
                if (getcwd(cwd, sizeof(cwd)) != nullptr)
                {
                    std::string currentDir = cwd;
                    // If in build/, go up one level
                    if (currentDir.find("/build") != std::string::npos)
                    {
                        size_t buildPos = currentDir.find("/build");
                        std::string projectRoot = currentDir.substr(0, buildPos);
                        finalPath = projectRoot + "/" + (dbPath.empty() ? "database/assembly_line.db" : dbPath);
                    }
                    else
                    {
                        finalPath = currentDir + "/" + (dbPath.empty() ? "database/assembly_line.db" : dbPath);
                    }
                }
            }
        }
        
        m_dbPath = finalPath;
        LOG_INFO("Database path resolved to: " + m_dbPath);
        
        // Create database directory if it doesn't exist
        size_t lastSlash = m_dbPath.find_last_of("/");
        if (lastSlash != std::string::npos)
        {
            std::string dir = m_dbPath.substr(0, lastSlash);
            std::string cmd = "mkdir -p \"" + dir + "\"";
            system(cmd.c_str());
        }

        int rc = sqlite3_open(m_dbPath.c_str(), &m_db);
        if (rc)
        {
            m_lastError = sqlite3_errmsg(m_db);
            LOG_ERROR("Cannot open database: " + m_lastError);
            sqlite3_close(m_db);
            m_db = nullptr;
            return false;
        }

        m_initialized = true;
        LOG_INFO("Database initialized: " + m_dbPath);

        if (!createSchema())
        {
            LOG_ERROR("Failed to create database schema");
            return false;
        }

        return true;
    }

    void Database::close()
    {
        if (m_db)
        {
            sqlite3_close(m_db);
            m_db = nullptr;
        }
        m_initialized = false;
    }

    bool Database::createSchema()
    {
        if (!m_db) return false;

        std::string createOrdersTable = R"(
            CREATE TABLE IF NOT EXISTS orders (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                order_id TEXT UNIQUE NOT NULL,
                customer_name TEXT NOT NULL,
                product TEXT NOT NULL,
                is_completed INTEGER NOT NULL DEFAULT 0,
                total_items INTEGER NOT NULL DEFAULT 0,
                filled_items INTEGER NOT NULL DEFAULT 0,
                created_at TEXT NOT NULL,
                completed_at TEXT
            )
        )";

        std::string createStationsTable = R"(
            CREATE TABLE IF NOT EXISTS station_history (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                station_name TEXT NOT NULL,
                items_processed INTEGER NOT NULL DEFAULT 0,
                inventory_remaining INTEGER NOT NULL DEFAULT 0,
                timestamp TEXT NOT NULL
            )
        )";

        std::string createIndex1 = "CREATE INDEX IF NOT EXISTS idx_orders_customer ON orders(customer_name)";
        std::string createIndex2 = "CREATE INDEX IF NOT EXISTS idx_orders_completed ON orders(is_completed)";
        std::string createIndex3 = "CREATE INDEX IF NOT EXISTS idx_stations_name ON station_history(station_name)";

        char* errMsg = nullptr;
        
        if (sqlite3_exec(m_db, createOrdersTable.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
        {
            m_lastError = errMsg ? errMsg : "Unknown error";
            sqlite3_free(errMsg);
            return false;
        }

        if (sqlite3_exec(m_db, createStationsTable.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
        {
            m_lastError = errMsg ? errMsg : "Unknown error";
            sqlite3_free(errMsg);
            return false;
        }

        sqlite3_exec(m_db, createIndex1.c_str(), nullptr, nullptr, nullptr);
        sqlite3_exec(m_db, createIndex2.c_str(), nullptr, nullptr, nullptr);
        sqlite3_exec(m_db, createIndex3.c_str(), nullptr, nullptr, nullptr);

        return true;
    }

    bool Database::dropSchema()
    {
        if (!m_db) return false;

        std::string dropOrders = "DROP TABLE IF EXISTS orders";
        std::string dropStations = "DROP TABLE IF EXISTS station_history";

        char* errMsg = nullptr;
        sqlite3_exec(m_db, dropOrders.c_str(), nullptr, nullptr, &errMsg);
        if (errMsg) sqlite3_free(errMsg);
        
        sqlite3_exec(m_db, dropStations.c_str(), nullptr, nullptr, &errMsg);
        if (errMsg) sqlite3_free(errMsg);

        return createSchema();
    }

    static std::string getCurrentTimestamp()
    {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    bool Database::saveOrder(const OrderRecord& order)
    {
        if (!m_db) return false;

        std::stringstream ss;
        // Use INSERT OR IGNORE to prevent duplicates, or just INSERT for new entries
        // Since order_id should be unique, we'll use INSERT (not REPLACE) to allow multiple runs
        ss << "INSERT INTO orders (order_id, customer_name, product, is_completed, "
           << "total_items, filled_items, created_at, completed_at) VALUES (?, ?, ?, ?, ?, ?, ?, ?)";

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(m_db, ss.str().c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        {
            m_lastError = sqlite3_errmsg(m_db);
            return false;
        }

        sqlite3_bind_text(stmt, 1, order.orderId.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, order.customerName.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, order.product.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 4, order.isCompleted ? 1 : 0);
        sqlite3_bind_int(stmt, 5, static_cast<int>(order.totalItems));
        sqlite3_bind_int(stmt, 6, static_cast<int>(order.filledItems));
        sqlite3_bind_text(stmt, 7, order.timestamp.c_str(), -1, SQLITE_STATIC);
        
        std::string completedAt = order.isCompleted ? getCurrentTimestamp() : "";
        sqlite3_bind_text(stmt, 8, completedAt.c_str(), -1, SQLITE_STATIC);

        int stepResult = sqlite3_step(stmt);
        bool success = (stepResult == SQLITE_DONE);
        
        if (!success && stepResult == SQLITE_CONSTRAINT)
        {
            // Order ID already exists - this is OK for duplicate simulations
            m_lastError = "Order ID already exists (duplicate simulation run)";
            LOG_DEBUG("Order ID already exists (skipping): " + order.orderId);
        }
        else if (!success)
        {
            m_lastError = sqlite3_errmsg(m_db);
            LOG_ERROR("Failed to save order: " + m_lastError);
        }
        else
        {
            LOG_DEBUG("Order saved: " + order.customerName + " - " + order.product);
        }
        
        sqlite3_finalize(stmt);
        return success;
    }

    bool Database::saveOrderCompletion(const std::string& customerName,
                                       const std::string& product,
                                       bool completed,
                                       size_t filledItems,
                                       size_t totalItems)
    {
        OrderRecord record;
        record.customerName = customerName;
        record.product = product;
        // Generate unique order_id with high-precision timestamp + random component
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(
            now.time_since_epoch()).count();
        // Add nanoseconds for extra uniqueness and a small random component
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
            now.time_since_epoch()).count();
        // Use nanoseconds + microseconds to ensure uniqueness even in rapid succession
        record.orderId = customerName + "_" + product + "_" + std::to_string(ns) + "_" + std::to_string(us);
        record.isCompleted = completed;
        record.filledItems = filledItems;
        record.totalItems = totalItems;
        record.timestamp = getCurrentTimestamp();

        return saveOrder(record);
    }

    static int orderCallback(void* data, int argc, char** argv, char** colNames)
    {
        std::vector<OrderRecord>* orders = static_cast<std::vector<OrderRecord>*>(data);
        OrderRecord record;
        
        for (int i = 0; i < argc; i++)
        {
            std::string colName = colNames[i];
            std::string value = argv[i] ? argv[i] : "";
            
            if (colName == "order_id") record.orderId = value;
            else if (colName == "customer_name") record.customerName = value;
            else if (colName == "product") record.product = value;
            else if (colName == "is_completed") record.isCompleted = (value == "1");
            else if (colName == "total_items") record.totalItems = std::stoul(value);
            else if (colName == "filled_items") record.filledItems = std::stoul(value);
            else if (colName == "created_at" || colName == "completed_at") record.timestamp = value;
        }
        
        orders->push_back(record);
        return 0;
    }

    std::vector<OrderRecord> Database::getOrderHistory(size_t limit)
    {
        std::vector<OrderRecord> orders;
        if (!m_db) return orders;

        std::stringstream ss;
        ss << "SELECT * FROM orders ORDER BY created_at DESC LIMIT " << limit;

        char* errMsg = nullptr;
        sqlite3_exec(m_db, ss.str().c_str(), orderCallback, &orders, &errMsg);
        
        if (errMsg)
        {
            m_lastError = errMsg;
            sqlite3_free(errMsg);
        }

        return orders;
    }

    std::vector<OrderRecord> Database::getOrdersByCustomer(const std::string& customerName)
    {
        std::vector<OrderRecord> orders;
        if (!m_db) return orders;

        std::stringstream ss;
        ss << "SELECT * FROM orders WHERE customer_name = ? ORDER BY created_at DESC";

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(m_db, ss.str().c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_text(stmt, 1, customerName.c_str(), -1, SQLITE_STATIC);
            
            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                OrderRecord record;
                record.orderId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                record.customerName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                record.product = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
                record.isCompleted = (sqlite3_column_int(stmt, 4) == 1);
                record.totalItems = sqlite3_column_int(stmt, 5);
                record.filledItems = sqlite3_column_int(stmt, 6);
                record.timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
                orders.push_back(record);
            }
            sqlite3_finalize(stmt);
        }

        return orders;
    }

    std::vector<OrderRecord> Database::getCompletedOrders()
    {
        std::vector<OrderRecord> orders;
        if (!m_db) return orders;

        std::string query = "SELECT * FROM orders WHERE is_completed = 1 ORDER BY completed_at DESC";
        char* errMsg = nullptr;
        sqlite3_exec(m_db, query.c_str(), orderCallback, &orders, &errMsg);
        if (errMsg) sqlite3_free(errMsg);

        return orders;
    }

    std::vector<OrderRecord> Database::getIncompleteOrders()
    {
        std::vector<OrderRecord> orders;
        if (!m_db) return orders;

        std::string query = "SELECT * FROM orders WHERE is_completed = 0 ORDER BY created_at DESC";
        char* errMsg = nullptr;
        sqlite3_exec(m_db, query.c_str(), orderCallback, &orders, &errMsg);
        if (errMsg) sqlite3_free(errMsg);

        return orders;
    }

    bool Database::saveStationStatus(const StationRecord& station)
    {
        if (!m_db) return false;

        std::stringstream ss;
        ss << "INSERT INTO station_history (station_name, items_processed, inventory_remaining, timestamp) "
           << "VALUES (?, ?, ?, ?)";

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(m_db, ss.str().c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        {
            m_lastError = sqlite3_errmsg(m_db);
            return false;
        }

        sqlite3_bind_text(stmt, 1, station.stationName.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, static_cast<int>(station.itemsProcessed));
        sqlite3_bind_int(stmt, 3, static_cast<int>(station.inventoryRemaining));
        
        // Use provided timestamp or generate current one
        std::string timestamp = station.timestamp.empty() ? getCurrentTimestamp() : station.timestamp;
        sqlite3_bind_text(stmt, 4, timestamp.c_str(), -1, SQLITE_STATIC);

        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        if (!success)
        {
            m_lastError = sqlite3_errmsg(m_db);
        }
        sqlite3_finalize(stmt);

        return success;
    }

    bool Database::updateStationInventory(const std::string& stationName, size_t inventory)
    {
        StationRecord record;
        record.stationName = stationName;
        record.itemsProcessed = 0; // Would need to track this separately
        record.inventoryRemaining = inventory;
        record.timestamp = getCurrentTimestamp();

        return saveStationStatus(record);
    }

    std::vector<StationRecord> Database::getStationHistory(const std::string& stationName, size_t limit)
    {
        std::vector<StationRecord> records;
        if (!m_db) return records;

        std::stringstream ss;
        ss << "SELECT * FROM station_history WHERE station_name = ? ORDER BY timestamp DESC LIMIT " << limit;

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(m_db, ss.str().c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_text(stmt, 1, stationName.c_str(), -1, SQLITE_STATIC);
            
            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                StationRecord record;
                record.stationName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                record.itemsProcessed = sqlite3_column_int(stmt, 2);
                record.inventoryRemaining = sqlite3_column_int(stmt, 3);
                record.timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
                records.push_back(record);
            }
            sqlite3_finalize(stmt);
        }

        return records;
    }

    size_t Database::getTotalOrdersProcessed()
    {
        if (!m_db) return 0;

        std::string query = "SELECT COUNT(*) FROM orders";
        sqlite3_stmt* stmt;
        
        if (sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            if (sqlite3_step(stmt) == SQLITE_ROW)
            {
                size_t count = sqlite3_column_int(stmt, 0);
                sqlite3_finalize(stmt);
                return count;
            }
            sqlite3_finalize(stmt);
        }
        return 0;
    }

    size_t Database::getCompletedOrdersCount()
    {
        if (!m_db) return 0;

        std::string query = "SELECT COUNT(*) FROM orders WHERE is_completed = 1";
        sqlite3_stmt* stmt;
        
        if (sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            if (sqlite3_step(stmt) == SQLITE_ROW)
            {
                size_t count = sqlite3_column_int(stmt, 0);
                sqlite3_finalize(stmt);
                return count;
            }
            sqlite3_finalize(stmt);
        }
        return 0;
    }

    size_t Database::getIncompleteOrdersCount()
    {
        return getTotalOrdersProcessed() - getCompletedOrdersCount();
    }

    double Database::getCompletionRate()
    {
        size_t total = getTotalOrdersProcessed();
        if (total == 0) return 0.0;
        
        size_t completed = getCompletedOrdersCount();
        return static_cast<double>(completed) / static_cast<double>(total) * 100.0;
    }

    std::string Database::getMostActiveStation()
    {
        if (!m_db) return "";

        std::string query = R"(
            SELECT station_name, SUM(items_processed) as total
            FROM station_history
            GROUP BY station_name
            ORDER BY total DESC
            LIMIT 1
        )";

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            if (sqlite3_step(stmt) == SQLITE_ROW)
            {
                std::string station = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                sqlite3_finalize(stmt);
                return station;
            }
            sqlite3_finalize(stmt);
        }
        return "";
    }

    std::vector<std::pair<std::string, size_t>> Database::getStationActivityStats()
    {
        std::vector<std::pair<std::string, size_t>> stats;
        if (!m_db) return stats;

        std::string query = R"(
            SELECT station_name, SUM(items_processed) as total
            FROM station_history
            GROUP BY station_name
            ORDER BY total DESC
        )";

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                std::string station = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                size_t total = sqlite3_column_int(stmt, 1);
                stats.push_back({station, total});
            }
            sqlite3_finalize(stmt);
        }

        return stats;
    }

    bool Database::executeQuery(const std::string& query)
    {
        if (!m_db) return false;

        char* errMsg = nullptr;
        int rc = sqlite3_exec(m_db, query.c_str(), nullptr, nullptr, &errMsg);
        
        if (rc != SQLITE_OK)
        {
            m_lastError = errMsg ? errMsg : "Unknown error";
            sqlite3_free(errMsg);
            return false;
        }

        return true;
    }

    std::string Database::getLastError() const
    {
        return m_lastError;
    }
} // namespace seneca

