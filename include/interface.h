#define SIZE_COLUMN 300

extern int size_row, size_col;
extern char **map;

void get_dimension();
void aloc_map();
void init_map();
void read_art(char *file_name, int art_row, int art_col);
void printw_map(int col);
void free_map();

