#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdbool.h>

typedef struct {
    int linhaDoBloco;
    int vidas;
    int posX1;
    int posX2;
    int posY1;
    int posY2;
    bool item;
    bool descerItem;
    int blinkItem;
} Blocos;

struct ElementoBloco {
    struct ElementoBloco *anterior;
    Blocos bloco;
    struct ElementoBloco *proximo;
};
typedef struct ElementoBloco ElementoDaListaBloco;

typedef struct {
    ElementoDaListaBloco *ponteiroInicio;
} ListaBlocos;

ListaBlocos *criaListaBlocos();
void liberaListaBlocos(ListaBlocos *lista);
int tamanhoDaListaBlocos(ListaBlocos *lista);
bool listaVaziaBlocos(ListaBlocos *lista);
void preencheLinhaBlocos(ListaBlocos *lista, int *score);
bool insereFinalDaListaBlocos(ListaBlocos *lista, Blocos bloco);
void drawBlocos(ListaBlocos *lista, ALLEGRO_FONT *font);
void updateBlocos(ListaBlocos *lista, bool *descerBlocos, bool *atirouBola, int *score);