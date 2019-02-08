#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <assert.h>
#include "../include/common.h"

int conn_server(char *server_name, int name_size) {
	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		#ifdef DEBUG
		fprintf(stderr, "Error in socket creation\n");
		#endif
		exit(EXIT_FAILURE);
	}
	#ifdef DEBUG
	fprintf(stderr, "Socket created\n");
	#endif

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
		#ifdef DEBUG
		fprintf(stderr, "Error in connection establishment\n");
		#endif
		exit(EXIT_FAILURE);
	}
	#ifdef DEBUG
	fprintf(stderr, "Connection established\n");
	#endif

	if (getnameinfo((struct sockaddr *)&server_addr, sizeof(struct sockaddr_in), \
					server_name, name_size, NULL, 0, 0) != 0) {
		#ifdef DEBUG
		fprintf(stderr, "Error in getting server name\n");
		#endif
		exit(EXIT_FAILURE);
	}
	#ifdef DEBUG
	fprintf(stderr, "Got server name\n");
	#endif

	printf("Connected to server %s (%s)\n", server_name, inet_ntoa(server_addr.sin_addr));
	return sockfd;
}

int main() {
	char server_name[NAME_SIZE];
	int sockfd = conn_server(server_name, NAME_SIZE);

	start_chat(sockfd, server_name);

	close(sockfd);
	return 0;
}
