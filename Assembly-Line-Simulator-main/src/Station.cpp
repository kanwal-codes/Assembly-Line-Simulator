#include "Station.h"

namespace seneca
{
    size_t Station::id_generator = 0;
    size_t Station::m_widthField = 0;

    Station::Station(const std::string &name)
    {
        //std::cout << name << std::endl;
        Utilities ut;
        size_t next_pos = 0;
        bool more = false;

        m_id = ++id_generator;
        try
        {
            m_name = ut.extractToken(name, next_pos, more);
            //std::cout << "Item name: " << m_name << std::endl;

            if(more) m_serialNumber = std::stoul(ut.extractToken(name, next_pos, more));

            if(more) m_itemQuantity = std::stoul(ut.extractToken(name, next_pos, more));

            if (m_widthField < ut.getFieldWidth())
            {
                m_widthField = ut.getFieldWidth();
            }

            if(more) m_description = ut.extractToken(name, next_pos, more);
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("Error constructing Station: " + name + " | " + e.what());
        }

        // std::cout << "m_name: " << m_name << std::endl;
        // std::cout << "m_serialNumber: " << m_serialNumber << std::endl;
        // std::cout << "m_itemQuantity: " << m_itemQuantity << std::endl;
        // std::cout << "m_widthField: " << m_widthField << std::endl;
        // std::cout << "m_description: " << m_description << std::endl;
    }

    const std::string &Station::getItemName() const
    {
        return m_name;
    }

    size_t Station::getNextSerialNumber()
    {
        return m_serialNumber++;
    }

    size_t Station::getQuantity() const
    {
        return m_itemQuantity;
    }

    void Station::updateQuantity()
    {
        if (m_itemQuantity > 0)
        {
            m_itemQuantity--;
        }
    }

    void Station::display(std::ostream &os, bool full) const
    {
        // std::cout << m_id << std::endl;
        // std::cout << m_name << std::endl;
        // std::cout << m_serialNumber << std::endl;
        // std::cout << m_itemQuantity << std::endl;
        
        os << std::right << std::setw(3) << std::setfill('0') << m_id << " | "
           << std::setw(m_widthField) << std::setfill(' ') << std::left << m_name << " | "
           << std::setw(6) << std::setfill('0') << std::right << m_serialNumber << " | ";

        if (full)
        {
            os << std::setw(4) << std::setfill(' ') << m_itemQuantity << " | "
               << m_description;
        }
        os << std::endl;
    }
} // namespace seneca
