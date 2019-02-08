#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <assert.h>
#include <string.h>
#include "../include/common.h"

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
