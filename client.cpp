#include <iostream>
#include <string>
#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif

void initializeSockets() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        exit(EXIT_FAILURE);
    }
#endif
}

void cleanupSockets() {
#ifdef _WIN32
    WSACleanup();
#endif
}

int main() {
    initializeSockets();

    int clientSocket;
#ifdef _WIN32
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
#else
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
#endif
    if (clientSocket < 0) {
        std::cerr << "Failed to create socket." << std::endl;
        cleanupSockets();
        return EXIT_FAILURE;
    }

    std::string serverIP;
    std::cout << "Enter server IP address: ";
    std::cin >> serverIP;

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    if (inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address or address not supported." << std::endl;
#ifdef _WIN32
        closesocket(clientSocket);
#else
        close(clientSocket);
#endif
        cleanupSockets();
        return EXIT_FAILURE;
    }

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection to the server failed." << std::endl;
#ifdef _WIN32
        closesocket(clientSocket);
#else
        close(clientSocket);
#endif
        cleanupSockets();
        return EXIT_FAILURE;
    }

    std::cout << "Connected to the server." << std::endl;

    std::cin.ignore(); // Clear the input buffer after std::cin.
    std::string message;
    char buffer[1024];
    while (true) {
        std::cout << "Enter message to send (or type 'exit' to quit): ";
        std::getline(std::cin, message);

        if (message == "exit") {
            break;
        }

        // Send the message, including the null terminator.
        if (send(clientSocket, message.c_str(), message.size(), 0) < 0) {
            std::cerr << "Failed to send message." << std::endl;
            break;
        }

        // Clear and receive server response.
        std::memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            std::cerr << "Connection closed by the server." << std::endl;
            break;
        }

        std::cout << "Server response: " << buffer << std::endl;
    }

#ifdef _WIN32
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif
    cleanupSockets();

    return 0;
}
