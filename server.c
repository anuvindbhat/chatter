#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <assert.h>
#include "common.h"

#define MAX_PENDING 3

int start_listen() {
	int server_sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (server_sockfd == -1) {
		fprintf(stderr, "Error in server socket creation\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "Server socket created\n");

	int option = 1;
	if (setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int)) != 0) {
		fprintf(stderr, "Error in setting SO_REUSEADDR\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "Set SO_REUSEADDR\n");

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUMBER);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) != 0) {
		fprintf(stderr, "Error in binding\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "Binding successful\n");

	if (listen(server_sockfd, MAX_PENDING) != 0) {
		fprintf(stderr, "Error in listening\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "Listening\n");

	return server_sockfd;
}

int conn_client(int server_sockfd, char *client_name, int name_size) {
	struct sockaddr_in client_addr;
	socklen_t addr_length = sizeof(struct sockaddr_in);
	int client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &addr_length);
	if (client_sockfd == -1) {
		fprintf(stderr, "Error in client socket creation\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "Client socket created\n");

	if (getnameinfo((struct sockaddr *)&client_addr, sizeof(struct sockaddr_in), \
					client_name, name_size, NULL, 0, 0) != 0) {
		fprintf(stderr, "Error in getting client name\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "Got client name\n");

	printf("Connected to client %s (%s)\n", client_name, inet_ntoa(client_addr.sin_addr));
	return client_sockfd;
}

void send_msg(int sockfd) {
	char msg[MSG_SIZE];
	int size;

	printf("> me: ");
	assert(scanf(" %[^\n]", msg) == 1);
	if (strcmp(msg, "/exit") == 0) {
		fprintf(stderr, "Exiting\n");
		exit(EXIT_SUCCESS);
	}
	size = strlen(msg);
	if (send(sockfd, msg, size, 0) != size) {
		fprintf(stderr, "Error while sending\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "Sent message\n");
}

void recv_msg(int sockfd, char *name) {
	char msg[MSG_SIZE];
	int size;

	if ((size = recv(sockfd, msg, MSG_SIZE - 1, 0)) == -1) {
		fprintf(stderr, "Error while receiving\n");
		exit(EXIT_FAILURE);
	}
	msg[size] = '\0';
	fprintf(stderr, "Received message\n");
	printf("> %s: %s\n", name, msg);
}

int main() {
	int server_sockfd = start_listen();
	char client_name[NAME_SIZE];
	int client_sockfd = conn_client(server_sockfd, client_name, NAME_SIZE);

	printf("Ready to message\n");
	printf("Type \"/exit\" (without quotes) to exit\n");
	while (1) {
		recv_msg(client_sockfd, client_name);
		send_msg(client_sockfd);
	}

	close(client_sockfd);
	close(server_sockfd);
	return 0;
}
