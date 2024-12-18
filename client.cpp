#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

// For Windows, include Windows specific headers
#ifdef _WIN32
#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#endif

// Function to detect OS
std::string get_os_name() {
#ifdef _WIN32
    return "Windows";
#elif __linux__
    return "Linux";
#elif __APPLE__
    return "macOS";
#else
    return "Unknown OS";
#endif
}

// Function to get local IP address for Linux/macOS
std::string get_local_ip() {
    struct sockaddr_in sa;
    char buffer[128];
    
    // Get the local IP address using a socket (for Linux/macOS)
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        return "";
    }
    
    sa.sin_family = AF_INET;
    sa.sin_port = htons(8080);  // Port doesn't matter, just need a valid IP
    sa.sin_addr.s_addr = inet_addr("8.8.8.8");  // Google DNS (arbitrary external IP)
    
    // Try to connect to the remote address to resolve the local IP
    if (connect(sockfd, (struct sockaddr*)&sa, sizeof(sa)) == -1) {
        perror("Failed to connect");
        close(sockfd);
        return "";
    }
    
    // Retrieve the local IP address used for the connection
    socklen_t len = sizeof(sa);
    if (getsockname(sockfd, (struct sockaddr*)&sa, &len) == -1) {
        perror("Failed to get socket name");
        close(sockfd);
        return "";
    }
    
    // Convert the IP to string and return it
    inet_ntop(AF_INET, &(sa.sin_addr), buffer, sizeof(buffer));
    close(sockfd);
    
    return std::string(buffer);
}

// Function to get local IP address for Windows
std::string get_local_ip_windows() {
    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = nullptr;
    DWORD dwSize = sizeof(IP_ADAPTER_INFO);
    char local_ip[16] = {0};
    
    pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
    if (GetAdaptersInfo(pAdapterInfo, &dwSize) == NO_ERROR) {
        pAdapter = pAdapterInfo;
        while (pAdapter) {
            if (pAdapter->IpAddressList.IpAddress.String[0] != '0') {
                strcpy(local_ip, pAdapter->IpAddressList.IpAddress.String);
                break;
            }
            pAdapter = pAdapter->Next;
        }
    }
    
    free(pAdapterInfo);
    return std::string(local_ip);
}

int main() {
    // Get OS and local IP
    std::string os_name = get_os_name();
    std::cout << "OS detected: " << os_name << std::endl;

    std::string local_ip;
#ifdef _WIN32
    local_ip = get_local_ip_windows();
#else
    local_ip = get_local_ip();
#endif

    if (local_ip.empty()) {
        std::cerr << "Error: Could not determine local IP address." << std::endl;
        return 1;
    }

    std::cout << "Local IP address: " << local_ip << std::endl;

    // Set server address and port
    std::string server_ip = "192.168.1.10"; // Replace with server's local IP
    int port = 8080;
    
    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Socket creation failed");
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());

    // Connect to the server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(sock);
        return 1;
    }

    // Send message to the server
    std::string message = "Hello from client!";
    if (send(sock, message.c_str(), message.length(), 0) == -1) {
        perror("Message send failed");
        close(sock);
        return 1;
    }
    
    std::cout << "Message sent to server: " << message << std::endl;

    // Close socket
    close(sock);

    return 0;
}
