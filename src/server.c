#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3434
char name[20];

int main(int argc, char const *argv[]) {

  printf("Enter name:");
  scanf("%s", name);
  
  int server_sock, client_sock;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_len = sizeof(client_addr);
  char buffer[1024];
  int read_size;

  server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock < 0) {
    perror("Socket creation failed");
    return 1;
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  printf("IP address is: %s\n", inet_ntoa(server_addr.sin_addr));
  printf("port is: %d\n", (int)ntohs(server_addr.sin_port));

  if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("Bind failed");
    return 1;
  }

  if (listen(server_sock, 3) < 0) {
    perror("Listen failed");
    return 1;
  }

  printf("Waiting for incoming connections...\n");

  client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
  if (client_sock < 0) {
    perror("Accept failed");
    return 1;
  }

  printf("Connection accepted\n");

  // Receive messages from the client
  while ((read_size = recv(client_sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
    buffer[read_size] = '\0'; // Null-terminate the string
    printf("Received: %s", buffer);
        
    // Exit if the message is "exit"
    if (strncmp(buffer, "exit", 4) == 0) {
      break;
    }
  }

  if (read_size == 0) {
    printf("Client disconnected\n");
  } else if (read_size == -1) {
    perror("Recv failed");
  }

  close(client_sock);
  close(server_sock);
  return 0;
}
