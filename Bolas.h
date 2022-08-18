#include <stdbool.h>

typedef struct
{
   int posX;
   int posY;
   int boundX;
   int boundY;
   double speedX;
   double speedY;
   bool bolaReferencia; // primeira bola a chegar no final da tela;
} Bola;

typedef struct
{
   ElementoDaLista *anterior;
   Bola bola;
   ElementoDaLista *proximo;
} ElementoDaLista;

typedef struct
{
   ElementoDaLista *ponteiroInicio;
} Lista;

Lista *criaLista();
void liberaLista(Lista *lista);
int tamanhoDaLista(Lista *lista);
bool listaVazia(Lista *lista);
bool insereFinalDaLista(Lista *lista, Bola bola);
void inicializaLista(Lista *lista);