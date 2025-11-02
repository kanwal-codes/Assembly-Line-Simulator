#include "seneca/Logger.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>

namespace seneca
{
    std::unique_ptr<Logger> Logger::s_instance = nullptr;
    std::mutex Logger::s_mutex;

    Logger::Logger()
        : m_level(LogLevel::INFO)
        , m_consoleOutput(true)
        , m_fileOutput(false)
        , m_logFile("assembly_line.log")
    {
    }

    Logger::~Logger()
    {
        if (m_file.is_open())
        {
            m_file.close();
        }
    }

    Logger& Logger::getInstance()
    {
        std::lock_guard<std::mutex> lock(s_mutex);
        if (s_instance == nullptr)
        {
            s_instance = std::unique_ptr<Logger>(new Logger());
        }
        return *s_instance;
    }

    void Logger::setLogLevel(LogLevel level)
    {
        m_level = level;
    }

    void Logger::setLogFile(const std::string& filename)
    {
        m_logFile = filename;
        if (m_fileOutput)
        {
            if (m_file.is_open())
            {
                m_file.close();
            }
            m_file.open(m_logFile, std::ios::app);
        }
    }

    void Logger::enableConsoleOutput(bool enable)
    {
        m_consoleOutput = enable;
    }

    void Logger::enableFileOutput(bool enable)
    {
        m_fileOutput = enable;
        if (enable)
        {
            if (m_file.is_open())
            {
                m_file.close();
            }
            m_file.open(m_logFile, std::ios::app);
        }
        else
        {
            if (m_file.is_open())
            {
                m_file.close();
            }
        }
    }

    void Logger::log(LogLevel level, const std::string& message)
    {
        if (level < m_level)
        {
            return;
        }

        std::string timestamp = getTimestamp();
        std::string levelStr = getLevelString(level);
        std::string logMessage = "[" + timestamp + "] [" + levelStr + "] " + message;

        std::lock_guard<std::mutex> lock(s_mutex);

        if (m_consoleOutput)
        {
            if (level == LogLevel::ERROR)
            {
                std::cerr << logMessage << std::endl;
            }
            else
            {
                std::cout << logMessage << std::endl;
            }
        }

        if (m_fileOutput && m_file.is_open())
        {
            m_file << logMessage << std::endl;
            m_file.flush();
        }
    }

    std::string Logger::getLevelString(LogLevel level) const
    {
        switch (level)
        {
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO:  return "INFO ";
            case LogLevel::WARN:  return "WARN ";
            case LogLevel::ERROR: return "ERROR";
            default: return "UNKNOWN";
        }
    }

    std::string Logger::getTimestamp() const
    {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }

    void Logger::debug(const std::string& message)
    {
        log(LogLevel::DEBUG, message);
    }

    void Logger::info(const std::string& message)
    {
        log(LogLevel::INFO, message);
    }

    void Logger::warn(const std::string& message)
    {
        log(LogLevel::WARN, message);
    }

    void Logger::error(const std::string& message)
    {
        log(LogLevel::ERROR, message);
    }
} // namespace seneca

