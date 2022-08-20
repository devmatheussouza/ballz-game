#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "Bolas.h"
#include "Blocos.h"

#define QNT_BLOCOS 11

int numAleatorio(int n, int m)
{
    return (rand() % (m - n + 1)) + n;
}

ListaBlocos *criaListaBlocos()
{
    ListaBlocos *lista = (ListaBlocos *)malloc(sizeof(ListaBlocos));
    if (lista != NULL)
        lista->ponteiroInicio = NULL;
    return lista;
}

void liberaListaBlocos(ListaBlocos *lista)
{
    if (listaVaziaBlocos(lista))
        free(lista);

    ElementoDaListaBloco *nodo_remocao;
    while (lista->ponteiroInicio != NULL)
    {
        nodo_remocao = lista->ponteiroInicio;
        lista->ponteiroInicio = lista->ponteiroInicio->proximo;
        free(nodo_remocao);
    }
    free(lista);
}

int tamanhoDaListaBlocos(ListaBlocos *lista)
{
    if (lista == NULL)
        return 0;
    int cont = 0;
    ElementoDaListaBloco *no = lista->ponteiroInicio;
    while (no != NULL)
    {
        cont++;
        no = no->proximo;
    }
    return cont;
}

bool listaVaziaBlocos(ListaBlocos *lista)
{
    if (lista->ponteiroInicio == NULL)
        return true;
    return false;
}

bool insereFinalDaListaBlocos(ListaBlocos *lista, Blocos bloco)
{
    if (lista == NULL)
        return false;

    ElementoDaListaBloco *no = (ElementoDaListaBloco *)malloc(sizeof(ElementoDaListaBloco));
    if (no == NULL)
        return false;

    no->bloco = bloco;
    no->proximo = NULL;
    if (lista->ponteiroInicio == NULL)
    {
        no->anterior = NULL;
        lista->ponteiroInicio = no;
    }
    else
    {
        ElementoDaListaBloco *aux = lista->ponteiroInicio;
        while (aux->proximo != NULL)
            aux = aux->proximo;
        aux->proximo = no;
        no->anterior = aux;
    }

    return true;
}

void preencheLinhaBlocos(ListaBlocos *lista, int *numeroDaLinha)
{
    (*numeroDaLinha)++;
    int probGerarBloco;
    int qntDeVida;
    int incrementoWidth = 72;
    Blocos bloco;

    for (int i = 0; i < QNT_BLOCOS; i++)
    {
        probGerarBloco = numAleatorio(0, 100);
        if (probGerarBloco <= 70)
        {
            qntDeVida = numAleatorio(1 * (*numeroDaLinha), 3 * (*numeroDaLinha));
            bloco.vidas = qntDeVida;
            bloco.posX1 = 10 + incrementoWidth * i;
            bloco.posY1 = 60;
            bloco.posX2 = 70 + incrementoWidth * i;
            bloco.posY2 = 120;
            bloco.boundX = 10;
            bloco.boundY = 10;
            bloco.linhaDoBloco = (*numeroDaLinha);
            insereFinalDaListaBlocos(lista, bloco);
        }
    }
}

void drawBlocos(ListaBlocos *lista, ALLEGRO_FONT *font)
{
    ALLEGRO_COLOR color;
    ElementoDaListaBloco *aux = lista->ponteiroInicio;
    while (aux != NULL)
    {
        if (aux->bloco.vidas > 0)
        {
            if (aux->bloco.vidas <= 5)
            {
                color = al_map_rgb(255, 182, 0);
            }
            else if (aux->bloco.vidas <= 10)
            {
                color = al_map_rgb(44, 138, 0);
            }
            else if (aux->bloco.vidas <= 15)
            {
                color = al_map_rgb(246, 95, 0);
            }
            else if (aux->bloco.vidas <= 25)
            {
                color = al_map_rgb(255, 15, 0);
            }
            else
            {
                color = al_map_rgb(255, 15, 142);
            }
            al_draw_filled_rectangle(aux->bloco.posX1, aux->bloco.posY1, aux->bloco.posX2, aux->bloco.posY2, color);
            al_draw_textf(font, al_map_rgb(0, 0, 0), aux->bloco.posX1 + 30, aux->bloco.posY1 + 20, ALLEGRO_ALIGN_CENTRE, "%d", aux->bloco.vidas);
        }
        aux = aux->proximo;
    }
}

void updateBlocos(ListaBlocos *lista, bool *descerBlocos, bool *atirouBola, int *numeroDaLinha)
{
    if (((*descerBlocos) == true) && ((*atirouBola) == false))
    {
        ElementoDaListaBloco *aux = lista->ponteiroInicio;
        for (int i = 1; i <= (*numeroDaLinha); i++)
        {
            while ((aux != NULL) && (aux->bloco.linhaDoBloco == i))
            {
                aux->bloco.posY1 += 70;
                aux->bloco.posY2 += 70;
                aux = aux->proximo;
            }
        }
        preencheLinhaBlocos(lista, numeroDaLinha);
        (*descerBlocos) = false;
    }
}