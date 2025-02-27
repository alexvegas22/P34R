#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 8080
#define MAX_PEERS 3
#define MAX_BUFFER 1024

int sockfd[MAX_PEERS];
struct sockaddr_in server_addr[MAX_PEERS];
pthread_t threads[MAX_PEERS];

void* receive_messages(void* arg) {
    int peer_index = *((int*)arg);
    char buffer[MAX_BUFFER];
    while (1) {
        int len = recv(sockfd[peer_index], buffer, MAX_BUFFER, 0);
        if (len <= 0) {
            perror("Receive failed or connection closed");
            break;
        }
        buffer[len] = '\0';
        printf("Peer %d received: %s\n> ", peer_index + 1, buffer);
    }
    return NULL;
}

void* send_messages(void* arg) {
    int peer_index = *((int*)arg);
    char message[MAX_BUFFER];
    while (1) {
        printf("> ");
        fgets(message, MAX_BUFFER, stdin);
        message[strcspn(message, "\n")] = 0;  // Remove the newline

        if (send(sockfd[peer_index], message, strlen(message), 0) == -1) {
            perror("Send failed");
            break;
        }
    }
    return NULL;
}

void* peer_listener(void* arg) {
    int peer_index = *((int*)arg);
    int listener_sockfd, new_sockfd;
    struct sockaddr_in peer_addr;
    socklen_t addr_len = sizeof(peer_addr);

    listener_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listener_sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port = htons(PORT + peer_index);  // Different port for each peer
    peer_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listener_sockfd, (struct sockaddr*)&peer_addr, sizeof(peer_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(listener_sockfd, 5) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Peer %d listening on port %d...\n", peer_index + 1, PORT + peer_index);
    
    // Accept incoming connection
    new_sockfd = accept(listener_sockfd, (struct sockaddr*)&peer_addr, &addr_len);
    if (new_sockfd < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    // Store the new connection for communication
    sockfd[peer_index] = new_sockfd;
    close(listener_sockfd); // Close the listening socket after accepting the connection

    return NULL;
}

int main() {
    int peer_index;
    char peer_ip[16];

    // Initialize sockets for each peer
    for (peer_index = 0; peer_index < MAX_PEERS; peer_index++) {
        sockfd[peer_index] = -1; // Initialize the socket array
    }

    // Create listener threads for each peer
    for (peer_index = 0; peer_index < MAX_PEERS; peer_index++) {
        int *index = malloc(sizeof(int));
        *index = peer_index;
        if (pthread_create(&threads[peer_index], NULL, peer_listener, (void*)index) != 0) {
            perror("Failed to create listener thread");
            exit(EXIT_FAILURE);
        }
    }

    // After listener threads are created, connect to other peers
    for (peer_index = 0; peer_index < MAX_PEERS; peer_index++) {
        if (peer_index == 0) {
            // Peer 1 connects to Peer 2 and Peer 3
            for (int i = 1; i < MAX_PEERS; i++) {
                sockfd[peer_index] = socket(AF_INET, SOCK_STREAM, 0);
                if (sockfd[peer_index] < 0) {
                    perror("Socket creation failed");
                    exit(EXIT_FAILURE);
                }

                server_addr[i].sin_family = AF_INET;
                server_addr[i].sin_port = htons(PORT + i);
                if (inet_pton(AF_INET, "127.0.0.1", &server_addr[i].sin_addr) <= 0) {
                    perror("Invalid address or address not supported");
                    exit(EXIT_FAILURE);
                }

                if (connect(sockfd[peer_index], (struct sockaddr *)&server_addr[i], sizeof(server_addr[i])) < 0) {
                    perror("Connection failed");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    // Create send and receive threads for each peer
    for (peer_index = 0; peer_index < MAX_PEERS; peer_index++) {
        int *index = malloc(sizeof(int));
        *index = peer_index;
        if (pthread_create(&threads[peer_index], NULL, send_messages, (void*)index) != 0) {
            perror("Failed to create send thread");
            exit(EXIT_FAILURE);
        }

        if (pthread_create(&threads[peer_index], NULL, receive_messages, (void*)index) != 0) {
            perror("Failed to create receive thread");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for threads to finish (they'll run indefinitely)
    for (peer_index = 0; peer_index < MAX_PEERS; peer_index++) {
        pthread_join(threads[peer_index], NULL);
    }

    // Close all sockets when done
    for (peer_index = 0; peer_index < MAX_PEERS; peer_index++) {
        close(sockfd[peer_index]);
    }

    return 0;
}
