#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include "../include/common.h"

#define MAX_PENDING 3

int start_listen() {
	int server_sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (server_sockfd == -1) {
		#ifdef DEBUG
		fprintf(stderr, "Error in server socket creation\n");
		#endif
		exit(EXIT_FAILURE);
	}
	#ifdef DEBUG
	fprintf(stderr, "Server socket created\n");
	#endif

	int option = 1;
	if (setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int)) != 0) {
		#ifdef DEBUG
		fprintf(stderr, "Error in setting SO_REUSEADDR\n");
		#endif
		exit(EXIT_FAILURE);
	}
	#ifdef DEBUG
	fprintf(stderr, "Set SO_REUSEADDR\n");
	#endif

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUMBER);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) != 0) {
		#ifdef DEBUG
		fprintf(stderr, "Error in binding\n");
		#endif
		exit(EXIT_FAILURE);
	}
	#ifdef DEBUG
	fprintf(stderr, "Binding successful\n");
	#endif

	if (listen(server_sockfd, MAX_PENDING) != 0) {
		#ifdef DEBUG
		fprintf(stderr, "Error in listening\n");
		#endif
		exit(EXIT_FAILURE);
	}
	#ifdef DEBUG
	fprintf(stderr, "Listening\n");
	#endif

	return server_sockfd;
}

int conn_client(int server_sockfd, char *client_name, int name_size) {
	struct sockaddr_in client_addr;
	socklen_t addr_length = sizeof(struct sockaddr_in);
	printf("Waiting for client to connect\n");
	int client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &addr_length);
	if (client_sockfd == -1) {
		#ifdef DEBUG
		fprintf(stderr, "Error in client socket creation\n");
		#endif
		exit(EXIT_FAILURE);
	}
	#ifdef DEBUG
	fprintf(stderr, "Client socket created\n");
	#endif

	if (getnameinfo((struct sockaddr *)&client_addr, sizeof(struct sockaddr_in), \
					client_name, name_size, NULL, 0, 0) != 0) {
		#ifdef DEBUG
		fprintf(stderr, "Error in getting client name\n");
		#endif
		exit(EXIT_FAILURE);
	}
	#ifdef DEBUG
	fprintf(stderr, "Got client name\n");
	#endif

	printf("Connected to client %s (%s)\n", client_name, inet_ntoa(client_addr.sin_addr));
	return client_sockfd;
}

int main() {
	int server_sockfd = start_listen();
	char client_name[NAME_SIZE];
	int client_sockfd = conn_client(server_sockfd, client_name, NAME_SIZE);

	start_chat(client_sockfd, client_name);

	close(client_sockfd);
	close(server_sockfd);
	return 0;
}