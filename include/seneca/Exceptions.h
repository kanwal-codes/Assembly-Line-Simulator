#ifndef SENECA_EXCEPTIONS_H
#define SENECA_EXCEPTIONS_H

#include <stdexcept>
#include <string>

namespace seneca
{
    // Base exception class for Assembly Line Simulator
    class AssemblyLineException : public std::runtime_error
    {
    public:
        explicit AssemblyLineException(const std::string& message)
            : std::runtime_error("Assembly Line Error: " + message) {}
    };

    // Configuration exceptions
    class ConfigException : public AssemblyLineException
    {
    public:
        explicit ConfigException(const std::string& message)
            : AssemblyLineException("Configuration Error: " + message) {}
    };

    // File I/O exceptions
    class FileException : public AssemblyLineException
    {
    public:
        explicit FileException(const std::string& message)
            : AssemblyLineException("File Error: " + message) {}
    };

    // Validation exceptions
    class ValidationException : public AssemblyLineException
    {
    public:
        explicit ValidationException(const std::string& message)
            : AssemblyLineException("Validation Error: " + message) {}
    };

    // Station exceptions
    class StationException : public AssemblyLineException
    {
    public:
        explicit StationException(const std::string& message)
            : AssemblyLineException("Station Error: " + message) {}
    };

    // Order exceptions
    class OrderException : public AssemblyLineException
    {
    public:
        explicit OrderException(const std::string& message)
            : AssemblyLineException("Order Error: " + message) {}
    };
} // namespace seneca

#endif // SENECA_EXCEPTIONS_H

