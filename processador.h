#ifndef PROCESSADOR_H
#define PROCESSADOR_H

#include "leitor.h"

typedef struct Ocorrencia {
    int idDoc;
    int qtd;
    double peso; // Adicionei um campo para o peso TF-IDF
    struct Ocorrencia* prox;
} Ocorrencia;

typedef struct Palavra {
    char* texto;
    Ocorrencia* ocorrencias;
} Palavra;

// Protótipos de todas as funções do processador
void normalizar_palavra(char* palavra);
void adicionar_ocorrencia(Palavra* palavra, int idDoc);
void calcular_relevancia(Palavra* palavra, ListaArquivos* lista);
void liberarOcorrenciasDaPalavra(Ocorrencia* ocorrencias);

#endif