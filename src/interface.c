#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include "basis.h"
#include "interface.h"

char *name_filearts[] = { "ASCII art/race_blank.txt",
	                  "ASCII art/hobbit.txt",
			  "ASCII art/elf.txt",
			  "ASCII art/dwarf.txt",
			  "ASCII art/ent.txt",
			  "ASCII art/goblin.txt",
			  "ASCII art/orc.txt",
			  "ASCII art/warg.txt",
			  "ASCII art/troll.txt"
};

char mat_races[N_RACES + 1][RACE_HEIGHT][RACE_WIDTH];

void get_dimension()
{
	getmaxyx(stdscr, size_row, size_col);

	if (size_row < MINIMUM_ROW)
	{
		endwin();
		printf("The terminal's dimension isn't big enough.\n");
		printf("We suggest you use a smaller font.\n");
		exit (1);
	}
}

void load_basicmap(char *file_name, int art_row, int art_col)
{
	FILE *fp = NULL;

	fp = read_file(file_name);

	for (int i = size_row - art_row; i < size_row; i++)
	{
		fscanf(fp, "%[^\n]s", map[i] + art_col);
		fgetc(fp);
		if (feof(fp))
			break;
	}
	fclose(fp);

}

void printw_map(int col)
{
	for (int i = 0; i < size_row; i++)
		for (int j = 0; (j < SIZE_COLUMN) && (j < size_col); j++)
			mvprintw(i, j, "%c", map[i][j + col]);
	refresh();
}

void get_art()
{
	FILE *fp = NULL;

	for (int i = 0; i <= N_RACES; i++)
	{
		fp = read_file(name_filearts[i]);
		for (int j = 0; j < 15; j++)
		{
			fscanf(fp, "%[^\n]s", mat_races[i][j]);
			fgetc(fp);
			if (feof(fp))
				break;
		}
		fclose(fp);
	}
}

void printw_unit(unit chr)
{
	for (int i = RACE_HEIGHT - chr.height; i < RACE_HEIGHT; i++)
	{
		move(chr.position[0] + i, chr.position[1]);
		for (int j = 0; j < RACE_WIDTH; j++)
			printw("%c", mat_races[chr.race][i][j]);
		printw("\n");
	}
}
