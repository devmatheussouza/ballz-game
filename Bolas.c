#include "Bolas.h"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define RES_WIDTH 800
#define RES_HEIGHT 960
#define RAIO 8
#define SPEED 10
#define HEIGHT_LANCAMENTO 800

int quantidadeBolasMortas = 0;
bool primeiraBolaMorta = false, flagTodasMorreram = false;
double posX_Ref = 0;

ListaBolas *criaListaBolas() {
    ListaBolas *lista = malloc(sizeof(ListaBolas));
    if (lista != NULL) lista->ponteiroInicio = NULL;
    return lista;
}

void liberaListaBolas(ListaBolas *lista) {
    if (lista == NULL) return;
    if (listaVaziaBolas(lista)) free(lista);
    ElementoDaLista *nodo_remocao;
    while (lista->ponteiroInicio != NULL) {
        nodo_remocao = lista->ponteiroInicio;
        lista->ponteiroInicio = lista->ponteiroInicio->proximo;
        free(nodo_remocao);
    }
    free(lista);
}

int tamanhoDaListaBolas(ListaBolas *lista) {
    if (lista == NULL) return 0;
    int cont = 0;
    ElementoDaLista *no = lista->ponteiroInicio;
    while (no != NULL) {
        cont++;
        no = no->proximo;
    }
    return cont;
}

bool listaVaziaBolas(ListaBolas *lista) {
    if (lista == NULL) return false;
    if (lista->ponteiroInicio == NULL) return true;
    return false;
}

bool insereFinalDaListaBolas(ListaBolas *lista) {
    if (lista == NULL) return false;

    ElementoDaLista *no = malloc(sizeof(ElementoDaLista));
    if (no == NULL) return false;

    no->bola = criaBola();
    no->proximo = NULL;
    if (lista->ponteiroInicio == NULL) {
        no->anterior = NULL;
        lista->ponteiroInicio = no;
    } else {
        ElementoDaLista *aux = lista->ponteiroInicio;
        while (aux->proximo != NULL) aux = aux->proximo;
        aux->proximo = no;
        no->anterior = aux;
        no->bola.posX = no->anterior->bola.posX;
        no->bola.posY = no->anterior->bola.posY;
    }

    return true;
}

Bola criaBola() {
    Bola novaBola;
    novaBola.viva = false;
    novaBola.posX = RES_WIDTH / 2;
    novaBola.posY = HEIGHT_LANCAMENTO - RAIO;
    novaBola.speedX = -SPEED;
    novaBola.speedY = SPEED;
    novaBola.bolaReferencia = false;
    novaBola.shooted = false;
    return novaBola;
}

void drawBolas(ListaBolas *lista) {
    ElementoDaLista *aux = lista->ponteiroInicio;
    while (aux != NULL) {
        if (aux->bola.viva == true) al_draw_filled_circle(aux->bola.posX, aux->bola.posY, RAIO, al_map_rgba(255, 255, 255, 255));
        aux = aux->proximo;
    }
}

void drawMiraBolas(ListaBolas *lista, double xMouse, double yMouse, double xReferencia) {
    double yDist = (HEIGHT_LANCAMENTO - yMouse - RAIO) / 10;
    double xDist = (xMouse - xReferencia) / 10;
    double raio = RAIO;

    if (yDist > 50) {
        raio = RAIO - 1;
    } else if (yDist > 40) {
        raio = RAIO - 2;
    } else {
        raio = RAIO - 3;
    }

    ElementoDaLista *aux = lista->ponteiroInicio;
    while (aux != NULL) {
        if (yMouse < HEIGHT_LANCAMENTO) {
            if (aux->anterior == NULL) {
                aux->bola.posX = xReferencia + xDist;
                aux->bola.posY = HEIGHT_LANCAMENTO - RAIO - yDist;
                al_draw_filled_triangle(aux->bola.posX, aux->bola.posY, xReferencia - 10, HEIGHT_LANCAMENTO - 12, xReferencia + 10,
                                        HEIGHT_LANCAMENTO - 12, al_map_rgb(255, 255, 255));
            } else {
                aux->bola.posX = aux->anterior->bola.posX + xDist;
                aux->bola.posY = aux->anterior->bola.posY - yDist;
                al_draw_filled_circle(aux->bola.posX, aux->bola.posY, raio, al_map_rgb(255, 255, 255));
            }
        }
        aux = aux->proximo;
    }
}

void updateBolas(ListaBolas *lista) {
    ElementoDaLista *aux = lista->ponteiroInicio;
    while (aux != NULL) {
        if (aux->bola.viva == true) {
            if (aux->anterior == NULL) {
                aux->bola.posX += aux->bola.speedX;
                aux->bola.posY += aux->bola.speedY;
            } else {
                if ((abs(aux->bola.posX - aux->anterior->bola.posX) >= 30 || abs(aux->bola.posY - aux->anterior->bola.posY) >= 30) &&
                    aux->bola.shooted == false)
                    aux->bola.shooted = true;

                if (aux->bola.shooted == true) {
                    aux->bola.posX += aux->bola.speedX;
                    aux->bola.posY += aux->bola.speedY;
                }
            }
        }

        aux = aux->proximo;
    }
}

void colisaoBolas(ListaBolas *listaBolas, bool *atirouBola, ListaBlocos *listaBlocos, int *qntBolasAdicionadas, int *qntBolasMortas,
                  ALLEGRO_SAMPLE *hitBola) {
    ElementoDaListaBloco *auxBloco = listaBlocos->ponteiroInicio;
    ElementoDaLista *aux = listaBolas->ponteiroInicio;

    while (aux != NULL && ((*atirouBola) == true)) {
        if (aux->bola.viva == true) {
            if ((aux->bola.posX > RES_WIDTH - SPEED && aux->bola.speedX > 0) || (aux->bola.posX < SPEED && aux->bola.speedX < 0))
                aux->bola.speedX *= -1;

            if ((aux->bola.posY < 160 + RAIO && aux->bola.speedY < 0)) aux->bola.speedY *= -1;

            while (auxBloco != NULL) {
                if (auxBloco->bloco.vidas > 0) {
                    if ((aux->bola.posX >= auxBloco->bloco.posX1 - SPEED && aux->bola.posX <= auxBloco->bloco.posX2 + SPEED) &&
                        ((aux->bola.posY <= auxBloco->bloco.posY1 && aux->bola.posY >= auxBloco->bloco.posY1 - SPEED) &&
                         (aux->bola.speedY > 0))) {
                        al_play_sample(hitBola, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                        if (auxBloco->bloco.item == true) {
                            auxBloco->bloco.descerItem = true;
                            (*qntBolasAdicionadas)++;
                        } else {
                            aux->bola.speedY *= -1;
                        }
                        if (auxBloco->bloco.vidas >= 1) auxBloco->bloco.vidas--;
                    }

                    if ((aux->bola.posX >= auxBloco->bloco.posX1 - SPEED && aux->bola.posX <= auxBloco->bloco.posX2 + SPEED) &&
                        ((aux->bola.posY >= auxBloco->bloco.posY2 && aux->bola.posY <= auxBloco->bloco.posY2 + SPEED) &&
                         (aux->bola.speedY < 0))) {
                        al_play_sample(hitBola, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                        if (auxBloco->bloco.item == true) {
                            auxBloco->bloco.descerItem = true;
                            (*qntBolasAdicionadas)++;
                        } else {
                            aux->bola.speedY *= -1;
                        }
                        if (auxBloco->bloco.vidas >= 1) auxBloco->bloco.vidas--;
                    }

                    if ((aux->bola.posY >= auxBloco->bloco.posY1 - SPEED && aux->bola.posY <= auxBloco->bloco.posY2 + SPEED) &&
                        (aux->bola.posX >= auxBloco->bloco.posX1 - SPEED && aux->bola.posX <= auxBloco->bloco.posX1) && (aux->bola.speedX > 0)) {
                        al_play_sample(hitBola, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                        if (auxBloco->bloco.item == true) {
                            auxBloco->bloco.descerItem = true;
                            (*qntBolasAdicionadas)++;
                        } else {
                            aux->bola.speedX *= -1;
                        }
                        if (auxBloco->bloco.vidas >= 1) auxBloco->bloco.vidas--;
                    }

                    if ((aux->bola.posY >= auxBloco->bloco.posY1 - SPEED && aux->bola.posY <= auxBloco->bloco.posY2 + SPEED) &&
                        ((aux->bola.posX >= auxBloco->bloco.posX2 && aux->bola.posX <= auxBloco->bloco.posX2 + SPEED) &&
                         (aux->bola.speedX < 0))) {
                        al_play_sample(hitBola, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                        if (auxBloco->bloco.item == true) {
                            auxBloco->bloco.descerItem = true;
                            (*qntBolasAdicionadas)++;
                        } else {
                            aux->bola.speedX *= -1;
                        }
                        if (auxBloco->bloco.vidas >= 1) auxBloco->bloco.vidas--;
                    }
                }

                auxBloco = auxBloco->proximo;
            }

            if ((aux->bola.posY > HEIGHT_LANCAMENTO)) {
                if (primeiraBolaMorta == false) {
                    posX_Ref = aux->bola.posX;
                    primeiraBolaMorta = true;
                    aux->bola.bolaReferencia = true;
                }

                aux->bola.speedY = 0;
                aux->bola.speedX = 0;
                aux->bola.shooted = false;
                aux->bola.viva = false;
                (*qntBolasMortas)++;

                if ((*qntBolasMortas) == (tamanhoDaListaBolas(listaBolas))) {
                    if ((*qntBolasAdicionadas) > 0)
                        for (int i = 0; i < (*qntBolasAdicionadas); i++) insereFinalDaListaBolas(listaBolas);

                    ElementoDaLista *auxiliar = listaBolas->ponteiroInicio;
                    while (auxiliar != NULL) {
                        auxiliar->bola.posX = posX_Ref;
                        auxiliar->bola.posY = HEIGHT_LANCAMENTO;
                        auxiliar = auxiliar->proximo;
                    }

                    posX_Ref = 0;
                    (*atirouBola) = false;
                    primeiraBolaMorta = false;
                    (*qntBolasMortas) = 0;
                    (*qntBolasAdicionadas) = 0;
                }
            }
        }
        aux = aux->proximo;
        auxBloco = listaBlocos->ponteiroInicio;
    }
}

void atiraBolas(ListaBolas *lista, double x_mouse, double y_mouse) {
    double dir;
    ElementoDaLista *aux = lista->ponteiroInicio;
    if (y_mouse < HEIGHT_LANCAMENTO + 2 * SPEED) {
        while (aux != NULL) {
            if (aux->bola.viva == false) {
                aux->bola.viva = true;
                dir = atan2((y_mouse - aux->bola.posY), (x_mouse - aux->bola.posX));
                aux->bola.speedX = SPEED * cos(dir);
                aux->bola.speedY = SPEED * sin(dir);
                // aux->bola.posY -= 20;
            }
            aux = aux->proximo;
        }
    }
}