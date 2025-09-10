#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>

int main()
{
    int sockfd;
    struct sockaddr_in multicast_addr;
    struct ip_mreq multicast_req;

    // Create a UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket creation failed");
        exit(1);
    }

    // Set up multicast address
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_port = htons(12345);
    inet_pton(AF_INET, "224.1.1.1", &multicast_addr.sin_addr);

    // Set up multicast request
    multicast_req.imr_multiaddr = multicast_addr.sin_addr;
    multicast_req.imr_interface.s_addr = INADDR_ANY;

    // Join multicast group
    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multicast_req, sizeof(multicast_req)) < 0)
    {
        perror("setsockopt failed");
        exit(1);
    }

    // Send multicast message
    char message[] = "Hello, world!";
    sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&multicast_addr, sizeof(multicast_addr));

    // Close socket
    close(sockfd);
    return 0;
}