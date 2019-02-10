#include <ncurses.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/common.h"

pthread_mutex_t ui_mutex;

WINDOW *chatwb, *chatw;
WINDOW *textwb, *textw;

int max_rows, max_cols;
int chat_rows, chat_cols;
int text_rows, text_cols;

void init_ui() {
	if (pthread_mutex_init(&ui_mutex, NULL) != 0) {
		#ifdef DEBUG
		fprintf(stderr, "Error in UI mutex creation\n");
		#endif
		exit(EXIT_FAILURE);
	}
	#ifdef DEBUG
	fprintf(stderr, "UI mutex created\n");
	#endif

	initscr();
	getmaxyx(stdscr, max_rows, max_cols);
	text_rows = max_rows / 8;
	text_cols = max_cols - 2;
	chat_rows = max_rows - text_rows - 4;
	chat_cols = max_cols - 2;
	chatwb = newwin(chat_rows + 2, chat_cols + 2, 0, 0);
	textwb = newwin(text_rows + 2, text_cols + 2, chat_rows + 2, 0);
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

void destroy_ui() {
	endwin();
}

void print_msg(char *msg) {
	pthread_mutex_lock(&ui_mutex);
	wprintw(chatw, "\n%s", msg);
	wrefresh(chatw);
	pthread_mutex_unlock(&ui_mutex);
}

void scan_msg(char *msg) {
	mvwgetstr(textw, 0, 0, msg);
	werase(textw);
	wrefresh(textw);
}
