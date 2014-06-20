#include <stdlib.h>
#include <ncurses.h>
#include "basis.h"

static unit attributes[] = {
	{HOBBIT, 100, 1, 10, 10, {0, 0}, {0, 0}, RIGHT, 100},
	{ELF, 130, 4, 40, 12, {0, 0}, {0, 0}, RIGHT, 200},
	{DWARF, 250, 2, 30, 10, {0, 0}, {0, 0}, RIGHT, 250},
	{ENT, 500, 1, 50, 15, {0, 0}, {0, 0}, RIGHT, 300},
	{GOBLIN, 100, 1, 10, 10, {0, 0}, {0, 0}, LEFT, 100},
	{ORC, 200, 2, 30, 12, {0, 0}, {0, 0}, LEFT, 200},
	{WARG, 120, 5, 25, 8, {0, 0}, {0, 0}, LEFT, 250},
	{TROLL, 500, 1, 50, 15, {0, 0}, {0, 0}, LEFT, 300}
};

char **map;
int size_row, size_col;

/* aloca uma matriz na memoria para guardar o mapa do jogo */
void aloc_map()
{
	map = (char**) calloc(size_row, sizeof(char*));
	for (int i = 0; i < size_row; i++)
		map[i] = (char*) calloc(SIZE_COLUMN, sizeof(char));
}

/* inicializa o mapa com espaços */
void init_map(){
	for (int i = 0; i < size_row; i++)
		for (int j = 0; j < SIZE_COLUMN; j++)
                   map[i][j] = ' ';
}

/* libera o mapa */
void free_map()
{
	for (int i = 0; i < size_row; i++)
		free(map[i]);
	free(map);
}

/* le os arquivos .txt contendo a ascii art do jogo */
FILE* read_file(char *name)
{
	FILE *fp = NULL;

	fp = fopen(name, "r");
	if (fp == NULL)
	{
		endwin();
		printf("%s not found\n", name);
		free_map();
		exit(1);
	}

	return fp;
}

/* inicializa as unidades com os atributos pre-definidos */
unit race_init(int race)
{
	unit new_unit;

	for (int i = 0; i < N_RACES; i++)
		if (attributes[i].race == race)
		{
			new_unit.race = attributes[i].race;
			new_unit.hp = attributes[i].hp;
			new_unit.spd = attributes[i].spd;
			new_unit.dmg = attributes[i].dmg;
			new_unit.height = attributes[i].height;
			new_unit.position[0] = attributes[i].position[0];
			new_unit.position[1] = attributes[i].position[1];
			new_unit.destination[0] = attributes[i].destination[0];
			new_unit.destination[1] = attributes[i].destination[1];
			new_unit.direction = attributes[i].direction;
			new_unit.spwan_time = attributes[i].direction;
			break;
		}
	return new_unit;
}
