#include "libScore.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>
#define LINE_SIZE 1024

bool verificaExistenciaArquivo() {
    FILE* arqScores;
    arqScores = fopen("scores.txt", "r");
    if (!arqScores) return false;
    return true;
}

void salvaScore(int score) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char line[LINE_SIZE];
    FILE* arqScores;
    arqScores = fopen("scores.txt", "a+");

    if (fgets(line, LINE_SIZE, arqScores) == NULL) {
        fprintf(arqScores, "*****************\n");
        fprintf(arqScores, "SCORES BALLZ GAME\n");
        fprintf(arqScores, "*****************\n\n");
    }

    fprintf(arqScores, "Score: %d\n", score);
    fprintf(arqScores, "Date: %d/%d/%d\n\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    fclose(arqScores);
}

Score* leituraScores(int* numScores) {
    int qntScores;
    FILE* arqScores;
    arqScores = fopen("scores.txt", "r");
    char line[LINE_SIZE];
    char* ptr_string;

    if (!arqScores) {
        printf("Nao foi possivel abrir esse arquivo\n");
        exit(1);
    }

    qntScores = quantidadeDeScoresNoArquivo(arqScores);
    // printf("QNT SCORES: %d\n", qntScores);
    int cont = qntScores - 1;
    fseek(arqScores, 0, SEEK_SET);

    Score* vetScores = malloc(sizeof(Score) * qntScores);
    while (fgets(line, LINE_SIZE, arqScores) != NULL) {
        ptr_string = strtok(line, ":");

        if (strcmp(ptr_string, "Score") == 0) {
            ptr_string = strtok(NULL, " ");
            vetScores[cont].score = atoi(ptr_string);
            continue;
        }

        if (strcmp(ptr_string, "Date") == 0) {
            ptr_string = strtok(NULL, " ");
            sscanf(ptr_string, "%d/%d/%d", &vetScores[cont].dia, &vetScores[cont].mes, &vetScores[cont].ano);
            cont--;
            continue;
        }
    }

    ordenacaoVetorScores(vetScores, qntScores);

    fclose(arqScores);

    *numScores = qntScores;
    return vetScores;
}

int quantidadeDeScoresNoArquivo(FILE* arqScores) {
    int qntScores = 0;
    char line[LINE_SIZE];
    char* ptr_string;
    while (fgets(line, LINE_SIZE, arqScores) != NULL) {
        ptr_string = strtok(line, ":");

        if (strcmp(ptr_string, "Score") == 0) {
            qntScores++;
            continue;
        }
    }
    return qntScores;
}

void ordenacaoVetorScores(Score* vetScores, int tamanho) {
    int i, j, indexMenor;
    Score aux;
    for (i = 0; i < tamanho - 1; i++) {
        indexMenor = i;
        for (j = i + 1; j < tamanho; j++) {
            if (vetScores[j].score > vetScores[indexMenor].score) indexMenor = j;
        }
        if (i != indexMenor) {
            aux = vetScores[indexMenor];
            vetScores[indexMenor] = vetScores[i];
            vetScores[i] = aux;
        }
    }
}