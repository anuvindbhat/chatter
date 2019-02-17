/*
chatter - A simple chat program written in C
Copyright (c) 2019 Anuvind Bhat
*/

#ifndef _COMMON
#define _COMMON

// #define DEBUG

#define PORT_NUMBER 1729
// Includes '\0' character
#define MSG_SIZE 1000
#define NAME_SIZE 16

void start_chat(int sockfd, char *name);

#endif
