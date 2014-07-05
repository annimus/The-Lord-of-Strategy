/** @file basis.h
  *
  * @brief Header contendo todos os defines e funcoes basicas do programa
  */

#include <ncurses.h>

#define MAP_ROW 40
#define MAP_COL 1000
#define REFRESH_TIME 200000
#define MINIMUM_ROW 50
#define MINIMUN_COL 110

#define STATUS_MOUSE_MOVED 0
#define STATUS_MOUSE_CLICK 1
#define STATUS_UP 2
#define STATUS_DOWN 3
#define STATUS_ENTER 4
#define STATUS_RIGHT 5
#define STATUS_LEFT 6

#define STATUS_MENU 0
#define STATUS_GAME 1
#define STATUS_PAUSED 2
#define STATUS_EXIT -1

#define EXIT 'q'
#define ENTER '\n'
#define PAUSE 'p'
#define YES 'y'
#define NO 'n'

#define SCROLL_RIGHT 9
#define SCROLL_LEFT -9

#define MENU_ROW 40
#define MENU_COL 110
#define N_OPTIONS 3
#define OPTIONS_WIDTH 8
#define NEW_GAME 103
#define LOAD_GAME 116
#define EXIT_GAME 52

#define N_BUILDS 6
#define MORDOR_WIDTH 86
#define MORDOR_ROW 40
#define MORDOR_COL MAP_COL - MORDOR_WIDTH
#define FRODO_WIDTH 121
#define FRODO_ROW 27
#define FRODO_COL 0

#define GOLD_COL1 FRODO_WIDTH + 1
#define FOOD_COL1 FRODO_WIDTH + 87
#define WOOD_COL1 FRODO_WIDTH + 176
#define METAL_COL1 FRODO_WIDTH + 260
#define HOBBIT_GOLD GOLD_COL1 + 8
#define HOBBIT_FOOD FOOD_COL1 + 2
#define HOBBIT_WOOD WOOD_COL1 + 4
#define HOBBIT_METAL METAL_COL1 + 23
#define GOOD_ROW 6

#define USER 0
#define COMP 1

/* races */
#define N_RACES 8
#define RACE_HEIGHT 15
#define RACE_WIDTH 19
#define HOBBIT 1
#define ELF 2
#define DWARF 3
#define ENT 4
#define GOBLIN 5
#define ORC 6
#define WARG 7
#define TROLL 8

/* buildings */
#define MAX_BUILD_LVL 3
#define CURRENT_INCOME(x) 20 + x * 10
#define MAX_FORTRESS_HP 100000;

/** @struct str_unit basis.h
  *
  * @brief Estrutura contendo as informacoes de cada unidade.
  *
  * @param race Indica a raca da unidade.
  *
  * @param hp Indica o quanto de pontos de vida a unidade possui. Varia de
  * acordo com a raca.
  *
  * @param spd_delay Indica o tempo que a unidade precisa para se movimentar
  * antes de poder se movimentar de novo. Varia de acordo com a raca.
  *
  * @param count_delay Conta o tempo nescessario para a unidade se mover de
  * novo. Varia de acordo com a raca.
  *
  * @param dmg Indica o quanto de pontos de vida essa unidade causa a unidades
  * inimigas. varia de acordo com a raca.
  *
  * @param height Indica a altura da unidade.
  *
  * @param position Indica a posicao atual da unidade no mapa.
  *
  * @param destination Indica a posicao do mapa a qual a unidade tentara chegar.
  *
  * @param good_type ...
  *
  * @param backpack ...
  *
  * @param next Indica qual e' a proxima unidade na lista encadadeada.
  */
typedef struct str_unit
{
	int race;
	int hp;
	int spd_delay;
	int count_delay;
	int dmg;
	int height;
	int position[2];
	int destination[2];
	int good_type;
	int backpack;
	struct str_unit *next;
} unit;

/** @struct str_build basis.h
  *
  * @brief Estrutura contendo as informacoes de cada construcao.
  *
  * @param id Indica o tipo de construcao.
  *
  * @param level Indica o nivel de aprimoramento da construcao.
  *
  * @param storage Indica a quantidade de recurso que construcao possui
  * armazenado.
  *
  * @param income Indica a quantidade de recurso que a construcao recebe por
  * looping. Varia de acordo com o nivel da construcao.
  *
  * @param position Indica a posicao da construcao no mapa.
  *
  * @param next Indica qual e' a proxima unidade na lista encadeada.
  */
typedef struct str_build
{
	int id;
	int level;
	int storage;
	int income;
	int position[2];
	struct str_build *next;
} build;


/** @struct str_fortress basis.h
  *
  * @brief Estrutura que contem as informacoes da fortaleza.
  *
  * @param id Indica o tipo da fortaleza: usuario ou computador.
  *
  * @param hp Indica quantos pontos de vida a fortaleza possui.
  *
  * @param level Indica o nivel atual da fortaleza.
  *
  * @param work_time ...
  */
typedef struct str_fortress
{
	int id;
	int hp;
	int level;
	int work_time;
} fortress;

/** @struct str_player basis.h
  *
  * @brief Estrutura contendo as informacoes do jogador.
  *
  * @param id Indica o tipo de jogador: usuario ou computador.
  *
  * @param good ...
  */
typedef struct str_player
{
	int id;
	int good[4];
} player;

extern pthread_mutex_t l_key, l_scroll, l_printmap, l_pause, l_unit;
extern char **map;
extern char **options[N_OPTIONS];

/** Funcao: Inicializar os locks, tambem conhecidos como mutex
  *
  * Descricao/Hipotese: Inicializa as variaveis que serao responsaveis pela
  *	                sincronizacao das threads do projeto.
  *
  * Interface explicita: N/A
  *
  * Interface implicita: pthread_mutex_t l_key
  *                      pthread_mutex_t l_scroll
  *                      pthread_mutex_t l_printmap
  *                      pthread_mutex_t l_pause
  *                      pthread_mutex_t l_unit
  *
  * Contrato/Requistos: N/A.
  *
  * @return Nao possui retorno.
  */
void init_locks();

/** Funcao: Inicializar a segunda thread usada no projeto.
  *
  * Descricao/Hipotese: Inicializa a thread que coleta e trata entrada de
  *                     dados. Caso nao seja possivel inicializa-la, a funcao
  *                     mostra mensagem de erro e da exit passando -1 como
  *                     parametro de saida.
  *
  * Interface explicita: N/A
  *
  * Interface implicita: pthread_t key_thread: thread que sera criada.
  *                      void read_key: funcao/metodo que a nova thread ira
  *                                     executar como sendo a principal.
  *
  * Contrato/Requistos: N/A.
  *
  * @return Nao possui retorno.
  */

void init_thread();

/** Funcao: Prepara a matriz de caracteres que ira conter o mapa.
  *
  * Descricao/Hipotese: Aloca o espaco para o mapa a partir de um ponteiro de
  *                     ponteiro de char, inicializa tudo com espacos, coloca
  *                     as ASCII art de predios e fortalezas no mapa e coloca
  *                     espacoes onde possivelmente existam marcadores '\0'.
  *
  * Interface explicita: N/A
  *
  * Interface implicita: char **map: ponteiro de ponteiro no qual sera alocada
  *                                  a matriz do mapa.
  *
  * Contrato/Requistos: A variavel map deve estar inicializada como NULL.
  *
  * @return Nao possui retorno.
  */
void prepare_map();

/** Funcao: Liberar a matriz do mapa.
  *
  * Descricao/Hipotese: Libera a matriz, que contem o mapa, da memoria do
  *                     computador. Se a matriz foi liberada corretamente, a
  *                     variavel mapa e' igualada a NULL.
  *
  * Interface explicita: N/A
  *
  * Interface implicita: char **map: ponteiro de ponteiro no qual a matriz dp
  *                                  mapa deve estar alocada.
  *
  * Contrato/Requisitos: N/A.
  *
  * @return Nao possui retorno.
  */
void free_map();

/** Funcao: Abre um arquivo.
  *
  * Descricao/Hipotese: Abre um arquivo para o modo especificado pelo segundo
  *                     parametro. Caso nao seja possivel, o programa mostra
  *                     uma mensagem de erro, libera as possiveis alocacoes e
  *                     da exit passando o parametro 1. Caso seja, a funcao
  *                     retorna o ponteiro do arquivo.
  *
  * Interface explicita: @param char *name: nome do arquivo que a funcao
  *                      tentara abrir.
  *                      @param char *mode: como o arquivo sera aberto, por
  *                      exemplo, "r", "w" e etc.
  *
  * Interface implicita: N/A.
  *
  * Contrato/Requisitos: O parametro name deve ser diferente de NULL e deve
  *                      corresponder a um nome de um arquivo existente e que
  *                      possa ser lido como arquivo texto.
  *
  * @return Um ponteiro de FILE, do arquivo que foi aberto.
  */
FILE* open_file(char *name, char *mode);

/** Funcao: Aloca o espaco na memoria para a matriz de opcoes do mapa.
  *
  * Descricao/Hipotese: Aloca uma matriz na memoria para guardar as opcoes, a
  *                     partir de um ponteiro de ponteiro de char.
  *
  * Interface explicita: N/A.
  *
  * Interface implicita: @param char **options[]: onde sera alocado o espaco
  *                      para guardar as ASCII art das opcoes do mapa
  *
  * Contrato/Requisitos: N/A.
  *
  * @return Nao possui retorno.
  */
void aloc_options();

/** Funcao: Libera a matriz de opcoes do menu.
  *
  * Descricao/Hipotese: Libera o espaco alocado para a matriz de opcoes do
  *                     menu. Se a matriz for  liberada corretamente, a
  *                     variavel options e' igualada 'a NULL.
  *
  * Interface explicita: N/A.
  *
  * Inteface implicita: char **options[]: onde deve ter sido alocado o espaco
  *                     para guardar as ASCII art das opcoes do mapa.
  *
  * Contrato/Requisitos: N/A.
  *
  * @return Nao possui rentorno.
  */
void free_options();

/** Funcao: Aloca e insere uma unidade na lista encadeada de unidades.
  *
  * Descricao/Hipotese: Aloca o espaco na memoria para uma variavel do tipo
  *                     unit e inicializa ela conforma a raca que devera ser
  *                     essa unidade. Depois, insere essa variavel na lista
  *                     de unidades, de ordem ordenada decrescentemente de
  *                     acordo com o tamanho de cada unidade.
  *
  * Interface explicita: @param unit **top: ponteiro de ponteiro do topo da
  *                      lista de unidades (Passagem por referencia do topo
  *                      da lista.
  *                      @param int race: indica a raca da nova unidade.
  *                      @param unit *saved: ponteiro para uma unidade que possa
  *                      estar sendo carregado em um load.
  *
  * Contrato/Requisitos: Caso a lista de unidades esteja vazia, a variavel
  *                      top deve estar inicializada com NULL. Alem disso, a
  *                      variavel race deve estar entre 1 e 8.
  *
  * @return -1, caso nao seja possivel criar a nova unidade.
  * @return  1, caso consiga criar e inserir a nova unidade.
  */
int insert_unit(unit **top, int race, unit *saved);

/** Funcao: Libera a lista encadeada de unidades.
  *
  * Descricao/Hipotese: Libera elemento a elemento da lista encadeada de
  *                     unidades. Se a lista for liberada corretamente, a
  *                     variavel top e' igualada 'a NULL.
  *
  * Interface explicita: @param unit **top: ponteiro de ponteiro contendo o
  *                      topo da lista de unidades.
  *
  * Contrato/Requisito: A variavel top deve ser diferente de NULL, caso
  *                     contrario nao a nada a ser liberado.
  *
  * @return Nao possui retorno.
  */
void free_units(unit **top);

/** Funcao: Cria a lista encadeada de construcoes.
  *
  * Descricao/Hipotese: Cria a lista encadeada de construcoes que serao
  *                     utilizadas no jogo. Caso nao seja possivel criar a
  *                     lista, o programa mostra uma mensaem de erro, da exit
  *                     passando o parametro 1.
  *
  * Interface explicita: N/A.
  *
  * Interface implicita: build *build_top: Ponteiro para o inicio da lista que
  *                      ira guardar as contrucoes.
  *
  * Contrato/Requisito: A variavel build_top, que faz parte de outro modulo,
  *                     deve ser inicializada como NULL.
  *
  * @return Nao possui retorno.
  */
void create_listbuild();

/** Funcao: Liberar a lista encadeada de construcoes.
  *
  * Descricao/Hipotese: Libera, elemento a elemento, a lista encadeada de
  *                     construcoes. Se a lista for liberada corretamente, a
  *                     variavel build_top e' igualada 'a NULL.
  *
  * Interface explicita: N/A.
  *
  * Interface implicita: build *build_top: Ponteiro para o inicio da lista que
  *                      guarda as construcoes.
  *
  * Contrato/Requisito: A variavel build_top, que faz parte de outro modulo,
  *                     deve ser diferente de NULL, caso contrario nao ha nada
  *                     para ser liberado.
  *
  * @return Nao possui retorno.
  */
void free_build();

/** Funcao: Fornecer o valor da variavel good_col para outros modulos.
  *
  * Descricao/Hipotese: Como boa pratica de programacao, a variavel good_col,
  *                     foi encapsulada. Essa funcao retorna os valores de
  *                     posicoes desse vetor, para que outros modulos tenham
  *                     acesso a esses valores.
  *
  * Interface explicita: @param i Indica qual das construcoes do usuario
  *                      deseja-se saber a posicao.
  *
  * Interface implicita: static const int good_col[]: variavel que guarda as
  *                      colunas onde se deve recolher os recursos dos predios.
  *
  * Contrato/Requisito: O parametro i deve estar entre 0 e 3.
  *
  * @return good_col[i]
  */
int get_goodcol(int i);

/** Funcao: Sair do jogo.
  *
  * Descricao/Hipotese: Chama as funcoes de liberar construcoes e mapa, libera
  *                     a interface e sai do programa passando o parametro 1
  *                     para a funcao exit.
  *
  * Interface explicita: N/A.
  *
  * Interface implicita: N/A.
  *
  * Contrato/Requisito: N/A.
  *
  * @returm Nao possui retorno.
  */
int exit_game();
