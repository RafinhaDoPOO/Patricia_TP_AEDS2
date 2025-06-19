#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "processador.h"

// Normaliza uma palavra: remove caracteres não alfanuméricos e converte para minúsculas.
void normalizar_palavra(char* palavra) {
    if (!palavra) return;
    int i, j = 0;
    for (i = 0; palavra[i] != '\0'; i++) {
        if (isalnum((unsigned char)palavra[i])) {
            palavra[j++] = tolower((unsigned char)palavra[i]);
        }
    }
    palavra[j] = '\0';
}

// Adiciona ou atualiza uma ocorrência para uma palavra em um determinado documento.
void adicionar_ocorrencia(Palavra* palavra, int idDoc) {
    if (!palavra) return;

    Ocorrencia* atual = palavra->ocorrencias;
    while (atual) {
        if (atual->idDoc == idDoc) {
            atual->qtd++;
            return;
        }
        atual = atual->prox;
    }

    Ocorrencia* nova = malloc(sizeof(Ocorrencia));
    if (!nova) {
        perror("Falha ao alocar memoria para Ocorrencia");
        exit(EXIT_FAILURE);
    }
    nova->idDoc = idDoc;
    nova->qtd = 1;
    nova->peso = 0.0;
    nova->prox = palavra->ocorrencias;
    palavra->ocorrencias = nova;
}

// Calcula e armazena a relevância (TF-IDF) para cada ocorrência de uma palavra.
void calcular_relevancia(Palavra* palavra, ListaArquivos* lista) {
    if (!palavra || !lista || lista->quantidade == 0) return;

    int N = lista->quantidade; // CORRIGIDO: Usando 'quantidade' da struct
    
    int dj = 0;
    Ocorrencia* temp = palavra->ocorrencias;
    while (temp) {
        dj++;
        temp = temp->prox;
    }

    if (dj == 0) return;

    double idf = log2((double)N / dj);

    Ocorrencia* ocorrencia = palavra->ocorrencias;
    while (ocorrencia) {
        int fji = ocorrencia->qtd;
        ocorrencia->peso = (double)fji * idf; // Armazena o peso na struct
        ocorrencia = ocorrencia->prox;
    }
}

// Libera a memória da lista de ocorrências
void liberarOcorrenciasDaPalavra(Ocorrencia* ocorrencias) {
    Ocorrencia* atual = ocorrencias;
    while (atual != NULL) {
        Ocorrencia* proximo = atual->prox;
        free(atual);
        atual = proximo;
    }
}