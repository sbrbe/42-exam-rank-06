#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h> //enlever <netdb.h>

static const int MAX_MSG_SIZE = 1000000; //define
static const int MAX_CLIENTS  = 1024; //define

typedef struct s_client
{
	int id;
	char msg[1000000];
} t_client;

t_client clients[MAX_CLIENTS];

int current_id = 0, maxfd = 0;

fd_set read_set, write_set, current;

char send_buffer[1000000], recv_buffer[1000000];

void putstr(int fd, char *str){ write(fd, str, strlen(str)); }

void err(char *msg){ putstr(2, msg ? msg : "Fatal error\n"); exit(1); }

void send_broadcast(int accepted, int sockfd)
{
	for (int fd = 0; fd <= maxfd; fd++)
	{
		if (fd == sockfd || fd == accepted) // pas sockfd ici 
			continue;
		if (FD_ISSET(fd, &write_set) && send(fd, send_buffer, strlen(send_buffer), 0) == -1)
			err(NULL);
	}
}

int main(int ac, char **av)
{
	if (ac != 2) err("Wrong number of arguments\n");

	int sockfd = socket(AF_INET, SOCK_STREAM, 0), connfd;
	struct sockaddr_in servaddr;

	if (sockfd == -1) err(NULL);

	memset(&servaddr, 0, sizeof(servaddr)); //	bzero(&servaddr, sizeof(servaddr)); 

	FD_ZERO(&current);
	FD_SET(sockfd, &current);
	maxfd = sockfd;

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433); // 127.0.0.1
	servaddr.sin_port = htons(atoi(av[1]));

	if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) err(NULL);
	if (listen(sockfd, 10) != 0) err(NULL);

	while (1)
	{
		read_set = current;
		write_set = current;

		FD_CLR(sockfd, &write_set); // pas la sur l autre

		if (select(maxfd + 1, &read_set, &write_set, NULL, NULL) == -1) err(NULL);

		for (int fd = 0; fd <= maxfd; fd++)
		{
			if (!FD_ISSET(fd, &read_set)) continue;

			if (fd == sockfd)
			{
				struct sockaddr_in cli;
				memset(&cli, 0, sizeof(cli)); //bzero(&cli, sizeof(cli));
				socklen_t len = sizeof(cli);
				connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
				if (connfd < 0) err(NULL);

				if (connfd > maxfd) maxfd = connfd;

				clients[connfd].id = current_id++;
				memset(clients[connfd].msg, 0, MAX_MSG_SIZE); // rajouter
				FD_SET(connfd, &current);

				sprintf(send_buffer, "server: client %d just arrived\n", clients[connfd].id);
				send_broadcast(connfd, sockfd);
			}
			else
			{
				int ret = recv(fd, recv_buffer, MAX_MSG_SIZE, 0);
				if (ret <= 0)
				{
					sprintf(send_buffer, "server: client %d just left\n", clients[fd].id);
					send_broadcast(fd, sockfd);
					FD_CLR(fd, &current);
					close(fd);
					memset(clients[fd].msg, 0, MAX_MSG_SIZE); // 	rajouter
				}
				else
				{
					int j = (int)strlen(clients[fd].msg);
					for (int i = 0; i < ret; i++, j++)
					{
						clients[fd].msg[j] = recv_buffer[i];

						if (clients[fd].msg[j] == '\n')
						{
							clients[fd].msg[j] = '\0';
							sprintf(send_buffer, "client %d: %s\n", clients[fd].id, clients[fd].msg);
							send_broadcast(fd, sockfd);
							memset(clients[fd].msg, 0, MAX_MSG_SIZE); // bzero(clients[fd].msg, strlen(clients[fd].msg));
							j = -1;
						}
					}
				}
			}
		}
	}
}
