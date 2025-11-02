#include "seneca/Workstation.h"

namespace seneca
{
    std::deque<CustomerOrder> g_pending{};
    std::deque<CustomerOrder> g_completed{};
    std::deque<CustomerOrder> g_incomplete{};

    Workstation::Workstation(const std::string& str) : Station(str){}

    void Workstation::fill(std::ostream& os) {
        if(!m_orders.empty()) {
            m_orders.front().fillItem(*this,os);
        }
    }

    // bool Workstation::attemptToMoveOrder() {
    //     if(!m_orders.empty()) {
    //         return false;
    //     }

    //     CustomerOrder& order = m_orders.front();

    //     if(!order.isItemFilled(getItemName())) {
    //         if(m_pNextStation) {
    //             *m_pNextStation += std::move(order);
    //         }
    //         else {
    //             if (order.isOrderFilled())
    //             {
    //                 g_completed.push_back(std::move(order));
    //             }
    //             else
    //             {
    //                 g_incomplete.push_back(std::move(order));
    //             }
    //         }
    //     }

    //     m_orders.pop_front();
    //     return true;

    // }

    bool Workstation::attemptToMoveOrder()
    {
        if (m_orders.empty())
        {
            return false;
        }

        CustomerOrder &order = m_orders.front();

        if (order.isItemFilled(getItemName()) || getQuantity() == 0)
        {
            if (m_pNextStation)
            {
                *m_pNextStation += std::move(order);
            }
            else
            {
                if (order.isOrderFilled())
                {
                    g_completed.push_back(std::move(order));
                }
                else
                {
                    g_incomplete.push_back(std::move(order));
                }
            }
            m_orders.pop_front();
            return true;
        }

        return false;
    }

    void Workstation::setNextStation(Workstation *station)
    {
        m_pNextStation = station;
    }

    Workstation *Workstation::getNextStation() const
    {
        return m_pNextStation;
    }

    void Workstation::display(std::ostream &os) const
    {
        os << getItemName() << " --> ";
        if (m_pNextStation)
        {
            os << m_pNextStation->getItemName();
        }
        else
        {
            os << "End of Line";
        }
        os << "\n";
    }

    Workstation &Workstation::operator+=(CustomerOrder &&newOrder)
    {
        m_orders.push_back(std::move(newOrder));
        return *this;
    }

    
} // namespace seneca


