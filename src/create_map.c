#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <math.h>
#include "basis.h"
#include "interface.h"

int main()
{
	int col = 0, lim_map;
	//unit hobbit;

	initscr();
	noecho();
	get_dimension();
	aloc_map();
	init_map();
	get_art();
	//hobbit = race_init(HOBBIT);

	load_basicmap("ASCII art/house_frodo.txt", FRODO_ROW, FRODO_COL);
	load_basicmap("ASCII art/mordor_tower.txt", MORDOR_ROW, MORDOR_COL);
	lim_map = sqrt(pow(size_col-SIZE_COLUMN+1, 2));

	while(1)
	{
		erase();
		printw_map(col);
		//printw_unit(hobbit);
		switch (getch())
		{
			case 'a':
				if (col > 0)
					col-=2;
				break;
			case 'd':
				if ((col < lim_map) && (SIZE_COLUMN > size_col))
					col+=2;
				break;
			case 'k':
				free_map();
				endwin();
				return 0;
		}
	}

	free_map();
	endwin();
	return 0;
}

