#pragma once

#include <SFML/Network.hpp>
#include <string>

class TCPClient
{
    public:
        TCPClient(const std::string& addr, int p);
        ~TCPClient();

        static bool validate_port(const std::string& port_str, int& port);
        bool connect();
        bool send_message(const std::string& message);
        void run();
        void disconnect();
        bool is_connected() const;

    private:
        std::string serverAddress;
        unsigned short port;
        sf::TcpSocket socket;
        bool connected;
};