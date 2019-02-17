/*
chatter - A simple chat program written in C
Copyright (c) 2019 Anuvind Bhat
*/

#include <ncurses.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/common.h"
#include "../include/ui.h"

#define BUFFER_SIZE (MSG_SIZE + NAME_SIZE + 20)

pthread_mutex_t chat_mutex;
pthread_mutex_t text_mutex;

WINDOW *chatwb, *chatw;
WINDOW *textwb, *textw;

int max_rows, max_cols;
int chat_rows, chat_cols;
int text_rows, text_cols;

void draw_ui();

void init_ui() {
	if (pthread_mutex_init(&chat_mutex, NULL) != 0) {
		#ifdef DEBUG
		fprintf(stderr, "Error in chat mutex creation\n");
		#endif
		exit(EXIT_FAILURE);
	}
	#ifdef DEBUG
	fprintf(stderr, "Chat mutex created\n");
	#endif

	if (pthread_mutex_init(&text_mutex, NULL) != 0) {
		#ifdef DEBUG
		fprintf(stderr, "Error in text mutex creation\n");
		#endif
		exit(EXIT_FAILURE);
	}
	#ifdef DEBUG
	fprintf(stderr, "Text mutex created\n");
	#endif

	initscr();
	draw_ui();
}

void draw_ui() {
	nocbreak();
	getmaxyx(stdscr, max_rows, max_cols);
	text_rows = max_rows / 8;
	text_cols = max_cols - 2;
	chat_rows = max_rows - text_rows - 4;
	chat_cols = max_cols - 2;
	chatwb = derwin(stdscr, chat_rows + 2, chat_cols + 2, 0, 0);
	textwb = derwin(stdscr, text_rows + 2, text_cols + 2, chat_rows + 2, 0);
	box(chatwb, 0, 0);
	box(textwb, 0, 0);
	wrefresh(chatwb);
	wrefresh(textwb);
	chatw = derwin(chatwb, chat_rows, chat_cols, 1, 1);
	textw = derwin(textwb, text_rows, text_cols, 1, 1);
	scrollok(chatw, TRUE);
	scrollok(textw, TRUE);
	wmove(chatw, chat_rows - 1, 0);
}

void redraw_ui() {
	pthread_mutex_lock(&text_mutex);
	pthread_mutex_lock(&chat_mutex);
	delwin(chatw);
	delwin(textw);
	delwin(chatwb);
	delwin(textwb);
	endwin();
	refresh();
	clear();
	refresh();
	draw_ui();
	pthread_mutex_unlock(&chat_mutex);
	pthread_mutex_unlock(&text_mutex);
}

void destroy_ui() {
	pthread_mutex_lock(&text_mutex);
	pthread_mutex_lock(&chat_mutex);
	endwin();
	pthread_mutex_unlock(&chat_mutex);
	pthread_mutex_unlock(&text_mutex);
}

void print_msg(char *msg, char *name, int flags) {
	char buffer[BUFFER_SIZE];
	time_t rawtime;
	time(&rawtime);
	struct tm timestamp;
	localtime_r(&rawtime, &timestamp);
	if ((flags & DISPLAY_TIME) && (flags & DISPLAY_NAME)) {
		sprintf(buffer, "|%02d:%02d:%02d| [%s] %s", \
				timestamp.tm_hour, timestamp.tm_min, timestamp.tm_sec, name, msg);
	}
	else if (flags & DISPLAY_TIME) {
		sprintf(buffer, "|%02d:%02d:%02d| %s", \
				timestamp.tm_hour, timestamp.tm_min, timestamp.tm_sec, msg);
	}
	else if (flags & DISPLAY_NAME) {
		sprintf(buffer, "[%s] %s", name, msg);
	}
	pthread_mutex_lock(&chat_mutex);
	wprintw(chatw, "\n%s", buffer);
	wrefresh(chatw);
	pthread_mutex_unlock(&chat_mutex);
}

void scan_msg(char *msg) {
	pthread_mutex_lock(&text_mutex);
	mvwgetstr(textw, 0, 0, msg);
	werase(textw);
	wrefresh(textw);
	pthread_mutex_unlock(&text_mutex);
}
