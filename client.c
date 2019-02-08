#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <assert.h>
#include "common.h"

int conn_server(char *server_name, int name_size) {
	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		fprintf(stderr, "Error in socket creation\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "Socket created\n");

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUMBER);

	while (1) {
		char ip_addr[16];
		printf("Enter the server IP address (. separated)\n");
		assert(scanf(" %s", ip_addr) == 1);
		if (inet_aton(ip_addr, &server_addr.sin_addr) != 0) {
			break;
		}
		printf("Invalid IP address\n");
	}

	if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) != 0) {
		fprintf(stderr, "Error in connection establishment\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "Connection established\n");

	if (getnameinfo((struct sockaddr *)&server_addr, sizeof(struct sockaddr_in), \
					server_name, name_size, NULL, 0, 0) != 0) {
		fprintf(stderr, "Error in getting server name\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "Got server name\n");

	printf("Connected to server %s (%s)\n", server_name, inet_ntoa(server_addr.sin_addr));
	return sockfd;
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
	char server_name[NAME_SIZE];
	int sockfd = conn_server(server_name, NAME_SIZE);

	printf("Ready to message\n");
	printf("Type \"/exit\" (without quotes) to exit\n");
	while (1) {
		send_msg(sockfd);
		recv_msg(sockfd, server_name);
	}

	close(sockfd);
	return 0;
}
