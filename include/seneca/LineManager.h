#ifndef SENECA_LINEMANAGER_H
#define SENECA_LINEMANAGER_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include "seneca/Workstation.h"
#include "seneca/Utilities.h"

namespace seneca
{
    class LineManager {
        std::vector<Workstation*> m_activeLine{};
        size_t m_cntCustomerOrder{};
        Workstation* m_firstStation{};

        public: 
            LineManager(const std::string& file, const std::vector<Workstation*>& stations);
            void reorderStations();
            bool run(std::ostream& os);
            void display(std::ostream& os) const;
    };
} // namespace seneca

#endif