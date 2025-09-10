#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() 
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = { 0 };
    const char* message = "Hello from client";

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IP address to binary format
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) 
    {
        perror("inet_pton failed");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
    {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Connected to server" << std::endl;

    // Send message to server
    send(sock, message, strlen(message), 0);
    std::cout << "Sent message: " << message << std::endl;

    // Receive response from server
    read(sock, buffer, BUFFER_SIZE);
    std::cout << "Received response: " << buffer << std::endl;

    // Close socket
    close(sock);

    return 0;
}