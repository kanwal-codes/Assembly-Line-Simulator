#include "seneca/CustomerOrder.h"

namespace seneca
{
    size_t CustomerOrder::m_widthField{0};

    CustomerOrder::CustomerOrder(const std::string& str) {
        Utilities ut;
        size_t next_pos = 0;
        bool more = true;

        m_name = ut.extractToken(str,next_pos,more);

        m_product = ut.extractToken(str,next_pos,more);

        std::vector<std::string> m_items;
        while(more) {
            m_items.push_back(ut.extractToken(str,next_pos,more));
        }

        m_cntItem = m_items.size();
        m_lstItem = new Item* [m_cntItem];

        for(size_t i = 0; i < m_items.size();i++) {
            m_lstItem[i] = new Item(m_items[i]);
        }

        m_widthField = (m_widthField > ut.getFieldWidth()) ? m_widthField : ut.getFieldWidth();
    }

    CustomerOrder::CustomerOrder(CustomerOrder&& customer) noexcept {
        *this = std::move(customer);
    }

    CustomerOrder& CustomerOrder::operator=(CustomerOrder&& customer) noexcept{
        if(this != &customer) {
            for(size_t i = 0;i < m_cntItem;i++) {
                delete m_lstItem[i];
            }

            delete[] m_lstItem;

            m_name = std::move(customer.m_name);
            m_product = std::move(customer.m_product);
            m_cntItem = customer.m_cntItem;
            m_lstItem = std::move(customer.m_lstItem);

            customer.m_lstItem = nullptr;
            customer.m_cntItem = 0;
        }
        return *this;
    }

    bool CustomerOrder::isOrderFilled() const {
        for(size_t i = 0;i < m_cntItem;i++) {
            if(!m_lstItem[i]->m_isFilled) {
                return false;
            }
        }
        return true;
    }

    bool CustomerOrder::isItemFilled(const std::string& itemName) const {
        for(size_t i = 0; i < m_cntItem; i++) {
            if(m_lstItem[i]->m_itemName == itemName && !m_lstItem[i]->m_isFilled) {
                return false;
            }
        }

        return true;
    }

    void CustomerOrder::fillItem(Station &station, std::ostream &os)
    {
        for (size_t i = 0; i < m_cntItem; i++)
        {
            if (m_lstItem[i]->m_itemName == station.getItemName() && !m_lstItem[i]->m_isFilled)
            {
                if (station.getQuantity() > 0)
                {
                    station.updateQuantity();
                    m_lstItem[i]->m_serialNumber = station.getNextSerialNumber();
                    m_lstItem[i]->m_isFilled = true;
                    os << "    Filled " << m_name << ", " << m_product << " [" << m_lstItem[i]->m_itemName << "]\n";
                    return;
                }
                else
                {
                    os << "    Unable to fill " << m_name << ", " << m_product << " [" << m_lstItem[i]->m_itemName << "]\n";
                }
            }
        }
    }

    void CustomerOrder::display(std::ostream &os) const
    {
        os << m_name << " - " << m_product << "\n";
        for (size_t i = 0; i < m_cntItem; ++i)
        {
            os << "[" << std::right << std::setw(6) << std::setfill('0') << m_lstItem[i]->m_serialNumber << "] "
               << std::setw(m_widthField) << std::setfill(' ') << std::left << m_lstItem[i]->m_itemName << " - "
               << (m_lstItem[i]->m_isFilled ? "FILLED" : "TO BE FILLED") << "\n";
        }
    }

    CustomerOrder::~CustomerOrder()
    {
        for (size_t i = 0; i < m_cntItem; i++)
        {
            delete m_lstItem[i];
        }
        delete[] m_lstItem;
    }

    CustomerOrder::CustomerOrder(const CustomerOrder& customer) {
        (void)customer;  // Suppress unused parameter warning
        throw "This is an error!";
    }

    size_t CustomerOrder::getFilledItemCount() const {
        size_t filled = 0;
        for (size_t i = 0; i < m_cntItem; i++) {
            if (m_lstItem[i]->m_isFilled) {
                filled++;
            }
        }
        return filled;
    }
} // namespace seneca
