#ifndef PROCESSADOR_H
#define PROCESSADOR_H

#include "leitor.h"

//indice invertido
typedef struct Ocorrencia {
    int idDoc;
    int qtd;
    struct Ocorrencia* prox;
} Ocorrencia;

// estrutura que eu usei 
typedef struct Palavra {
    char* texto;
    Ocorrencia* ocorrencias;
    struct Palavra* prox;
} Palavra;

typedef struct {
    Palavra* inicio;
} Processador;

void processar(Processador* p, ListaArquivos* lista);
void imprimir_processador(const Processador* p, ListaArquivos* lista);
void liberar_processador(Processador* p);

#endif
