#include <stdbool.h>
#include <stdio.h>

typedef struct {
    int score;
    int dia, mes, ano;
} Score;

void salvaScore(int score);
Score* leituraScores(int* qntScores);
int quantidadeDeScoresNoArquivo(FILE* arquivo);
void ordenacaoVetorScores(Score* vetScores, int tamanho);
bool verificaExistenciaArquivo();