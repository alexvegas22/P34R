#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3434
#define SERVER "127.0.0.1"

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char message[1024];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Set up the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER);

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    // Get user input and send it to the server
    while (1) {
        printf("Enter message: ");
        fgets(message, sizeof(message), stdin);

        // Send message to the server
        if (send(sock, message, strlen(message), 0) < 0) {
            perror("Send failed");
            return 1;
        }

        // Exit if the user types "exit"
        if (strncmp(message, "exit", 4) == 0) {
            break;
        }
    }

    close(sock);
    return 0;
}
