#ifndef SENECA_CONFIG_H
#define SENECA_CONFIG_H

#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <memory>
#include <mutex>

namespace seneca
{
    class Config
    {
    private:
        static std::unique_ptr<Config> s_instance;
        static std::mutex s_mutex;
        std::unordered_map<std::string, std::string> m_config;
        std::string m_configFile;

        Config();
        
        // Delete copy constructor and assignment operator
        Config(const Config&) = delete;
        Config& operator=(const Config&) = delete;

    public:
        ~Config() = default;
        static Config& getInstance();
        
        // Load configuration from file (simple key=value format)
        bool loadFromFile(const std::string& filename);
        
        // Get configuration values
        std::string getString(const std::string& key, const std::string& defaultValue = "") const;
        int getInt(const std::string& key, int defaultValue = 0) const;
        bool getBool(const std::string& key, bool defaultValue = false) const;
        double getDouble(const std::string& key, double defaultValue = 0.0) const;

        // Set configuration values
        void setString(const std::string& key, const std::string& value);
        void setInt(const std::string& key, int value);
        void setBool(const std::string& key, bool value);
        void setDouble(const std::string& key, double value);

        // Save configuration to file
        bool saveToFile(const std::string& filename = "") const;

        // Check if key exists
        bool hasKey(const std::string& key) const;

        // Clear all configuration
        void clear();
    };
} // namespace seneca

#endif // SENECA_CONFIG_H

