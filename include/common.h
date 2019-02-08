#ifndef _COMMON
#define _COMMON

#define PORT_NUMBER 1729
#define MSG_SIZE 1000
#define NAME_SIZE 15

void send_msg(int sockfd);
void recv_msg(int sockfd, char *name);

#endif
