#include <ncurses.h>

WINDOW *chatwb, *chatw;
WINDOW *textwb, *textw;

int max_rows, max_cols;
int chat_rows, chat_cols;
int text_rows, text_cols;

void init_ui() {
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
	wprintw(chatw, "\n%s", msg);
	wrefresh(chatw);
}

void scan_msg(char *msg) {
	mvwgetstr(textw, 0, 0, msg);
	werase(textw);
	wrefresh(textw);
}
