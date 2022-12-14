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

#include "Bolas.h"
#include "libScore.h"

#define QNT_BLOCOS 9
#define HEIGHT_LANCAMENTO 800
#define RAIO 8

// Funcao para gerar numeros aleatorios
int numAleatorio(int n, int m) { return (rand() % (m - n + 1)) + n; }

ListaBlocos *criaListaBlocos() {
    ListaBlocos *lista = (ListaBlocos *)malloc(sizeof(ListaBlocos));
    if (lista != NULL) lista->ponteiroInicio = NULL;
    return lista;
}

void liberaListaBlocos(ListaBlocos *lista) {
    if (listaVaziaBlocos(lista)) free(lista);

    ElementoDaListaBloco *nodo_remocao;
    while (lista->ponteiroInicio != NULL) {
        nodo_remocao = lista->ponteiroInicio;
        lista->ponteiroInicio = lista->ponteiroInicio->proximo;
        free(nodo_remocao);
    }
    free(lista);
}

int tamanhoDaListaBlocos(ListaBlocos *lista) {
    if (lista == NULL) return 0;
    int cont = 0;
    ElementoDaListaBloco *no = lista->ponteiroInicio;
    while (no != NULL) {
        cont++;
        no = no->proximo;
    }
    return cont;
}

bool listaVaziaBlocos(ListaBlocos *lista) {
    if (lista->ponteiroInicio == NULL) return true;
    return false;
}

bool insereFinalDaListaBlocos(ListaBlocos *lista, Blocos bloco) {
    if (lista == NULL) return false;

    ElementoDaListaBloco *no = (ElementoDaListaBloco *)malloc(sizeof(ElementoDaListaBloco));
    if (no == NULL) return false;

    no->bloco = bloco;
    no->proximo = NULL;
    if (lista->ponteiroInicio == NULL) {
        no->anterior = NULL;
        lista->ponteiroInicio = no;
    } else {
        ElementoDaListaBloco *aux = lista->ponteiroInicio;
        while (aux->proximo != NULL) aux = aux->proximo;
        aux->proximo = no;
        no->anterior = aux;
    }

    return true;
}

// Funcao para preencher uma nova linha de blocos
void preencheLinhaBlocos(ListaBlocos *lista, int *score) {
    (*score)++;
    int probGerarBloco, qntDeVida, incrementoWidth = 88;
    Blocos bloco;
    int posicaoItem = numAleatorio(0, QNT_BLOCOS - 1);

    for (int i = 0; i < QNT_BLOCOS; i++) {
        probGerarBloco = numAleatorio(0, 100);
        if (i == posicaoItem) {
            bloco.vidas = 1;
            bloco.item = true;
        } else if (probGerarBloco <= 50) {
            qntDeVida = numAleatorio(1 * (*score), 2 * (*score));
            bloco.vidas = qntDeVida;
            bloco.item = false;
        }

        if (i == posicaoItem || probGerarBloco <= 50) {
            if (bloco.item == true) {
                bloco.posX1 = 35 + incrementoWidth * i;
                bloco.posY1 = 180;
                bloco.posX2 = 69 + incrementoWidth * i;
                bloco.posY2 = 224;
            } else {
                bloco.posX1 = 5 + incrementoWidth * i;
                bloco.posY1 = 160;
                bloco.posX2 = 89 + incrementoWidth * i;
                bloco.posY2 = 244;
            }
            bloco.linhaDoBloco = (*score);
            bloco.descerItem = false;
            bloco.blinkItem = 0;
            insereFinalDaListaBlocos(lista, bloco);
        }
    }
}

// Funcao de controle do display dos blocos
void drawBlocos(ListaBlocos *lista, ALLEGRO_FONT *fonts) {
    ALLEGRO_COLOR color;
    ElementoDaListaBloco *aux = lista->ponteiroInicio;
    while (aux != NULL) {
        // Bolinha verde
        if (aux->bloco.descerItem == true) {
            al_draw_filled_circle(aux->bloco.posX1 + 45, aux->bloco.posY1, 8, al_map_rgb(0, 200, 0));
            al_draw_text(fonts, al_map_rgb(255, 255, 255), aux->bloco.posX1 + 60, aux->bloco.posY1 - 30, 8, "+1");
        }

        // Controle da cor do bloco de acordo com a vida
        if ((aux->bloco.vidas > 0) && (aux->bloco.item == false)) {
            if (aux->bloco.vidas <= 5) {
                color = al_map_rgb(255, 182, 0);
            } else if (aux->bloco.vidas <= 10) {
                color = al_map_rgb(44, 138, 0);
            } else if (aux->bloco.vidas <= 15) {
                color = al_map_rgb(246, 95, 0);
            } else if (aux->bloco.vidas <= 25) {
                color = al_map_rgb(255, 15, 0);
            } else {
                color = al_map_rgb(255, 15, 142);
            }
            al_draw_filled_rectangle(aux->bloco.posX1, aux->bloco.posY1, aux->bloco.posX2, aux->bloco.posY2, color);
            al_draw_textf(fonts, al_map_rgb(0, 0, 0), aux->bloco.posX1 + 42, aux->bloco.posY1 + 30, ALLEGRO_ALIGN_CENTRE, "%d",
                          aux->bloco.vidas);
        }

        // Display dos itens
        if ((aux->bloco.vidas > 0) && (aux->bloco.item == true) && (aux->bloco.descerItem == false)) {
            // al_draw_filled_rectangle(aux->bloco.posX1, aux->bloco.posY1, aux->bloco.posX2, aux->bloco.posY2, al_map_rgb(255, 0, 255));

            if (aux->bloco.blinkItem <= 30) {
                al_draw_circle(aux->bloco.posX1 + 15, aux->bloco.posY1 + 22, 15, al_map_rgb(255, 255, 255), 3);
                al_draw_filled_circle(aux->bloco.posX1 + 15, aux->bloco.posY1 + 22, 10, al_map_rgb(255, 255, 255));
                aux->bloco.blinkItem++;
            }

            if (aux->bloco.blinkItem > 30 && aux->bloco.blinkItem <= 60) {
                al_draw_circle(aux->bloco.posX1 + 15, aux->bloco.posY1 + 22, 20, al_map_rgb(255, 255, 255), 3);
                al_draw_filled_circle(aux->bloco.posX1 + 15, aux->bloco.posY1 + 22, 10, al_map_rgb(255, 255, 255));
                aux->bloco.blinkItem++;
                if (aux->bloco.blinkItem == 60) aux->bloco.blinkItem = 0;
            }
        }

        aux = aux->proximo;
    }
}

// Funcao que controla a movimentacao dos blocos e dos itens
void updateBlocos(ListaBlocos *lista, bool *descerBlocos, bool *atirouBola, int *score, int *estadoAtual, ALLEGRO_SAMPLE *pontoScore,
                  ALLEGRO_SAMPLE *gameoverSound) {
    bool acabou = false;
    ElementoDaListaBloco *aux = lista->ponteiroInicio;
    while (aux != NULL) {
        // Controle da descida da bolinha verde
        if (aux->bloco.descerItem == true) {
            if (aux->bloco.posY1 + RAIO < HEIGHT_LANCAMENTO)
                aux->bloco.posY1 += 12;
            else if (aux->bloco.posY1 + RAIO > HEIGHT_LANCAMENTO)
                aux->bloco.posY1 -= 1;
            else if ((*atirouBola) == false)
                aux->bloco.descerItem = false;
        }
        aux = aux->proximo;
    }

    // Descida de linhas de bloco
    if (((*descerBlocos) == true) && ((*atirouBola) == false)) {
        aux = lista->ponteiroInicio;
        for (int i = 1; i <= (*score) && acabou == false; i++) {
            while ((aux != NULL) && (aux->bloco.linhaDoBloco == i)) {
                aux->bloco.posY1 += 88;
                aux->bloco.posY2 += 88;
                if (aux->bloco.posY2 >= HEIGHT_LANCAMENTO - 80 && aux->bloco.vidas > 0) {
                    *estadoAtual = 3;
                    salvaScore(*score);
                    acabou = true;
                    al_play_sample(gameoverSound, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                    break;
                }
                aux = aux->proximo;
            }
        }
        if (acabou == false) {
            al_play_sample(pontoScore, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
            preencheLinhaBlocos(lista, score);
        }
        (*descerBlocos) = false;
    }
}