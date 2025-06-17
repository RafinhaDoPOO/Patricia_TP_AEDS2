#ifndef PROCESSADOR_H
#define PROCESSADOR_H

#include "leitor.h"

typedef struct Ocorrencia {
    int idDoc;                 // ID do documento.
    int qtd;                   // Quantidade de vezes que a palavra apareceu.
    struct Ocorrencia* prox;   // Ponteiro para a próxima ocorrência da mesma palavra.
} Ocorrencia;

typedef struct Palavra {
    char* texto;               // A string da palavra.
    Ocorrencia* ocorrencias;   // Ponteiro para o início da lista de ocorrências.
} Palavra;



#endif
