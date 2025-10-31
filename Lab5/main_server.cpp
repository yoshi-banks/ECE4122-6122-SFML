#include <iostream>

#include "TCPServer.hpp"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Inavlid command line argument detected: " << std::endl;
        std::cout << "Please check your valus and press any key to end the program!" << std::endl;
        std::cin.get();
        return 1;
    }

    int port;
    if (!TCPServer::validate_port(argv[1], port))
    {
        std::cout << "Invalid command line argument detected: " << argv[1] << std::endl;
        std::cout << "Please check your values and press any key to end the program!" << std:: endl;
        std::cin.get();
        return 1;
    }

    TCPServer server(port);

    if (!server.start())
    {
        std::cerr << "Failed to start server" << std::endl;
        return 1;
    }

    server.run();

    return 0;
}