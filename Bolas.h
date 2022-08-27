#include <stdbool.h>

#include "Blocos.h"

typedef struct {
    bool viva;
    double posX;
    double posY;
    double speedX;
    double speedY;
    bool bolaReferencia;  // primeira bola a chegar no final da tela;
    bool shooted;
} Bola;

struct Elemento {
    struct Elemento *anterior;
    Bola bola;
    struct Elemento *proximo;
};
typedef struct Elemento ElementoDaLista;

typedef struct {
    ElementoDaLista *ponteiroInicio;
} ListaBolas;

ListaBolas *criaListaBolas();
void liberaListaBolas(ListaBolas *lista);
int tamanhoDaListaBolas(ListaBolas *lista);
bool listaVaziaBolas(ListaBolas *lista);
bool insereFinalDaListaBolas(ListaBolas *lista);
Bola criaBola();
void drawBolas(ListaBolas *lista);
void drawMiraBolas(ListaBolas *lista, double xMouse, double yMouse, double xReferencia);
void updateBolas(ListaBolas *lista);
void colisaoBolas(ListaBolas *lista, bool *atirouBola, ListaBlocos *listaBlocos, int *qntBolasAdicionadas, int *qntBolasMortas);
void atiraBolas(ListaBolas *lista, double x_mouse, double y_mouse);