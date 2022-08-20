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

#define RES_WIDTH 800
#define RES_HEIGHT 960
#define RAIO 8
#define SPEED 8.0

ListaBolas *criaListaBolas()
{
   ListaBolas *lista = (ListaBolas *)malloc(sizeof(ListaBolas));
   if (lista != NULL)
      lista->ponteiroInicio = NULL;
   return lista;
}

void liberaListaBolas(ListaBolas *lista)
{
   if (listaVaziaBolas(lista))
      free(lista);

   ElementoDaLista *nodo_remocao;
   while (lista->ponteiroInicio != NULL)
   {
      nodo_remocao = lista->ponteiroInicio;
      lista->ponteiroInicio = lista->ponteiroInicio->proximo;
      free(nodo_remocao);
   }
   free(lista);
}

int tamanhoDaListaBolas(ListaBolas *lista)
{
   if (lista == NULL)
      return 0;
   int cont = 0;
   ElementoDaLista *no = lista->ponteiroInicio;
   while (no != NULL)
   {
      cont++;
      no = no->proximo;
   }
   return cont;
}

bool listaVaziaBolas(ListaBolas *lista)
{
   if (lista->ponteiroInicio == NULL)
      return true;
   return false;
}

bool insereFinalDaListaBolas(ListaBolas *lista)
{
   if (lista == NULL)
      return false;

   ElementoDaLista *no = (ElementoDaLista *)malloc(sizeof(ElementoDaLista));
   if (no == NULL)
      return false;

   no->bola = criaBola();
   no->proximo = NULL;
   if (lista->ponteiroInicio == NULL)
   {
      no->anterior = NULL;
      lista->ponteiroInicio = no;
   }
   else
   {
      ElementoDaLista *aux = lista->ponteiroInicio;
      while (aux->proximo != NULL)
         aux = aux->proximo;
      aux->proximo = no;
      no->anterior = aux;
   }

   return true;
}

Bola criaBola()
{
   Bola novaBola;
   novaBola.viva = false;
   novaBola.posX = RES_WIDTH / 2;
   novaBola.posY = RES_HEIGHT - 2 * RAIO;
   novaBola.boundX = RAIO + 2;
   novaBola.boundY = RAIO + 2;
   novaBola.speedX = -SPEED;
   novaBola.speedY = SPEED;
   novaBola.bolaReferencia = false;
   novaBola.shooted = false;
   return novaBola;
}

void drawBolas(ListaBolas *lista)
{
   ElementoDaLista *aux = lista->ponteiroInicio;
   while (aux != NULL)
   {
      if (aux->bola.viva == true)
         al_draw_filled_circle(aux->bola.posX, aux->bola.posY, RAIO, al_map_rgba(255, 255, 255, 255));
      aux = aux->proximo;
   }
}

void updateBolas(ListaBolas *lista)
{
   ElementoDaLista *aux = lista->ponteiroInicio;
   while (aux != NULL)
   {
      if (aux->bola.viva == true)
      {
         if (aux->anterior == NULL)
         {
            aux->bola.posX += aux->bola.speedX;
            aux->bola.posY += aux->bola.speedY;
         }
         else
         {
            if ((abs(aux->bola.posX - aux->anterior->bola.posX) > 40 ||
                 abs(aux->bola.posY - aux->anterior->bola.posY) > 40) &&
                aux->bola.shooted == false)
            {
               aux->bola.shooted = true;
            }

            if (aux->bola.shooted == true)
            {

               aux->bola.posX += aux->bola.speedX;
               aux->bola.posY += aux->bola.speedY;
            }
         }
      }
      aux = aux->proximo;
   }
}

void colisaoBolas(ListaBolas *lista, bool *atirouBola)
{
   bool flagBolasTodasMortas = false;
   ElementoDaLista *aux = lista->ponteiroInicio;
   while (aux != NULL)
   {
      if (aux->bola.viva == true)
      {
         if ((aux->bola.posX > RES_WIDTH - 2 * RAIO) || (aux->bola.posX < 5))
         {
            aux->bola.speedX *= -1;
         }

         if ((aux->bola.posY < 5))
         {
            aux->bola.speedY *= -1;
         }

         if ((aux->bola.posY > RES_HEIGHT - 2 * RAIO))
         {
            aux->bola.speedY = 0;
            aux->bola.speedX = 0;
            aux->bola.viva = false;
            aux->bola.shooted = false;
            // aux->bola.speedY *= -1;
            flagBolasTodasMortas = true;
         }

         if ((aux->proximo == NULL) && (aux->bola.shooted == false) && (aux->bola.speedX == 0) && (flagBolasTodasMortas == true))
         {
            (*atirouBola) = false;
         }
      }
      aux = aux->proximo;
   }
}

void atiraBolas(ListaBolas *lista, double x_mouse, double y_mouse)
{
   double dir;
   ElementoDaLista *aux = lista->ponteiroInicio;
   while (aux != NULL)
   {
      if (aux->bola.viva == false)
      {
         aux->bola.viva = true;
         dir = atan2((y_mouse - aux->bola.posY), (x_mouse - aux->bola.posX));
         aux->bola.speedX = SPEED * cos(dir);
         aux->bola.speedY = SPEED * sin(dir);
         aux->bola.posY -= 20;
      }
      aux = aux->proximo;
   }
}