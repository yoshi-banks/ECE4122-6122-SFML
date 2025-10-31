#pragma once

#include <SFML/Network.hpp>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <memory>

class TCPServer
{
    public:
        TCPServer(int p);
        TCPServer(int p, const std::string& lf);
        ~TCPServer();

        static bool validate_port(const std::string& port_str, int& port);
        bool start();
        void run();
        void stop();
        std::string getLogFile() const;

    private:
        const std::string logFile;
        unsigned short port;
        sf::TcpListener listener;
        std::vector<std::unique_ptr<std::thread>> client_threads;
        std::mutex log_mutex;
        bool running;

        void log_message(const std::string& message);
        void handle_client(std::unique_ptr<sf::TcpSocket> clientSocket);
};