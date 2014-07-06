#include <stdlib.h>
#include <pthread.h>
#include "basis.h"
#include "logic.h"
#include "interface.h"

pthread_mutex_t l_key, l_scroll, l_printmap, l_pause, l_unit;
char **map = NULL;
char **options[N_OPTIONS];

pthread_t key_thread;
static const int options_len[] = { NEW_GAME, LOAD_GAME, EXIT_GAME};
static const int good_col[] =
{HOBBIT_GOLD, HOBBIT_FOOD, HOBBIT_WOOD, HOBBIT_METAL,
MORDOR_GOLD, MORDOR_FOOD, MORDOR_WOOD, MORDOR_METAL};

void aloc_map();
void init_map();
int insert_build(int id);

void init_locks()
{
	pthread_mutex_init(&l_key, NULL);
	pthread_mutex_init(&l_scroll, NULL);
	pthread_mutex_init(&l_printmap, NULL);
	pthread_mutex_init(&l_pause, NULL);
	pthread_mutex_init(&l_unit, NULL);
}

void init_thread()
{
	if (pthread_create(&key_thread, NULL, &read_key, NULL))
	{
		endwin();
		fprintf(stderr, "Cannot create thread read_key\n");
		exit(-1);
	}
}

void prepare_map()
{
	aloc_map();
	init_map();
	get_art();
	put_builds();
	map_spaces();
}

/* aloca uma matriz na memoria para guardar o mapa do jogo */
void aloc_map()
{
	int i;

	map = (char**) calloc(MAP_ROW, sizeof(char*));
	for (i = 0; i < MAP_ROW; i++)
		map[i] = (char*) calloc(MAP_COL, sizeof(char));
}

/* inicializa o mapa com espaços */
void init_map()
{
	int i, j;

	for (i = 0; i < MAP_ROW; i++)
		for (j = 0; j < MAP_COL; j++)
                   map[i][j] = ' ';
}

/* libera o mapa */
void free_map()
{
	int i;

	if (map != NULL)
	{
		for (i = 0; i < MAP_ROW; i++)
			if (map[i] != NULL)
				free(map[i]);
		free(map);
		map = NULL;
	}
}

FILE* open_file(char *name, char *mode)
{
	FILE *fp = NULL;

	fp = fopen(name, mode);
	if (fp == NULL)
	{
		endwin();
		printf("%s not found\n", name);
		free_build();
		free_map();
		exit(1);
	}

	return fp;
}

void aloc_options()
{
	int i, j;

	for (i = 0; i < N_OPTIONS; i++)
	{
		options[i] = (char**) calloc(OPTIONS_WIDTH, sizeof(char*));
		for (j = 0; j < OPTIONS_WIDTH; j++)
			options[i][j] = (char*)
			calloc(options_len[i], sizeof(char));
	}
}

void free_options()
{
	int i, j;

	for (i = 0; i < N_OPTIONS; i++){
		for (j = 0; j < OPTIONS_WIDTH; j++)
		{
			if (options[i][j] != NULL)
				free(options[i][j]);
		}
		if (options[i] != NULL)
		{
			free(options[i]);
			options[i] = NULL;
		}
	}
}

int insert_unit(unit **top, int race, unit *saved)
{
	unit *aux, *new = (unit*) calloc(1, sizeof(unit));

	pthread_mutex_lock(&l_unit);
	if (new == NULL)
	{
		pthread_mutex_unlock(&l_unit);
		return -1;
	}

	race_init(new, race);
	if (saved != NULL)
	{
		new->hp = saved->hp;
		new->count_delay = saved->count_delay;
		new->position[0] = saved->position[0];
		new->position[1] = saved->position[1];
		new->destination[0] = saved->destination[0];
		new->destination[1] = saved->destination[1];
		new->good_type = saved->good_type;
		new->backpack = saved->backpack;
	}

	if (*top == NULL)
	{
		pthread_mutex_unlock(&l_unit);
		*top = new;
		return 1;
	}

	if (new->height >= (*top)->height)
	{
		new->next = *top;
		*top = new;
	}
	else
	{
		for (aux = *top; aux != NULL; aux = aux->next)
		{
			if ((aux->next == NULL) ||
			   (aux->next->height <= new->height))
			{
				new->next = aux->next;
				aux->next = new;
				break;
			}
		}
	}
	pthread_mutex_unlock(&l_unit);

	return 1;
}

void free_units(unit **top)
{
	unit *aux = *top;

	while (aux != NULL)
	{
		aux = aux->next;
		free(*top);
		*top = aux;
	}

	*top = NULL;
}

void create_listbuild()
{
	int i;

	for (i = 7; i >= 0; i--)
		if (!insert_build(i))
		{
			endwin();
			printf("Insufficient memory\n");
			exit(1);
		}
}

int insert_build(int id)
{
	build *build_top = get_buildtop();
	build *new = (build*) calloc(1, sizeof(build));

	if (new == NULL)
		return -1;

	new->id = id;
	new->n_workers = 0;
	new->storage = 0;
	new->income = 10;
	new->position[0] = GOOD_ROW;
	new->position[1] = good_col[id];

	if (build_top == NULL)
	{
		set_buildtop(new);
		return 1;
	}

	new->next = build_top;
	build_top = new;
	set_buildtop(build_top);

	return 1;
}

void free_build()
{
	build *build_top = get_buildtop();
	build *aux = build_top;

	while (aux != NULL) {
		aux = aux->next;
		free(build_top);
		build_top = aux;
	}
	set_buildtop(NULL);
}

int get_goodcol(int i)
{
	return good_col[i];
}

int exit_game()
{
	delete_panels();
	free_build();
	free_map();
	endwin();
	exit(1);
}
