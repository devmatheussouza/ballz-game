#include <stdbool.h>

typedef struct
{
   int linhaDoBloco;
   int vidas;
   int posX1;
   int posX2;
   int posY1;
   int posY2;
   int boundX;
   int boundY;
} Blocos;

struct ElementoBloco
{
   struct ElementoBloco *anterior;
   Blocos bloco;
   struct ElementoBloco *proximo;
};
typedef struct ElementoBloco ElementoDaListaBloco;

typedef struct
{
   ElementoDaListaBloco *ponteiroInicio;
} ListaBlocos;

ListaBlocos *criaListaBlocos();
void liberaListaBlocos(ListaBlocos *lista);
int tamanhoDaListaBlocos(ListaBlocos *lista);
bool listaVaziaBlocos(ListaBlocos *lista);
void preencheLinhaBlocos(ListaBlocos *lista, int *numeroDaLinha);
bool insereFinalDaListaBlocos(ListaBlocos *lista, Blocos bloco);
void drawBlocos(ListaBlocos *lista, ALLEGRO_FONT *font);