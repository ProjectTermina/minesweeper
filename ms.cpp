extern "C" {
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
}
#include "game.hpp"
#include <cmath>
#include <cctype>
#include <iostream>

#define USAGE_STRING "Usage:\n\t./ms [args]\n[-d density]:	floating point between 0.0 and 1.0\n[-w width]:	width of grid\n[-h height]:	height of grid\n"

static WINDOW *header, *game, *separator, *rules;

void initialize_windows(Game g) {
	initscr();
	int c = can_change_color() == TRUE;
	if (has_colors() == TRUE) {
		start_color();
		init_pair(1, COLOR_CYAN, 0);
		init_pair(2, COLOR_GREEN, 0);
		init_pair(3, COLOR_RED, 0);
		init_pair(4, COLOR_BLUE, 0);
		init_pair(5, COLOR_YELLOW, 0);
		init_pair(6, COLOR_MAGENTA, 0);
		init_pair(7, COLOR_BLACK, 0);
		init_pair(8, COLOR_WHITE, 0);
	}

	size_t width = g.get_width() * 2 ;
	size_t height = g.get_height();

	size_t header_height = 2;
	size_t horizontal_divider = std::floor(COLS/2.0);
	size_t board_margin = (horizontal_divider - width) / 2;
	header	= newwin(header_height, horizontal_divider, 0, 0);
	game	= newwin(height, width, header_height + 1, board_margin);
	separator = newwin(LINES, 2, 0, horizontal_divider + 1);
	rules	= newwin(LINES, horizontal_divider - 2, 0, horizontal_divider + 3);
	keypad(game, TRUE);
	
	mvwprintw(header, 0, 0, "ProjectTermina's Minesweeper");
	mvwprintw(header, 1, 0, "~*~*~*~*~*~*~*~*~*~*~*~*~*~*");
	mvwprintw(rules, 0, 0, "\n`, ', f, F: flag a cell\n\n\
SPACEBAR, c, C: clear a cell\n\n\
arrow keys, hjkl, wasd: move\n\n\
q: quit");
	for (int i = 0; i < LINES; i++) {
		waddch(separator, ACS_VLINE);
		waddch(separator, ' ');
	}
	wrefresh(header);
	wrefresh(rules);
	wrefresh(separator);

	noecho();	// user input does not display on screen
	cbreak();	// buffering does not wait for newline
}

void print_usage_and_exit() {
	endwin();
	std::cerr << USAGE_STRING;
	exit(1);
}

void print_board(const Game &g) {
	wmove(game, 0, 0);
	std::stringstream s = g.status();
	//mvwaddstr(game, 0, 0, s.data());
	for (char c : s.str()) {
		if (std::isdigit(c)) {
			attr_t old_attr;
			short old_short;
			wattr_get(game, &old_attr, &old_short, NULL);
			wattr_set(game, old_attr, c - '0', NULL);
			waddch(game, c);
			wattr_set(game, old_attr, old_short, NULL);
		} else {
			waddch(game, c);
		}
		waddch(game, ' ');
	}
}

extern char *optarg;
extern int optind, opterr, optopt;

int main(int argc, char **argv) {

	size_t width = 0;
	size_t height = 0;
	double density = 0.0;
	const char *optstring = "w:h:d:";
	bool some_values_set = false;
	bool density_set = false;

	while (int i = getopt(argc, argv, optstring)) {
		if (i == 'w') {
			width = atoi(optarg);
		} else if (i == 'h') {
			height = atoi(optarg);
		} else if (i == 'd') {
			density_set = true;
			density = atof(optarg);
		} else if (i == -1) {
			break;
		} else {
			print_usage_and_exit();
		}
		some_values_set = true;
	}

	if (some_values_set && (width == 0 || height == 0 || !density_set)) {
		print_usage_and_exit();
	}

	Game game_state;
	if (some_values_set) game_state = Game(width, height, density);

	initialize_windows(game_state);
	int i = 0;
	while (game_state.is_running()) {
		print_board(game_state);
		wmove(game, game_state.get_y(), 2*game_state.get_x());
		int ch = wgetch(game);
		switch (ch) {
			case KEY_UP:
			case 'k':
			case 'K':
			case 'w':
			case 'W':
				game_state.cursor_up(); break;
			case KEY_DOWN:
			case 'j':
			case 'J':
			case 's':
			case 'S':
				game_state.cursor_down(); break;
			case KEY_LEFT:
			case 'h':
			case 'H':
			case 'a':
			case 'A':
				game_state.cursor_left(); break;
			case KEY_RIGHT:
			case 'l':
			case 'L':
			case 'd':
			case 'D':
				game_state.cursor_right(); break;
			case '`':
			case '\'':
			case 'f':
			case 'F':
				game_state.cursor_flag(); break;
			case ' ':
			case 'c':
			case 'C':
				game_state.cursor_clear(); break;
			case 'q':
			case 'Q':
				game_state.quit();
				break;
		}

	}
	endwin();
}
