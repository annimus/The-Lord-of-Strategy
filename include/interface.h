void init_interface();
void get_dimension();
WINDOW* create_win(int win_y, int win_x, int starty, int startx);
void destroy_win(WINDOW **local_win);
void menu();
int click_option(int option);
void wprintw_menu(int highlight);
int createmap_win();
void wprintw_map();
