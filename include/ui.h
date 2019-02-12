/*
chatter - A simple chat program written in C
Copyright (c) 2019 Anuvind Bhat
*/

#ifndef _UI
#define _UI

#define DISPLAY_TIME 0x01
#define DISPLAY_NAME 0x02

void init_ui();
void destroy_ui();
void print_msg(char *msg, char *name, int flags);
void scan_msg(char *msg);

#endif
