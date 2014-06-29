typedef struct str_scrll
{
	int col;
	int row;
	int position;
}scrll;

void* read_key(void *arg);
player get_user();
MEVENT get_event();
int get_keystatus();
void set_keystatus(int key_status);
int get_gamestatus();
void set_gamestatus(int key_status);
int get_termcol();
void set_maplim(int lim);
void init_options();
int report_option(int mouse_row, int mouse_col);
void mouse_scroll(int mouse_col);
void arrow_scroll(int direction);
scrll get_mapscroll();
void set_mapscroll(scrll scroll_map);
void term_coltest();
void race_init(unit *chr, int race);
int load_build(char *file_name, int art_row, int art_col);
int get_art();
void map_spaces();
void put_builds();
void printmap_unit(unit chr);
void clear_unit(unit chr);
void move_unit(unit *chr);
void good_generator();
unit* get_freeraces();
void set_freeraces(unit *top);
build* get_buildtop();
void set_buildtop(build *top);
void print_good();
void check_good(unit *chr);
void goto_build(unit *chr, int n_build);
