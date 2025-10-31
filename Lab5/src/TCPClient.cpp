#include <iostream>

#include "TCPClient.hpp"

TCPClient::TCPClient(const std::string& addr, int p)
    : serverAddress(addr)
    , port(p)
    , connected(false) 
{

}

TCPClient::~TCPClient()
{
    disconnect();
}

bool TCPClient::validate_port(const std::string& port_str, int& port)
{
    try
    {
        port = std::stoi(port_str);
        if (port < 61000 || port > 65535)
        {
            return false;
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool TCPClient::connect()
{
    sf::IpAddress ipAddress(serverAddress);

    if (ipAddress == sf::IpAddress::None)
    {
        std::cout << "Failed to connect to the server at " << serverAddress
                  << " on " << port << "." << std::endl;
        std::cout << "Please check your values and press any key to end program!" << std::endl;
        return false;
    }

    sf::Socket::Status status = socket.connect(ipAddress, port, sf::seconds(5));

    if (status != sf::Socket::Done)
    {
        std::cout << "Failed to connect to the server at " << serverAddress
                  << " on " << port << "." << std::endl;
        std::cout << "Please check your values and press any key to end program!" << std::endl;
        return false;
    }

    connected = true;
    std::cout << "Connected to server at " << serverAddress << ":" << port << std::endl;
    return true;
}

bool TCPClient::send_message(const std::string& message)
{
    if (!connected)
    {
        std::cerr << "Not connected to server" << std::endl;
        return false;
    }

    sf::Socket::Status status = socket.send(message.c_str(), message.length());

    if (status != sf::Socket::Done)
    {
        std::cerr << "Error sending message to server" << std::endl;
        return false;
    }
    return true;
}

void TCPClient::run()
{
    std::cout << "Type messages to send to the server (Ctrl+C to exit)" << std::endl;

    std::string message;
    while (true)
    {
        std::cout << "Please enter a message: ";
        std::getline(std::cin, message);

        if (std::cin.eof())
        {
            break;
        }

        if (!send_message(message))
        {
            break;
        }
    }
}

void TCPClient::disconnect()
{
    if (connected)
    {
        socket.disconnect();
        connected = false;
        std::cout << "Disconnected from server" << std::endl;
    }
}

bool TCPClient::is_connected() const
{
    return connected;
}