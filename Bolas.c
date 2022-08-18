#include <stdio.h>
#include <stdlib.h>
#include "Bolas.h"

Lista *criaLista()
{
   Lista *lista = (Lista *)malloc(sizeof(Lista));
   if (lista != NULL)
      lista->ponteiroInicio = NULL;
   return lista;
}

void liberaLista(Lista *lista)
{
   if (listaVazia(lista))
      free(lista);

   ElementoDaLista *nodo_remocao;
   while (lista != NULL)
   {
      nodo_remocao = lista->ponteiroInicio;
      lista->ponteiroInicio = lista->ponteiroInicio->proximo;
      free(nodo_remocao);
   }
   free(lista);
}

int tamanhoDaLista(Lista *lista)
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

bool listaVazia(Lista *lista)
{
   if (lista->ponteiroInicio == NULL)
      return true;
   return false;
}

bool insereFinalDaLista(Lista *lista, Bola bola)
{
   if (lista == NULL)
      return false;

   ElementoDaLista *no = (ElementoDaLista *)malloc(sizeof(ElementoDaLista));
   if (no == NULL)
      return false;

   no->bola = bola;
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