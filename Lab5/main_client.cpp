#include <iostream>

#include  "TCPClient.hpp"

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cout << "Invalid command line argument detected: " << std::endl;
        std::cout << "Please check your values and press any key to end the program!" << std::endl;
        std::cin.get();
        return 1;
    }

    std::string serverAddress = argv[1];
    int port;

    if (!TCPClient::validate_port(argv[2], port))
    {
        std::cout << "Inavlid command line argument detected: " << argv[2] << std::endl;
        std::cout << "Please check your values and press any key to end the program!" << std::endl;
        std::cin.get();
        return 1;
    }

    TCPClient client(serverAddress, port);

    if (!client.connect())
    {
        std::cin.get();
        return 1;
    }

    client.run();

    return 0;
}