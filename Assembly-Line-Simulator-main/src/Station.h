#ifndef SENECA_STATION_H
#define SENECA_STATION_H
#include <iostream>
#include <string>
#include <iomanip>
#include "Utilities.h"

namespace seneca
{
    class Station  {
        
    int m_id{};
    std::string m_name{};
    std::string m_description{};
    size_t m_serialNumber{};
    size_t m_itemQuantity{};

    static size_t id_generator;
    static size_t m_widthField;

    public : 
        Station(const std::string& name);
        Station(Station &&) noexcept = default;
        Station &operator=(Station &&) noexcept = default;
        Station(const Station &) = default;
        Station &operator=(const Station &) = default;
        const std::string& getItemName() const;
        size_t getNextSerialNumber();
        size_t getQuantity() const;
        void updateQuantity();
        void display(std::ostream& os, bool full) const;
    };
} // namespace seneca

#endif