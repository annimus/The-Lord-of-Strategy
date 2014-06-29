#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ncurses.h>
#include <pthread.h>
#include <math.h>
#include "basis.h"
#include "logic.h"
#include "interface.h"

struct build_art
{
	char name[50];
	int row, col;
};

static char *options_files[] = { "ASCII art/new_game.txt",
			"ASCII art/load_game.txt",
			"ASCII art/exit.txt"};


static struct build_art builds[]={
{"ASCII art/house_frodo.txt", MAP_ROW - FRODO_ROW, FRODO_COL},
{"ASCII art/mordor_tower.txt",  MAP_ROW - MORDOR_ROW, MORDOR_COL},
{"ASCII art/gold_build.txt", 1,  GOLD_COL1},
{"ASCII art/food_build.txt", 1, FOOD_COL1},
{"ASCII art/wood_build.txt", 1, WOOD_COL1},
{"ASCII art/metal_build.txt", 1, METAL_COL1}
};

/* matriz contendo o endereço da ascii art do jogo */
static char *name_filearts[] = { "ASCII art/hobbit.txt",
			         "ASCII art/elf.txt",
				 "ASCII art/dwarf.txt",
				 "ASCII art/ent.txt",
				 "ASCII art/goblin.txt",
				 "ASCII art/orc.txt",
				 "ASCII art/warg.txt",
				 "ASCII art/troll.txt"
};

static unit attr[] = {
{HOBBIT, 100, 1, 10, 10, {30, 40}, {30, 40}, 100, {0, 0, 0, 0}, NULL},
{ELF, 130, 4, 40, 12, {28, 40}, {28, 40}, 200, {0, 0, 0, 0}, NULL},
{DWARF, 250, 2, 30, 10, {30, 40}, {30, 40}, 250, {0, 0, 0, 0}, NULL},
{ENT, 500, 1, 50, 15, {25, 40}, {25, 40}, 300, {0, 0, 0, 0}, NULL},
{GOBLIN, 100, 1, 10, 10, {30, 34+MORDOR_COL}, {30, 34+MORDOR_COL}, 100,
{0, 0, 0, 0}, NULL},
{ORC, 200, 2, 30, 12, {28, 34+MORDOR_COL}, {28, 34+MORDOR_COL}, 200,
{0, 0, 0, 0}, NULL},
{WARG, 120, 5, 25, 8, {31, 34+MORDOR_COL}, {31, 34+MORDOR_COL}, 250,
{0, 0, 0, 0}, NULL},
{TROLL, 500, 1, 50, 15, {25, 34+MORDOR_COL}, {25, 34+MORDOR_COL}, 300,
{0, 0, 0, 0}, NULL}
};

static int good_col[] = {HOBBIT_GOLD, HOBBIT_FOOD, HOBBIT_WOOD, HOBBIT_METAL};

scrll map_scroll;
int status[2];
int lim_map = 0;
int term_col = 1;
char mat_races[N_RACES + 1][RACE_HEIGHT][RACE_WIDTH];
MEVENT event;
build *build_top = NULL;
unit *free_races = NULL;
player user = {0, 0, 0, 0, 0};

void* read_key(void *arg)
{
	while (1)
	{
		pthread_mutex_lock(&l_key);
		switch(getch())
		{
			case (KEY_MOUSE):
				if(getmouse(&event) == OK)
				{
					status[0] = STATUS_MOUSE_MOVED;
					if (event.bstate == BUTTON1_CLICKED)
					{
						status[0] = STATUS_MOUSE_CLICK;
						if ((event.y >= map_scroll.row-1) &&
						   (event.y <= map_scroll.row+1))
							mouse_scroll(event.x);
					}
				}
				break;
			case (KEY_UP):
				status[0] = STATUS_UP;
				break;
			case (KEY_DOWN):
				status[0] = STATUS_DOWN;
				break;
			case (KEY_RIGHT):
				if (status[1] != STATUS_MENU)
					arrow_scroll(SCROLL_RIGHT);
				break;
			case (KEY_LEFT):
				if (status[1] != STATUS_MENU)
					arrow_scroll(SCROLL_LEFT);
				break;
			case (EXIT):
				free_build();
				free_map();
				endwin();
				exit(1);
			case (ENTER):
				status[0] = STATUS_ENTER;
		}
		pthread_mutex_unlock(&l_key);
		usleep(2);
	}

	return arg;
}

player get_user()
{
	return user;
}

MEVENT get_event()
{
	return event;
}

int get_keystatus()
{
	return status[0];
}

void set_keystatus(int key_status)
{
	status[0] = key_status;
}

int get_gamestatus()
{
	return status[1];
}

void set_gamestatus(int game_status)
{
	status[1] = game_status;
}

int get_termcol()
{
	return term_col;
}

void set_maplim(int lim)
{
	lim_map = lim;
}

void init_options()
{
	char ***options = get_options();
	int i, j;
	FILE *fp = NULL;

	for (i = 0; i < N_OPTIONS; i++)
	{
		fp = read_file(options_files[i]);
		for (j = 0; j < OPTIONS_WIDTH; j++)
		{
			fscanf(fp, "%[^\n]s", options[i][j]);
			if (feof(fp))
				break;
			fgetc(fp);
		}
		fclose(fp);
	}
}

int report_option(int mouse_row, int mouse_col)
{
	if ((mouse_row > 1) && (mouse_row < MENU_ROW - 2) &&
	   (mouse_col > 1) && (mouse_col < MENU_COL - 2))
		return 1 + (mouse_row/ 13);
	return 0;
}

void mouse_scroll(int mouse_col)
{
	int n;

	n = MAP_COL / map_scroll.col + 1;
	if ((mouse_col > 0) && (mouse_col <= map_scroll.col) &&
	   (MAP_COL > get_sizecol()))
	{
		map_scroll.position = mouse_col - 1;
		term_col = n * map_scroll.position;
		term_coltest();
		wprintw_map();
	}
}

void arrow_scroll(int direction)
{
	if ((term_col <= lim_map) && (MAP_COL > get_sizecol()) &&
	   (term_col >= 0))
	{
		term_col+=direction;
		term_coltest();
		map_scroll.position = term_col / (MAP_COL / map_scroll.col + 1);
		wprintw_map();
	}
}

scrll get_mapscroll()
{
	return map_scroll;
}

void set_mapscroll(scrll scroll_map)
{
	map_scroll = scroll_map;
}

void term_coltest()
{
	if (term_col < 0)
		term_col = 0;
	if (term_col > lim_map)
		term_col = lim_map;
}

/* inicializa as unidades com os atributos pre-definidos */
void race_init(unit *chr, int race)
{
	int i;

	for (i = 0; i < N_RACES; i++)
		if (attr[i].race == race)
		{
			chr->race = attr[i].race;
			chr->hp = attr[i].hp;
			chr->spd = attr[i].spd;
			chr->dmg = attr[i].dmg;
			chr->height = attr[i].height;
			chr->position[0] = attr[i].position[0];
			chr->position[1] = attr[i].position[1];
			chr->destination[0] = attr[i].destination[0];
			chr->destination[1] = attr[i].destination[1];
			chr->spwan_time = attr[i].spwan_time;
			break;
		}
}

/* carrega o mapa com as fortalezas do jogador e do computador */
int load_build(char *file_name, int art_row, int art_col)
{
	int i;
	FILE *fp = NULL;

	fp = read_file(file_name);

	for (i = art_row; i < MAP_ROW; i++)
	{
		fscanf(fp, "%[^\n]s", map[i] + art_col);
		fgetc(fp);
		if (feof(fp))
			break;
	}
	fclose(fp);

	return 1;
}

void put_builds()
{
	int i;

	for (i = 0; i < N_BUILDS; i++)
		load_build(builds[i].name, builds[i].row, builds[i].col);
}

/* le a ascii art das racas */
int get_art()
{
	int i, j;
	FILE *fp = NULL;

	for (i = 0; i < N_RACES; i++)
	{
		fp = read_file(name_filearts[i]);
		for (j = 0; j < 15; j++)
		{
			fscanf(fp, "%[^\n]s", mat_races[i][j]);
			fgetc(fp);
			if (feof(fp))
				break;
		}
		fclose(fp);
	}

	return 1;
}

void map_spaces()
{
	int i, j;

	for (i = 0; i < MAP_ROW; i++)
		for (j = 0; j < MAP_COL; j++)
			if (map[i][j] == '\0')
				map[i][j] = ' ';
}

/* printa as unidades na tela do terminal */
void printmap_unit(unit chr)
{
	int i, j, row = chr.position[0], col = chr.position[1];

	for (i = RACE_HEIGHT - chr.height; i < RACE_HEIGHT; i++)
	{
		for (j = 0; j < RACE_WIDTH; j++)
		{
			if (mat_races[chr.race-1][i][j] != ' ')
				map[row][col] = mat_races[chr.race-1][i][j];
			col++;
		}
		col = chr.position[1];
		row++;
	}
}

void clear_unit(unit chr)
{
	int i, j, row = chr.position[0], col = chr.position[1];

	for (i = RACE_HEIGHT - chr.height; i < RACE_HEIGHT; i++)
	{
		for (j = 0; j < RACE_WIDTH; j++)
			map[row][col++] = ' ';
		col = chr.position[1];
		row++;
	}

	if (col < FRODO_WIDTH)
		load_build(builds[0].name, builds[0].row, builds[0].col);
	else if (col > MORDOR_COL)
		load_build(builds[1].name, builds[1].row, builds[1].col);
	map_spaces();
}

void move_unit(unit *chr)
{
	int dest = 1, n = 1;

	if (chr->race > 4)
		dest = -1;
	if ((chr->position[1]*dest >= chr->destination[1]*dest) &&
	   (chr->position[0] != chr->destination[0]))
	{
		n = 0;
	}

	if (chr->position[n]*dest > chr->destination[n]*dest)
		dest = dest * -1;
	if (chr->position[n]*dest < chr->destination[n]*dest)
	{
		if (n == 0)
			chr->position[0]+= chr->height * dest;
		else
			chr->position[1]+= 9 * dest;
		if (chr->position[n]*dest > chr->destination[n]*dest)
			chr->position[n] = chr->destination[n];
	}
}

void good_generator()
{
	build *aux;

	for (aux = build_top; aux != NULL; aux = aux->next)
	{
		if (aux->storage != pow(10, aux->level+2) / 2)
			aux->storage+=aux->income;
	}
}

unit* get_freeraces()
{
	return free_races;
}

void set_freeraces(unit *top)
{
	free_races = top;
}

build* get_buildtop()
{
	return build_top;
}

void set_buildtop(build *top)
{
	build_top = top;
}

void print_good()
{
	int i = 0, j = 0, n = 2, storage = 0, num = 0;
	build *aux = NULL;

	for (i = MAP_COL - FRODO_COL; i < MAP_COL - MORDOR_COL; i++)
		map[0][i] = ' ';

	i = 0;
	for (aux = build_top; aux != NULL; aux = aux->next)
	{
		storage = aux->storage;

		for (i = 0; i < 6; i++)
			map[0][builds[n].col+i] = ' '; 

		for (i = 999999; i > 0; i = i/10)
		{
			if (storage > i)
			{
				num = storage/(i+1);
				map[0][builds[n].col+j++] = num + 48;
				storage = storage - ((i + 1) * num);
			}
			else if (j > 0)
					map[0][builds[n].col+j++] = 48;
		}
		map[0][builds[n].col+j] = storage + 48;
		n++;
		j = 0;
	}
}

void check_good(unit *chr)
{
	int i, j;
	build *aux = build_top;

	for (i = 0; i < N_BUILDS - 2; i++)
	{
		if ((chr->position[1] == good_col[i]) &&
				(chr->position[0] == GOOD_ROW))
		{
			while ((aux != NULL) && (aux->id != i))
				aux = aux->next;

			if (aux->id == i)
			{
				chr->backpack[i]+=aux->storage;
				aux->storage = 0;
				chr->destination[0] = 30;
				chr->destination[1] = 40;
			}
		}
		else if ((chr->position[1] == 40) &&
				(chr->position[0] == 30))
		{
			if (chr->race == HOBBIT)
			{
				user.gold+=chr->backpack[0];
				user.food+=chr->backpack[1];
				user.wood+=chr->backpack[2];
				user.metal+=chr->backpack[3];
				for (j = 0; j < 4; j++)
					chr->backpack[j] = 0;
			}
		}
		else
			break;
	}
}

void goto_build(unit *chr, int n_build)
{
	int i;

	if (chr->race == HOBBIT)
		for (i = 0; i < N_BUILDS - 2; i++)
			if (i == n_build)
			{
				chr->destination[0] = GOOD_ROW;
				chr->destination[1] = good_col[i];
				break;
			}
}
