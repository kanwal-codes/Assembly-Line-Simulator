#ifndef SENECA_APISERVER_H
#define SENECA_APISERVER_H

#include <string>
#include <memory>
#include <mutex>
#include <functional>
#include <map>

namespace seneca
{
    class Database;

    class APIServer
    {
    private:
        static std::unique_ptr<APIServer> s_instance;
        static std::mutex s_mutex;

        int m_port;
        bool m_running;
        void* m_server; // Opaque pointer to avoid including httplib.h in header

        APIServer();
        APIServer(const APIServer&) = delete;
        APIServer& operator=(const APIServer&) = delete;

    public:
        ~APIServer();
        
        static APIServer& getInstance();

        // Server control
        bool start(int port = 8080);
        void stop();
        bool isRunning() const { return m_running; }
        int getPort() const { return m_port; }

        // Status
        std::string getStatus() const;
    };
} // namespace seneca

#endif // SENECA_APISERVER_H

