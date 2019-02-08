#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

#define PORT_NUMBER 12345
#define MSG_SIZE 1000

int conn_server() {
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
		scanf(" %s", ip_addr);
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

	return sockfd;
}

void send_msg(int sockfd) {
	char msg[MSG_SIZE];
	int length;

	printf("> @: ");
	scanf(" %[^\n]", msg);
	if (strcmp(msg, "/exit") == 0) {
		fprintf(stderr, "Exiting\n");
		exit(EXIT_SUCCESS);
	}
	length = strlen(msg);
	if (send(sockfd, msg, length, 0) != length) {
		fprintf(stderr, "Error while sending\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "Sent message\n");
}

void recv_msg(int sockfd) {
	char msg[MSG_SIZE];
	int length;

	if ((length = recv(sockfd, msg, MSG_SIZE - 1, 0)) == -1) {
		fprintf(stderr, "Error while receiving\n");
		exit(EXIT_FAILURE);
	}
	msg[length] = '\0';
	fprintf(stderr, "Received message\n");
	printf("> &: %s\n", msg);
}

int main() {
	int sockfd = conn_server();

	printf("Ready to message\n");
	printf("Type \"/exit\" (without quotes) to exit\n");
	while (1) {
		send_msg(sockfd);
		recv_msg(sockfd);
	}

	close(sockfd);
	return 0;
}
