#include <stdbool.h>

typedef struct
{
   bool viva;
   double posX;
   double posY;
   int boundX;
   int boundY;
   double speedX;
   double speedY;
   bool bolaReferencia; // primeira bola a chegar no final da tela;
   bool shooted;
} Bola;

struct Elemento
{
   struct Elemento *anterior;
   Bola bola;
   struct Elemento *proximo;
};
typedef struct Elemento ElementoDaLista;

typedef struct
{
   ElementoDaLista *ponteiroInicio;
} Lista;

Lista *criaLista();
void liberaLista(Lista *lista);
int tamanhoDaLista(Lista *lista);
bool listaVazia(Lista *lista);
bool insereFinalDaLista(Lista *lista);
Bola criaBola();
void drawBolas(Lista *lista);
void updateBolas(Lista *lista);
void colisaoBolas(Lista *lista, bool *atirouBola);
void atiraBolas(Lista *lista, double x_mouse, double y_mouse);