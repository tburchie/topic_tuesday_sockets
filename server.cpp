#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

// For Windows
#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to handle Windows initialization
#ifdef _WIN32
void init_win_socket() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed!" << std::endl;
        exit(1);
    }
}
#endif

int main() {
#ifdef _WIN32
    init_win_socket(); // Initialize Windows socket library
#endif

    // Create server socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP

    // Bind the socket to the address and port
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        close(server_fd);
        return 1;
    }

    std::cout << "Server listening on port " << PORT << "..." << std::endl;

    // Accept an incoming connection
    int client_socket;
    sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
    if (client_socket < 0) {
        perror("Accept failed");
        close(server_fd);
        return 1;
    }

    std::cout << "Connection established with client." << std::endl;

    // Receive a message from the client
    char buffer[BUFFER_SIZE] = {0};
    int read_size = read(client_socket, buffer, BUFFER_SIZE);
    if (read_size < 0) {
        perror("Read failed");
        close(client_socket);
        close(server_fd);
        return 1;
    }

    std::cout << "Message from client: " << buffer << std::endl;

    // Optionally, send a response to the client
    std::string response = "Hello from server!";
    send(client_socket, response.c_str(), response.length(), 0);

    // Close the sockets
    close(client_socket);
    close(server_fd);

#ifdef _WIN32
    WSACleanup(); // Clean up Windows socket library
#endif

    return 0;
}
