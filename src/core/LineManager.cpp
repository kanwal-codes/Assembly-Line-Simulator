#include "seneca/LineManager.h"
#include "seneca/Logger.h"
#include "seneca/Exceptions.h"

namespace seneca
{
    LineManager::LineManager(const std::string &file, const std::vector<Workstation *> &stations)
        : m_cntCustomerOrder(0), m_firstStation(nullptr)
    {
        LOG_INFO("Initializing LineManager with file: " + file);
        std::ifstream inputFile(file);
        if (!inputFile)
        {
            LOG_ERROR("Assembly line configuration file not found: " + file);
            throw FileException("Assembly line configuration file not found: " + file);
        }

        std::string record;
        std::vector<std::pair<std::string, std::string>> stationLinks;

        while (std::getline(inputFile, record))
        {
            Utilities util;
            size_t next_pos = 0;
            bool more = true;

            std::string stationName = util.extractToken(record, next_pos, more);
            std::string nextStationName = more ? util.extractToken(record, next_pos, more) : "";

            stationLinks.emplace_back(stationName, nextStationName);
        }

        std::vector<Workstation *> activeStations;

        for (const auto &link : stationLinks)
        {
            const std::string& stationName = link.first;
            const std::string& nextStationName = link.second;
            
            auto current = std::find_if(stations.begin(), stations.end(),
                                        [stationName](Workstation *ws)
                                        { return ws->getItemName() == stationName; });

            auto next = std::find_if(stations.begin(), stations.end(),
                                     [nextStationName](Workstation *ws)
                                     { return ws->getItemName() == nextStationName; });

            if (current != stations.end())
            {
                (*current)->setNextStation(next != stations.end() ? *next : nullptr);
                activeStations.push_back(*current);
            }
        }

        auto it = std::find_if(stations.begin(), stations.end(),
                               [&stationLinks](Workstation *ws)
                               {
                                   return std::none_of(stationLinks.begin(), stationLinks.end(),
                                                       [&ws](const std::pair<std::string, std::string> &link)
                                                       {
                                                           return ws->getItemName() == link.second;
                                                       });
                               });

        if (it != stations.end())
        {
            m_firstStation = *it;
        }

        m_activeLine = activeStations;
        m_cntCustomerOrder = g_pending.size();
        
        LOG_INFO("LineManager initialized with " + std::to_string(m_activeLine.size()) + " stations");
        LOG_INFO("Pending orders: " + std::to_string(m_cntCustomerOrder));
        if (m_firstStation)
        {
            LOG_INFO("First station: " + m_firstStation->getItemName());
        }
    }

    void LineManager::reorderStations()
    {
        std::vector<Workstation *> reorderedLine;
        Workstation *currentStation = m_firstStation;

        while (currentStation)
        {
            reorderedLine.push_back(currentStation);
            currentStation = currentStation->getNextStation();
        }

        m_activeLine = reorderedLine;
    }

    bool LineManager::run(std::ostream &os)
    {
        static size_t iterationCount = 0;
        iterationCount++;
        LOG_DEBUG("Running iteration " + std::to_string(iterationCount));
        os << "Line Manager Iteration: " << iterationCount << std::endl;

        if (!g_pending.empty())
        {
            *m_firstStation += std::move(g_pending.front());
            g_pending.pop_front();
        }

        std::for_each(m_activeLine.begin(), m_activeLine.end(),
                      [&os](Workstation *ws)
                      { ws->fill(os); });

        std::for_each(m_activeLine.begin(), m_activeLine.end(),
                      [](Workstation *ws)
                      { ws->attemptToMoveOrder(); });

        bool allProcessed = (g_completed.size() + g_incomplete.size() == m_cntCustomerOrder);
        if (allProcessed)
        {
            LOG_INFO("All orders processed. Completed: " + std::to_string(g_completed.size()) + 
                     ", Incomplete: " + std::to_string(g_incomplete.size()));
        }
        
        return allProcessed;
    }

    void LineManager::display(std::ostream &os) const
    {
        std::for_each(m_activeLine.begin(), m_activeLine.end(),
                      [&os](Workstation *ws)
                      { ws->display(os); });
    }
} // namespace seneca
