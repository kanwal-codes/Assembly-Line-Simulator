#ifndef SENECA_WORKSTATION_H
#define SENECA_WORKSTATION_H

#include <iostream>
#include <deque>
#include "seneca/CustomerOrder.h"
#include "seneca/Station.h"

namespace seneca
{
    extern std::deque<CustomerOrder> g_pending;
    extern std::deque<CustomerOrder> g_completed;
    extern std::deque<CustomerOrder> g_incomplete;

    class Workstation : public Station {
        std::deque<CustomerOrder> m_orders{};
        Workstation* m_pNextStation{};

        public:
            Workstation(const std::string& str);
            void fill(std::ostream& os);
            bool attemptToMoveOrder();
            void setNextStation(Workstation* station);
            Workstation* getNextStation() const;
            void display(std::ostream& os) const;
            Workstation& operator+=(CustomerOrder&& newOrder);
    };
} // namespace seneca


#endif