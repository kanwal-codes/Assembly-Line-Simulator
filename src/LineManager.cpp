#include "LineManager.h"

namespace seneca
{
    LineManager::LineManager(const std::string &file, const std::vector<Workstation *> &stations)
        : m_cntCustomerOrder(0), m_firstStation(nullptr)
    {
        std::ifstream inputFile(file);
        if (!inputFile)
        {
            throw std::runtime_error("Error: file " + file + " not found");
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
        os << "Line Manager Iteration: " << ++iterationCount << std::endl;

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

        return (g_completed.size() + g_incomplete.size() == m_cntCustomerOrder);
    }

    void LineManager::display(std::ostream &os) const
    {
        std::for_each(m_activeLine.begin(), m_activeLine.end(),
                      [&os](Workstation *ws)
                      { ws->display(os); });
    }
} // namespace seneca
