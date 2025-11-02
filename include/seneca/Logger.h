#ifndef SENECA_LOGGER_H
#define SENECA_LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <memory>

namespace seneca
{
    enum class LogLevel
    {
        DEBUG = 0,
        INFO = 1,
        WARN = 2,
        ERROR = 3,
        NONE = 4
    };

    class Logger
    {
    private:
        static std::unique_ptr<Logger> s_instance;
        static std::mutex s_mutex;
        
        LogLevel m_level;
        std::ofstream m_file;
        bool m_consoleOutput;
        bool m_fileOutput;
        std::string m_logFile;

        Logger();
        
        // Delete copy constructor and assignment operator
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        void log(LogLevel level, const std::string& message);
        std::string getLevelString(LogLevel level) const;
        std::string getTimestamp() const;

    public:
        ~Logger();

        static Logger& getInstance();
        
        // Configuration
        void setLogLevel(LogLevel level);
        void setLogFile(const std::string& filename);
        void enableConsoleOutput(bool enable);
        void enableFileOutput(bool enable);

        // Logging methods
        void debug(const std::string& message);
        void info(const std::string& message);
        void warn(const std::string& message);
        void error(const std::string& message);

        // Convenience macros-style methods
        template<typename... Args>
        void debug(const std::string& format, Args... args)
        {
            if (m_level <= LogLevel::DEBUG)
            {
                char buffer[1024];
                snprintf(buffer, sizeof(buffer), format.c_str(), args...);
                log(LogLevel::DEBUG, std::string(buffer));
            }
        }

        template<typename... Args>
        void info(const std::string& format, Args... args)
        {
            if (m_level <= LogLevel::INFO)
            {
                char buffer[1024];
                snprintf(buffer, sizeof(buffer), format.c_str(), args...);
                log(LogLevel::INFO, std::string(buffer));
            }
        }
    };
} // namespace seneca

// Convenience macros
#define LOG_DEBUG(msg) seneca::Logger::getInstance().debug(msg)
#define LOG_INFO(msg) seneca::Logger::getInstance().info(msg)
#define LOG_WARN(msg) seneca::Logger::getInstance().warn(msg)
#define LOG_ERROR(msg) seneca::Logger::getInstance().error(msg)

#endif // SENECA_LOGGER_H

