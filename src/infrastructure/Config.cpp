#include "seneca/Config.h"
#include "seneca/Logger.h"
#include <algorithm>
#include <cctype>

namespace seneca
{
    std::unique_ptr<Config> Config::s_instance = nullptr;
    std::mutex Config::s_mutex;

    Config::Config()
        : m_configFile("config.txt")
    {
    }

    Config& Config::getInstance()
    {
        std::lock_guard<std::mutex> lock(s_mutex);
        if (s_instance == nullptr)
        {
            s_instance = std::unique_ptr<Config>(new Config());
        }
        return *s_instance;
    }

    bool Config::loadFromFile(const std::string& filename)
    {
        m_configFile = filename;
        std::ifstream file(filename);
        
        if (!file.is_open())
        {
            LOG_WARN("Configuration file not found: " + filename + " (using defaults)");
            return false;
        }

        std::string line;
        int lineNumber = 0;

        while (std::getline(file, line))
        {
            lineNumber++;
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#' || line[0] == ';')
            {
                continue;
            }

            // Remove leading/trailing whitespace
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);

            // Find the '=' separator
            size_t eqPos = line.find('=');
            if (eqPos == std::string::npos)
            {
                LOG_WARN("Invalid configuration line " + std::to_string(lineNumber) + ": " + line);
                continue;
            }

            std::string key = line.substr(0, eqPos);
            std::string value = line.substr(eqPos + 1);

            // Trim key and value
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            if (!key.empty())
            {
                m_config[key] = value;
                LOG_DEBUG("Loaded config: " + key + " = " + value);
            }
        }

        file.close();
        LOG_INFO("Configuration loaded from: " + filename);
        return true;
    }

    std::string Config::getString(const std::string& key, const std::string& defaultValue) const
    {
        auto it = m_config.find(key);
        if (it != m_config.end())
        {
            return it->second;
        }
        return defaultValue;
    }

    int Config::getInt(const std::string& key, int defaultValue) const
    {
        auto it = m_config.find(key);
        if (it != m_config.end())
        {
            try
            {
                return std::stoi(it->second);
            }
            catch (...)
            {
                LOG_WARN("Invalid integer value for config key: " + key);
            }
        }
        return defaultValue;
    }

    bool Config::getBool(const std::string& key, bool defaultValue) const
    {
        auto it = m_config.find(key);
        if (it != m_config.end())
        {
            std::string value = it->second;
            std::transform(value.begin(), value.end(), value.begin(), ::tolower);
            
            if (value == "true" || value == "1" || value == "yes" || value == "on")
            {
                return true;
            }
            else if (value == "false" || value == "0" || value == "no" || value == "off")
            {
                return false;
            }
        }
        return defaultValue;
    }

    double Config::getDouble(const std::string& key, double defaultValue) const
    {
        auto it = m_config.find(key);
        if (it != m_config.end())
        {
            try
            {
                return std::stod(it->second);
            }
            catch (...)
            {
                LOG_WARN("Invalid double value for config key: " + key);
            }
        }
        return defaultValue;
    }

    void Config::setString(const std::string& key, const std::string& value)
    {
        m_config[key] = value;
    }

    void Config::setInt(const std::string& key, int value)
    {
        m_config[key] = std::to_string(value);
    }

    void Config::setBool(const std::string& key, bool value)
    {
        m_config[key] = value ? "true" : "false";
    }

    void Config::setDouble(const std::string& key, double value)
    {
        m_config[key] = std::to_string(value);
    }

    bool Config::saveToFile(const std::string& filename) const
    {
        std::string fileToWrite = filename.empty() ? m_configFile : filename;
        std::ofstream file(fileToWrite);
        
        if (!file.is_open())
        {
            LOG_ERROR("Failed to save configuration to: " + fileToWrite);
            return false;
        }

        file << "# Assembly Line Simulator Configuration\n";
        file << "# Format: key=value\n";
        file << "# Lines starting with # or ; are comments\n\n";

        for (const auto& pair : m_config)
        {
            file << pair.first << "=" << pair.second << "\n";
        }

        file.close();
        LOG_INFO("Configuration saved to: " + fileToWrite);
        return true;
    }

    bool Config::hasKey(const std::string& key) const
    {
        return m_config.find(key) != m_config.end();
    }

    void Config::clear()
    {
        m_config.clear();
    }
} // namespace seneca

