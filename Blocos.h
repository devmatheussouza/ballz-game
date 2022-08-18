#include <stdbool.h>

typedef struct
{
   int linhaDoBloco;
   int vidas;
   int posX;
   int posY;
   int boundX;
   int boundY;
   bool criarBloco;
} Blocos;

typedef struct
{
   Blocos vetorDeBlocos;
} CounjuntoDeBlocos;

