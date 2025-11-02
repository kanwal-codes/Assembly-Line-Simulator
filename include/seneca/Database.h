#ifndef SENECA_DATABASE_H
#define SENECA_DATABASE_H

#include <string>
#include <memory>
#include <mutex>
#include <vector>

struct sqlite3;

namespace seneca
{
    struct OrderRecord
    {
        std::string customerName;
        std::string product;
        std::string orderId;
        bool isCompleted;
        size_t totalItems;
        size_t filledItems;
        std::string timestamp;
    };

    struct StationRecord
    {
        std::string stationName;
        size_t itemsProcessed;
        size_t inventoryRemaining;
        std::string timestamp;
    };

    class Database
    {
    private:
        static std::unique_ptr<Database> s_instance;
        static std::mutex s_mutex;
        
        sqlite3* m_db;
        std::string m_dbPath;
        bool m_initialized;
        mutable std::string m_lastError;

        Database();
        Database(const Database&) = delete;
        Database& operator=(const Database&) = delete;

    public:
        ~Database();
        
        static Database& getInstance();
        
        // Initialization
        bool initialize(const std::string& dbPath = "database/assembly_line.db");
        void close();
        bool isInitialized() const { return m_initialized; }

        // Schema management
        bool createSchema();
        bool dropSchema();

        // Order operations
        bool saveOrder(const OrderRecord& order);
        bool saveOrderCompletion(const std::string& customerName, 
                                 const std::string& product, 
                                 bool completed,
                                 size_t filledItems,
                                 size_t totalItems);
        std::vector<OrderRecord> getOrderHistory(size_t limit = 100);
        std::vector<OrderRecord> getOrdersByCustomer(const std::string& customerName);
        std::vector<OrderRecord> getCompletedOrders();
        std::vector<OrderRecord> getIncompleteOrders();

        // Station operations
        bool saveStationStatus(const StationRecord& station);
        bool updateStationInventory(const std::string& stationName, size_t inventory);
        std::vector<StationRecord> getStationHistory(const std::string& stationName, size_t limit = 100);

        // Analytics
        size_t getTotalOrdersProcessed();
        size_t getCompletedOrdersCount();
        size_t getIncompleteOrdersCount();
        double getCompletionRate();
        std::string getMostActiveStation();
        std::vector<std::pair<std::string, size_t>> getStationActivityStats();
        
        // Utility
        bool executeQuery(const std::string& query);
        std::string getLastError() const;
    };
} // namespace seneca

#endif // SENECA_DATABASE_H

