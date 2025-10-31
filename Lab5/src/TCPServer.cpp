#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "TCPServer.hpp"

TCPServer::TCPServer(int p)
    : TCPServer(p, "server.log")
{

}

TCPServer::TCPServer(int p, const std::string& lf)
    : port(p)
    , running(false)
    , logFile(lf)
{
    
}

TCPServer::~TCPServer()
{
    stop();
}

bool TCPServer::validate_port(const std::string& port_str, int& port)
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

bool TCPServer::start()
{
    if (listener.listen(port) != sf::Socket::Done)
    {
        std::cerr << "Error listening on port " << port << std::endl;
        return false; 
    }

    running = true;
    std::cout << "Server listening on port " << port << std::endl;
    log_message("Server started on port " + std::to_string(port));

    return true;
}

void TCPServer::run()
{
    while (running)
    {
        auto clientSocket = std::make_unique<sf::TcpSocket>();

        if (listener.accept(*clientSocket) == sf::Socket::Done)
        {
            // Create a thread to handle this client
            client_threads.emplace_back(
                std::make_unique<std::thread>(&TCPServer::handle_client, this, std::move(clientSocket))
            );
            client_threads.back()->detach();
        }
        else
        {
            if (running)
            {
                std::cerr << "Error accepting client connection" << std::endl;
            }
        }
    }
}

void TCPServer::stop()
{
    running = false;
    listener.close();
}

std::string TCPServer::getLogFile() const
{
    return logFile;
}

void TCPServer::log_message(const std::string& message)
{
    std::lock_guard<std::mutex> lock(log_mutex);
    std::ofstream log(logFile, std::ios::app);
    if (log.is_open())
    {
        log << message << std::endl;
        log.close();
    }
}

void TCPServer::handle_client(std::unique_ptr<sf::TcpSocket> clientSocket)
{
    // Get client address and port
    sf::IpAddress clientIp = clientSocket->getRemoteAddress();
    unsigned short clientPort = clientSocket->getRemotePort();
    std::string clientInfo = clientIp.toString() + ":" + std::to_string(clientPort);

    log_message("Client connected from " + clientInfo);
    std::cout << "Client connected from " << clientInfo << std::endl;

    char buffer[4096];
    while (running)
    {
        std::size_t received;
        sf::Socket::Status status = clientSocket->receive(buffer, sizeof(buffer) - 1, received);

        if (status == sf::Socket::Done)
        {
            buffer[received] = '\0';
            std::string message(buffer, received);
            log_message(message);
            std::cout << "Received from " << clientInfo << ": " << message << std::endl;
        }
        else if (status == sf::Socket::Disconnected)
        {
            break;
        }
        else if (status == sf::Socket::Error)
        {
            std::cerr << "Error receiving from client " << clientInfo << std::endl;
            break;
        }
    }

    log_message("Client disconnected from " + clientInfo);
    std::cout << "Client disconnected from " << clientInfo << std::endl;

    clientSocket->disconnect();
}