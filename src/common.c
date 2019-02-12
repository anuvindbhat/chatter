/*
chatter - A simple chat program written in C
Copyright (c) 2019 Anuvind Bhat
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <pthread.h>
#include "../include/common.h"
#include "../include/ui.h"

struct recv_params {
	int sockfd;
	char *name;
};

#ifdef DEBUG
pthread_mutex_t stderr_mutex;
#endif

void * send_msg(void *arg) {
	int sockfd = *((int *)arg);

	while(1) {
		char msg[MSG_SIZE];
		int size;

		do {
			scan_msg(msg);
		} while(strlen(msg) == 0);
		if (strcmp(msg, "/exit") == 0) {
			#ifdef DEBUG
			pthread_mutex_lock(&stderr_mutex);
			fprintf(stderr, "Exiting\n");
			pthread_mutex_unlock(&stderr_mutex);
			#endif
			break;
		}
		char buffer[BUFFER_SIZE];
		sprintf(buffer, "[me] %s", msg);
		print_msg(buffer);

		size = strlen(msg);
		if (send(sockfd, msg, size, 0) != size) {
			#ifdef DEBUG
			pthread_mutex_lock(&stderr_mutex);
			fprintf(stderr, "Error while sending\n");
			pthread_mutex_unlock(&stderr_mutex);
			#endif
			destroy_ui();
			exit(EXIT_FAILURE);
		}
		#ifdef DEBUG
		pthread_mutex_lock(&stderr_mutex);
		fprintf(stderr, "Sent message\n");
		pthread_mutex_unlock(&stderr_mutex);
		#endif
	}

	pthread_exit(NULL);
}

void * recv_msg(void *arg) {
	struct recv_params rparams = *((struct recv_params *)arg);
	int sockfd = rparams.sockfd;
	char *name = rparams.name;

	while(1) {
		char msg[MSG_SIZE];

		int size = recv(sockfd, msg, MSG_SIZE - 1, 0);
		if (size == -1) {
			#ifdef DEBUG
			pthread_mutex_lock(&stderr_mutex);
			fprintf(stderr, "Error while receiving\n");
			pthread_mutex_unlock(&stderr_mutex);
			#endif
			destroy_ui();
			exit(EXIT_FAILURE);
		}
		else if (size == 0) {
			char buffer[BUFFER_SIZE];
			sprintf(buffer, "%s has left the chat", name);
			print_msg(buffer);
			break;
		}
		msg[size] = '\0';
		#ifdef DEBUG
		pthread_mutex_lock(&stderr_mutex);
		fprintf(stderr, "Received message\n");
		pthread_mutex_unlock(&stderr_mutex);
		#endif

		char buffer[BUFFER_SIZE];
		sprintf(buffer, "[%s] %s", name, msg);
		print_msg(buffer);
	}

	pthread_exit(NULL);
}

void start_chat(int sockfd, char *name) {
	print_msg("Ready to chat");
	print_msg("Type \"/exit\" (without quotes) to exit");

	#ifdef DEBUG
	if (pthread_mutex_init(&stderr_mutex, NULL) != 0) {
		fprintf(stderr, "Error in stderr mutex creation\n");
		destroy_ui();
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "stderr mutex created\n");
	#endif

	pthread_t send_thread, recv_thread;

	if (pthread_create(&send_thread, NULL, send_msg, (void *)&sockfd) != 0) {
		#ifdef DEBUG
		pthread_mutex_lock(&stderr_mutex);
		fprintf(stderr, "Error in send thread creation\n");
		pthread_mutex_unlock(&stderr_mutex);
		#endif
		destroy_ui();
		exit(EXIT_FAILURE);
	}
	#ifdef DEBUG
	pthread_mutex_lock(&stderr_mutex);
	fprintf(stderr, "Send thread created\n");
	pthread_mutex_unlock(&stderr_mutex);
	#endif

	struct recv_params rparams;
	rparams.sockfd = sockfd;
	rparams.name = name;
	if (pthread_create(&recv_thread, NULL, recv_msg, (void *)&rparams) != 0) {
		#ifdef DEBUG
		pthread_mutex_lock(&stderr_mutex);
		fprintf(stderr, "Error in receive thread creation\n");
		pthread_mutex_unlock(&stderr_mutex);
		#endif
		destroy_ui();
		exit(EXIT_FAILURE);
	}
	#ifdef DEBUG
	pthread_mutex_lock(&stderr_mutex);
	fprintf(stderr, "Receive thread created\n");
	pthread_mutex_unlock(&stderr_mutex);
	#endif

	if (pthread_join(send_thread, NULL) != 0) {
		#ifdef DEBUG
		pthread_mutex_lock(&stderr_mutex);
		fprintf(stderr, "Error in send thread join\n");
		pthread_mutex_unlock(&stderr_mutex);
		#endif
	}
	else {
		#ifdef DEBUG
		pthread_mutex_lock(&stderr_mutex);
		fprintf(stderr, "Send thread joined\n");
		pthread_mutex_unlock(&stderr_mutex);
		#endif
	}

	pthread_cancel(recv_thread);

	if (pthread_join(recv_thread, NULL) != 0) {
		#ifdef DEBUG
		pthread_mutex_lock(&stderr_mutex);
		fprintf(stderr, "Error in receive thread join\n");
		pthread_mutex_unlock(&stderr_mutex);
		#endif
	}
	else {
		#ifdef DEBUG
		pthread_mutex_lock(&stderr_mutex);
		fprintf(stderr, "Receive thread joined\n");
		pthread_mutex_unlock(&stderr_mutex);
		#endif
	}
}
