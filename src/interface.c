#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <pthread.h>
#include "basis.h"
#include "interface.h"
#include "logic.h"

WINDOW *menu_win;
WINDOW *map_win;

void init_interface()
{
	stdscr = initscr();
	raw();
	cbreak();
	start_color();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);
	mousemask(ALL_MOUSE_EVENTS, NULL);
	init_pair(1, COLOR_CYAN, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	menu_win = NULL;
	map_win = NULL;
	get_dimension();
}

/* alerta o usuario se a janela do terminal nao tiver altura suficiente
para o jogo */
void get_dimension()
{
	getmaxyx(stdscr, size_row, size_col);

	if ((size_row < MINIMUM_ROW) || (size_col < MINIMUN_COL))
	{
		endwin();
		printf("The terminal's dimension isn't big enough.\n");
		printf("We suggest you use a smaller font.\n");
		exit (1);
	}
}

WINDOW* create_win(int win_y, int win_x, int starty, int startx)
{
	WINDOW *local_win;

	refresh();
	local_win = newwin(win_y, win_x, starty, startx);
	box(local_win, 0 , 0);
	wrefresh(local_win);
	refresh();

	return local_win;
}

void destroy_win(WINDOW **local_win)
{
	werase(*local_win);
	wrefresh(*local_win);
	delwin(*local_win);
	*local_win = NULL;
}

void menu()
{
	int opt = 1, x, y, n = 1;

	menu_win = NULL;
	y = (size_row-MENU_ROW)/2;
	x = (size_col-MENU_COL)/2;
	menu_win = create_win(MENU_ROW, MENU_COL, y, x);

	aloc_options();
	init_options();

	while(n)
	{
		wprintw_menu(opt);
		keypad(menu_win, TRUE);
		refresh();
		pthread_mutex_lock(&l_key);

		switch (key_status)
		{
			case (STATUS_MOUSE_MOVED):
				opt = report_option(event.y-3, event.x-3, y, x);
				break;
			case (STATUS_DOWN):
				if (opt != N_OPTIONS)
					opt++;
				else
					opt = 0;
				break;
			case (STATUS_UP):
				if (opt != 0)
					opt--;
				else
					opt = N_OPTIONS;
				break;
			case (STATUS_MOUSE_CLICK):
				opt = report_option(event.y-3, event.x-3, y, x);
			case (STATUS_ENTER):
				n = click_option(opt);
				break;
		}
		pthread_mutex_unlock(&l_key);
	}
}

int click_option(int option)
{
	if (option == 1)
	{
		destroy_win(&menu_win);
		return 0;
	}
	if (option == 3)
	{
		endwin();
		exit(1);
	}

	return 1;
}

void wprintw_menu(int highlight)
{
	int i, j, row = 3, col = 3;

	for(i = 0; i < N_OPTIONS; ++i)
	{
		col = (MENU_COL-strlen(options[i][0]))/2;
		if(highlight == i + 1)
			wattron(menu_win, COLOR_PAIR(1));
		else
			wattron(menu_win, COLOR_PAIR(2));
		for(j = 0; j < OPTIONS_WIDTH; j++)
			mvwprintw(menu_win, row++, col, "%s", options[i][j]);
		row+=4;
	}
	wattroff(menu_win, COLOR_PAIR(1));
	wattroff(menu_win, COLOR_PAIR(2));
	box(menu_win, 0, 0);
	wrefresh(menu_win);
}

int createmap_win()
{
	map_win = create_win(MAP_ROW+2, size_col, (size_row-MAP_ROW-2)/2, 0);
	if (map_win == NULL)
		return -1;

	return 1;
}

/* printa o mapa na janela do terminal */
void wprintw_map()
{
	int i, j;

	for (i = 1; i < MAP_ROW + 1; i++)
		for (j = 1; (j < MAP_COL) && (j < size_col - 1); j++)
			mvwprintw(map_win, i, j, "%c", map[i-1][j+term_col-1]);
	refresh();
	wrefresh(map_win);
}
