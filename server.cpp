#include <iostream>
#include <thread>
#include <vector>
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

void handleClient(int clientSocket) {
    char buffer[1024];
    while (true) {
        std::memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            std::cout << "Client disconnected." << std::endl;
#ifdef _WIN32
            closesocket(clientSocket);
#else
            close(clientSocket);
#endif
            return;
        }

        std::cout << "Received: " << buffer << std::endl;
        std::string response = "Echo: " + std::string(buffer);
        send(clientSocket, response.c_str(), response.size(), 0);
    }
}

int main() {
    initializeSockets();

    int serverSocket;
#ifdef _WIN32
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
#else
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
#endif
    if (serverSocket < 0) {
        std::cerr << "Failed to create socket." << std::endl;
        cleanupSockets();
        return EXIT_FAILURE;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Bind failed." << std::endl;
#ifdef _WIN32
        closesocket(serverSocket);
#else
        close(serverSocket);
#endif
        cleanupSockets();
        return EXIT_FAILURE;
    }

    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Listen failed." << std::endl;
#ifdef _WIN32
        closesocket(serverSocket);
#else
        close(serverSocket);
#endif
        cleanupSockets();
        return EXIT_FAILURE;
    }

    std::cout << "Server listening on port 8080..." << std::endl;

    std::vector<std::thread> clientThreads;

    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket < 0) {
            std::cerr << "Failed to accept client connection." << std::endl;
            continue;
        }

        std::cout << "Client connected: " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl;

        clientThreads.emplace_back([clientSocket]() { handleClient(clientSocket); });
        clientThreads.back().detach();
    }


#ifdef _WIN32
    closesocket(serverSocket);
#else
    close(serverSocket);
#endif
    cleanupSockets();

    return 0;
}
