#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT_NUMBER 12345
#define MAX_PENDING 5
#define BUFFER_SIZE 1000

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

int conn_client(int server_sockfd) {
	struct sockaddr_in client_addr;
	socklen_t addr_length = sizeof(struct sockaddr_in);
	int client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &addr_length);
	if (client_sockfd == -1) {
		fprintf(stderr, "Error in client socket creation\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "Client socket created\n");

	printf("Connected to client %s\n", inet_ntoa(client_addr.sin_addr));
	return client_sockfd;
}

void send_msg(int sockfd) {
	char buffer[BUFFER_SIZE];
	int length;

	printf("> @: ");
	scanf(" %[^\n]", buffer);
	if (strcmp(buffer, "/exit") == 0) {
		fprintf(stderr, "Exiting\n");
		exit(EXIT_SUCCESS);
	}
	length = strlen(buffer);
	if (send(sockfd, buffer, length, 0) != length) {
		fprintf(stderr, "Error while sending\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "Sent message\n");
}

void recv_msg(int sockfd) {
	char buffer[BUFFER_SIZE];
	int length;

	if ((length = recv(sockfd, buffer, BUFFER_SIZE - 1, 0)) == -1) {
		fprintf(stderr, "Error while receiving\n");
		exit(EXIT_FAILURE);
	}
	buffer[length] = '\0';
	fprintf(stderr, "Received message\n");
	printf("> &: %s\n", buffer);
}

int main() {
	int server_sockfd = start_listen();
	int client_sockfd = conn_client(server_sockfd);

	printf("Ready to message\n");
	printf("Type \"/exit\" (without quotes) to exit\n");
	while (1) {
		recv_msg(client_sockfd);
		send_msg(client_sockfd);
	}

	close(client_sockfd);
	close(server_sockfd);
	return 0;
}
