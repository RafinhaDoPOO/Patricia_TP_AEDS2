#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "leitor.h"
#include "processador.h"

static void normalizar_palavra(char* palavra) {
    int i, j = 0;
    for (i = 0; palavra[i] != '\0'; i++) {
        if (isalnum((unsigned char)palavra[i])) {
            palavra[j++] = tolower((unsigned char)palavra[i]);
        }
    }
    palavra[j] = '\0';
}


static void adicionar_ocorrencia(Palavra* palavra, int idDoc) {
    Ocorrencia* atual = palavra->ocorrencias;
    while (atual) {
        if (atual->idDoc == idDoc) {
            atual->qtd++;
            return;
        }
        atual = atual->prox;
    }

    // Se não encontrou, adiciona nova ocorrência
    Ocorrencia* nova = malloc(sizeof(Ocorrencia));
    nova->idDoc = idDoc;
    nova->qtd = 1;
    nova->prox = palavra->ocorrencias;
    palavra->ocorrencias = nova;
}

#include <math.h>

void calcular_relevancia(Palavra* palavra, ListaArquivos* lista) {
    Ocorrencia* ocorrencia = palavra->ocorrencias;
    int N = lista->n;

    // Conta em quantos documentos a palavra aparece (d_j)
    int dj = 0;
    Ocorrencia* temp = palavra->ocorrencias;
    while (temp) {
        dj++;
        temp = temp->prox;
    }

    // Agora calcula w_j^i para cada documento e imprime ou armazena como quiser
    ocorrencia = palavra->ocorrencias;
    while (ocorrencia) {
        int fji = ocorrencia->qtd;    // frequência da palavra no documento i
        int idDoc = ocorrencia->idDoc;

        // Calcula log2(N/dj)
        double idf = log2((double)N / dj);
        double peso = fji * idf;

        printf("Doc %d | Palavra '%s' | fji=%d | dj=%d | N=%d | peso=%.4lf\n",
               idDoc, palavra->texto, fji, dj, N, peso);

        // Aqui você pode armazenar o peso no seu documento ou vetor de relevância, se desejar

        ocorrencia = ocorrencia->prox;
    }
}

//calculo de relevancia

#include <math.h>

void calcular_relevancia(Palavra* palavra, ListaArquivos* lista) {
    Ocorrencia* ocorrencia = palavra->ocorrencias;
    int N = lista->n;

    // Conta em quantos documentos a palavra aparece (d_j)
    int dj = 0;
    Ocorrencia* temp = palavra->ocorrencias;
    while (temp) {
        dj++;
        temp = temp->prox;
    }

    // Agora calcula w_j^i para cada documento e imprime ou armazena como quiser
    ocorrencia = palavra->ocorrencias;
    while (ocorrencia) {
        int fji = ocorrencia->qtd;       // frequência da palavra no documento i
        int idDoc = ocorrencia->idDoc;

        // Calcula log2(N/dj)
        double idf = log2((double)N / dj);
        double peso = fji * idf;

        printf("Doc %d | Palavra '%s' | fji=%d | dj=%d | N=%d | peso=%.4lf\n",
               idDoc, palavra->texto, fji, dj, N, peso);


        ocorrencia = ocorrencia->prox;
    }
}




