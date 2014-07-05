#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <panel.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "basis.h"
#include "interface.h"
#include "logic.h"

WINDOW *menu_win = NULL;
WINDOW *map_win = NULL;
PANEL *map_panel = NULL;
WINDOW *msg_win = NULL;
PANEL *msg_panel = NULL;
int size[2];

void get_dimension();
WINDOW* create_win(int win_y, int win_x, int starty, int startx);
void destroy_win(WINDOW **local_win);
int click_option(int option);
void wprintw_menu(int highlight);
void printw_scroll();
void print_map();
void print_gamebar();
void print_msghobbit();
void print_msgpaused();
void print_msgquit();
void print_msgsaved();
void print_msgload();

void init_interface()
{
	int lim_map, n;
	scrll map_scroll;

	initscr();
	raw();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);
	get_dimension();

	if (!mousemask(ALL_MOUSE_EVENTS, NULL))
	{
		endwin();
		printf("Terminal doesn't support mouse event reporting\n");
		exit(1);
	}

	start_color();
	init_pair(1, COLOR_CYAN, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);

	lim_map = sqrt(pow(MAP_COL - size[1] + 2, 2));
	set_maplim(lim_map);
	map_scroll.row = ((size[0] - MAP_ROW - 2) / 2) + MAP_ROW + 2;
	map_scroll.row = map_scroll.row + ((size[0] - map_scroll.row) / 2);
	map_scroll.col = size[1] - 2;
	map_scroll.position = 1;
	n = MAP_COL / map_scroll.col;
	map_scroll.proportion = (MAP_COL / (map_scroll.col + n)) + 1;
	map_scroll.residue = 0;
	set_mapscroll(map_scroll);
}

/* alerta o usuario se a janela do terminal nao tiver altura suficiente
para o jogo */
void get_dimension()
{
	getmaxyx(stdscr, size[0], size[1]);

	if ((size[0] < MINIMUM_ROW) || (size[1] < MINIMUN_COL))
	{
		endwin();
		printf("The terminal's dimension isn't big enough.\n");
		printf("We suggest you use a smaller font.\n");
		exit (1);
	}
	else if (size[1] > 240)
	{
		endwin();
		printf("The terminal width is too big.\n");
		printf("We suggest you resize your terminal.\n");
		exit (1);
	}
}

int get_sizerow()
{
	return size[0];
}

int get_sizecol()
{
	return size[1];
}

WINDOW* get_mapwin()
{
	return map_win;
}

WINDOW* create_win(int win_y, int win_x, int starty, int startx)
{
	WINDOW *local_win;

	refresh();
	local_win = newwin(win_y, win_x, starty, startx);
	box(local_win, 0, 0);
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
	int opt = 1, x, y, n = 1, key_status = 0;
	MEVENT event;
	menu_win = NULL;
	y = (size[0]-MENU_ROW)/2;
	x = (size[1]-MENU_COL)/2;
	menu_win = create_win(MENU_ROW, MENU_COL, y, x);

	aloc_options();
	init_options();

	while(n)
	{
		wprintw_menu(opt);
		keypad(menu_win, TRUE);
		refresh();
		pthread_mutex_lock(&l_key);
		event = get_event();
		wmouse_trafo(menu_win, &event.y, &event.x, false);
		key_status = get_keystatus();

		switch (key_status)
		{
			case (STATUS_MOUSE_MOVED):
				opt = report_option(event.y-1, event.x-1);
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
				opt = report_option(event.y-1, event.x-1);
			case (STATUS_ENTER):
				n = click_option(opt);
				break;
		}
		pthread_mutex_unlock(&l_key);
	}
}

int click_option(int option)
{
	int i;
	fortress frodo_house;
	struct stat st;

	if ((option == 1) || (option == 2))
	{
		if ((option == 2) && (stat("saves/save1", &st) == -1))
		{
			move((size[0]-MENU_ROW)/2-1, (size[1]-MENU_COL)/2);
			printw("Nothing is saved");
			refresh();
			return 1;
		}
		free_options();
		destroy_win(&menu_win);
		set_gamestatus(STATUS_GAME);
		init_interface();
		keypad(stdscr, TRUE);
		create_listbuild();
		prepare_map();
		createmap_win();
		createmsg_win();
		load_houseoption(0);
		load_houseoption(1);
		if (option == 2)
			load("saves/save1");

		frodo_house = get_frodohouse();
		for (i = 1; i <= frodo_house.level; i++)
			load_houseoption(i);

		return 0;
	}
	if (option == 3)
	{
		free_options();
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

void printw_scroll()
{
	int i, box, interval;
	scrll map_scroll = get_mapscroll();
	unit *free_races = get_freeraces(), *aux;

	pthread_mutex_lock(&l_scroll);
	interval = map_scroll.col / map_scroll.proportion;

	for (i = 1; i <= map_scroll.col; i++)
		mvprintw(map_scroll.row, i, "-");

	box = map_scroll.position;
	if (box > map_scroll.col - interval)
		box = map_scroll.col - interval;
	for (i = box; i <= interval + box; i++)
	{
		mvprintw(map_scroll.row - 1, i, "-");
		mvprintw(map_scroll.row + 1, i, "-");
	}

	mvprintw(map_scroll.row, box, "|");
	mvprintw(map_scroll.row, box + interval, "|");

	for (aux = free_races; aux != NULL; aux = aux->next)
	{
		move(map_scroll.row, aux->position[1]/map_scroll.proportion+1);
		if (aux->race < 5)
			printw("@");
		else
			printw("$");
	}

	pthread_mutex_unlock(&l_scroll);
}

int createmap_win()
{
	map_win = create_win(MAP_ROW+2, size[1], (size[0]-MAP_ROW-2)/2, 0);
	if (map_win == NULL)
		return -1;

	map_panel = new_panel(map_win);
	if (map_panel == NULL)
		return -1;

	show_panel(map_panel);
	top_panel(map_panel);

	return 1;
}

int createmsg_win()
{
	msg_win = create_win(12, 29, size[0]/2 - 6, size[1]/2 - 15);
	if (msg_win == NULL)
		return -1;

	msg_panel = new_panel(msg_win);
	if (msg_panel == NULL)
		return -1;

	hide_panel(msg_panel);

	return 1;
}


/* printa o mapa na janela do terminal */
void refresh_allgame()
{
	if (pthread_mutex_trylock(&l_printmap) == 0)
	{
		clear();

		print_gamebar();
		print_map();

		switch (get_gamestatus())
		{
			case (STATUS_EXIT):
				werase(msg_win);
				print_msgquit();
				break;
			case (STATUS_GAME):
				werase(msg_win);
				print_msghobbit();
				break;
			case (STATUS_PAUSED):
				werase(msg_win);
				print_msgpaused();
				break;
			case (STATUS_SAVING):
				werase(msg_win);
				print_msgsaved();
				break;
			case (STATUS_LOAD):
				werase(msg_win);
				print_msgload();
				break;
			case (STATUS_LOADFAIL):
				werase(msg_win);
				mvwprintw(msg_win, 5, 7, "NOTHING IS SAVED!");

		}
		box(msg_win, 0, 0);

		printw_scroll();
		update_panels();
		doupdate();

		pthread_mutex_unlock(&l_printmap);
	}
}

void print_map()
{
	int i, j;
	int term_col = get_termcol();

	for (i = 1; i < MAP_ROW + 1; i++)
		for (j = 1; (j < MAP_COL) && (j < size[1] - 1); j++)
		{
			wmove(map_win, i, j);
			wprintw(map_win, "%c", map[i-1][j+term_col-1]);
		}
	box(map_win, 0, 0);

}

void print_gamebar()
{
	player user = get_user();

	move((size[0]-MAP_ROW-2)/2 - 1, 0);
	printw("GOLD: %d    FOOD: %d    WOOD: %d    METAL: %d   ",
	user.good[0], user.good[1], user.good[2], user.good[3]);
	move((size[0]-MAP_ROW-2)/2 - 1, size[1] - 27);
	printw("PAUSE   LOAD   SAVE   QUIT");
}

void print_msghobbit()
{
	mvwprintw(msg_win, 1, 2, "PLEASE, CHOOSE WHICH GOOD");
	mvwprintw(msg_win, 2, 2, "YOUR HOBBIT SHOULD COLECT");
	mvwprintw(msg_win, 4, 12, "GOLD");
	mvwprintw(msg_win, 6, 12, "FOOD");
	mvwprintw(msg_win, 8, 12, "WOOD");
	mvwprintw(msg_win, 10, 12, "METAL");
}

void print_msgpaused()
{
	mvwprintw(msg_win, 3, 2, " _            __  ___  _");
	mvwprintw(msg_win, 4, 2, "| \\  /\\  |  | |   |   | \\");
	mvwprintw(msg_win, 5, 2, "|_/ /__\\ |  |  \\  |-- |  |");
	mvwprintw(msg_win, 6, 2, "|   |  | |__| __| |__ |_/");
}

void print_msgquit()
{
	mvwprintw(msg_win, 2, 12, "ARE YOU");
	mvwprintw(msg_win, 3, 4, "SURE YOU WANT TO QUIT?");
	mvwprintw(msg_win, 5, 12, "CANCEL");
	mvwprintw(msg_win, 7, 8, "QUIT AND SAVE");
	mvwprintw(msg_win, 9, 5, "QUIT WITHOUT SAVING");
}

void print_msgsaved()
{
	mvwprintw(msg_win, 3, 3, "__             ___  _");
	mvwprintw(msg_win, 4, 3, "|    /\\  |   | |   | \\");
	mvwprintw(msg_win, 5, 3,  " \\  /__\\  \\ /  |-- |  |");
	mvwprintw(msg_win, 6, 3, "__| |  |   |   |__ |_/");
}

void print_msgload()
{
	mvwprintw(msg_win, 2, 12, "ARE YOU");
	mvwprintw(msg_win, 3, 4, "SURE YOU WANT TO LOAD?");
	mvwprintw(msg_win, 4, 3, "YOU WILL LOSE THIS GAME");
	mvwprintw(msg_win, 7, 9, "[YES]    [NO]");
}

int click_frodooption()
{
	int col = get_termcol();
	MEVENT event = get_event();

	wmouse_trafo(map_win, &event.y, &event.x, false);
	if ((event.y >= MAP_ROW - FRODO_ROW - 8) &&
	   (event.y <= MAP_ROW - FRODO_ROW - 2))
	{
		fortress_buy(col + event.x - 1);
		return 1;
	}

	return 0;
}

void frodo_colect(unit *chr)
{
	MEVENT event;

	move_msg(1);

	while(1)
	{
		if ((getch() == KEY_MOUSE) &&
		   (getmouse(&event) == OK) &&
		   (event.bstate == BUTTON1_CLICKED))
		{
			wmouse_trafo(msg_win, &event.y, &event.x, false);

		  	if ((event.y > 3) && (event.y < 11) && (event.y%2 == 0))
			{
				if (chr->good_type != (event.y - 4) / 2)
				{
					chr->destination[0] = 30;
					chr->destination[1] = 40;
					chr->good_type = (event.y - 4) / 2;
					change_nworkers(chr->good_type, 1);
				}
				break;
			}
		}
	}

	move_msg(0);
}


void change_hobbit(int row, int col)
{
	int term_col = get_termcol(), old_destination[2];
	unit *aux;

	wmouse_trafo(map_win, &row, &col, false);

	for (aux = get_freeraces(); aux != NULL; aux = aux->next)
		if ((aux->race == HOBBIT) &&
		   (row >= aux->position[0]) && (row <= aux->position[0]+10) &&
		   (col + term_col >= aux->position[1]) &&
		   (col + term_col <= aux->position[1] + 19))
		{
			old_destination[0] = aux->destination[0];
			old_destination[1] = aux->destination[1];
			aux->destination[0] = aux->position[0];
			aux->destination[1] = aux->position[1];

			change_nworkers(aux->good_type, -1);
			frodo_colect(aux);

			if ((aux->position[0] == aux->destination[0]) &&
			   (aux->position[1] == aux->destination[1]))
			{
				aux->destination[0] = old_destination[0];
				aux->destination[1] = old_destination[1];
			}
			break;
		}
}

void move_msg(int n)
{
	if (n == 1)
	{
		top_panel(msg_panel);
		show_panel(msg_panel);
	}
	else if (n == 0)
	{
		hide_panel(msg_panel);
		top_panel(map_panel);
	}
}

void quit_answer()
{
	MEVENT event = get_event();

	wmouse_trafo(msg_win, &event.y, &event.x, false);

	if (get_gamestatus() == STATUS_EXIT)
	{
		if (event.y == 5)
			quit_select(2);
		else if (event.y == 7)
			quit_select(3);
		else if (event.y == 9)
			quit_select(4);
	}
}

void load_answer()
{
	MEVENT event = get_event();

	wmouse_trafo(msg_win, &event.y, &event.x, false);

	if ((event.y == 7) && (get_gamestatus() == STATUS_LOAD))
	{
		if (event.x < 16)
			load_select(2);
		else
			load_select(3);
	}
}
