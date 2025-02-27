// C program to demonstrate peer to peer chat using Socket Programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

//Sending messages to port
void sending(char name, int PORT)
{

  char buffer[2000] = {0};
  //Fetching port number
  int PORT_server;

  //IN PEER WE TRUST
  printf("Enter the port to send message:");
  scanf("%d", &PORT_server);

  int sock = 0, valread;
  struct sockaddr_in serv_addr;
  char hello[1024] = {0};
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      printf("\n Socket creation error \n");
      return;
    }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(PORT_server);

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
      printf("\nConnection Failed \n");
      return;
    }

  char dummy;
  printf("Enter your message:");
  scanf("%c", &dummy); //The buffer is our enemy
  scanf("%[^\n]s", hello);
  sprintf(buffer, "%s[PORT:%d] says: %s", name, PORT, hello);
  send(sock, buffer, sizeof(buffer), 0);
  printf("\nMessage sent\n");
  close(sock);
}


//Receiving messages on our port
void receiving(int server_fd)
{
  struct sockaddr_in address;
  int valread;
  char buffer[2000] = {0};
  int addrlen = sizeof(address);
  fd_set current_sockets, ready_sockets;

  //Initialize my current set
  FD_ZERO(&current_sockets);
  FD_SET(server_fd, &current_sockets);
  int k = 0;
  while (1)
    {
      k++;
      ready_sockets = current_sockets;

      if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0)
        {
	  perror("Error");
	  exit(EXIT_FAILURE);
        }

      for (int i = 0; i < FD_SETSIZE; i++)
        {
	  if (FD_ISSET(i, &ready_sockets))
            {

	      if (i == server_fd)
                {
		  int client_socket;

		  if ((client_socket = accept(server_fd, (struct sockaddr *)&address,
					      (socklen_t *)&addrlen)) < 0)
                    {
		      perror("accept");
		      exit(EXIT_FAILURE);
                    }
		  FD_SET(client_socket, &current_sockets);
                }
	      else
                {
		  valread = recv(i, buffer, sizeof(buffer), 0);
		  printf("\n%s\n", buffer);
		  FD_CLR(i, &current_sockets);
                }
            }
        }

      if (k == (FD_SETSIZE * 2))
	break;
    }
}
