#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/hash.h"
#include "processador.h"


int funcaoHash(char *palavra) {   //devolve o indice que a palavra sera armazenada na tabela hash
    unsigned long hash = 0;  // acumula o valor calculado a partir dos caracteres da palavra 
    int c;   // variavel para guradar o valor do caracter atual

    while ((c = *palavra++)) { // enquanto houver caracteres na palavra
        hash = hash * 31 + c; //gera valores diferentes  para evitar colisões ("esplaha")
    }

    return hash % TAM_HASH;
}


void inserirOcorrencia(Ocorrencia **lista, int idDoc) { // recebe o ponteiro para a lista de ocorrencias e o id do documento onde a palavra aparece
    Ocorrencia *aux = *lista; //aponta para o primeiro nó da lista de ocorrências.

    while (aux != NULL) {
        if (aux->idDoc == idDoc) { // se a palavra ja apareceu naquele documento, aumenta a quantidade
            aux->qtde++;
            return;
        }
        aux = aux->prox; // avança para o próximo nó da lista caso caso "idDoc" não foi encontrado
    }

    // Se não encontrou, cria nova ocorrência
    Ocorrencia *nova = malloc(sizeof(Ocorrencia)); // criando novo no na lista de ocorrências
    nova->idDoc = idDoc;
    nova->qtde = 1;
    nova->prox = *lista;
    *lista = nova;
}


void inserirPalavra(HashTable *ht, char *palavra, int idDoc) { //insere palavra na tabela hash

    normalizar_palavra(char* palavra)

    int indice = funcaoHash(palavra); // ve em qual posiçao da tabela a palvara deve ser inserida
    HashItem *atual = ht->tabela[indice]; //percorre a lista de intens na posiçao encontrada 

    while (atual != NULL) { // verifica se a palavra ja existe 
        if (strcmp(atual->palavra, palavra) == 0) { //se a palavra ja esta na tabela \/
            inserirOcorrencia(&(atual->listaOcorrencias), idDoc);  //                atualizar ou criar a ocorrência correspondente ao idDoc
            return;
        }
        atual = atual->prox; // se nao encontrou, avança para o próximo item da lista
    }

    // Palavra não encontrada ->cria novo HashItem
    HashItem *novo = malloc(sizeof(HashItem));
    novo->palavra = strdup(palavra); // copia a string
    novo->listaOcorrencias = NULL; // inicializa a lista de ocorrências como vazia
    novo->prox = ht->tabela[indice]; //Insere o novo item no início da lista no índice correspondente da tabela
    ht->tabela[indice] = novo;

    inserirOcorrencia(&(novo->listaOcorrencias), idDoc); //registra a primeira ocorrencia
}

int compararPalavras(const void *a, const void *b) { //funçao auxiliar para ordenar
    EntradaOrdenada *ea = (EntradaOrdenada *)a;
    EntradaOrdenada *eb = (EntradaOrdenada *)b;
    return strcmp(ea->palavra, eb->palavra);
}

void imprimirIndiceHash(HashTable *ht) {
    // 1: conta quantas palavras existem
    int total = 0;
    for (int i = 0; i < TAM_HASH; i++) {
        HashItem *item = ht->tabela[i];
        while (item != NULL) {
            total++;
            item = item->prox;
        }
    }

    // 2: alocar vetor auxiliar
    EntradaOrdenada *lista = malloc(total * sizeof(EntradaOrdenada));

    if (lista == NULL) {
    printf("Erro de alocação de memória\n");
    exit(1);
    }

    int idx = 0;

    // Passo 3: preencher vetor
    for (int i = 0; i < TAM_HASH; i++) {
        HashItem *item = ht->tabela[i];
        while (item != NULL) {
            lista[idx].palavra = item->palavra;
            lista[idx].ocorrencias = item->listaOcorrencias;
            idx++;
            item = item->prox;
        }
    }

    // Passo 4: ordenar
    qsort(lista, total, sizeof(EntradaOrdenada), compararPalavras);

    // Passo 5: imprimir
    for (int i = 0; i < total; i++) {
        printf("%s: ", lista[i].palavra);
        Ocorrencia *oc = lista[i].ocorrencias;
        while (oc != NULL) {
            printf("<%d, %d> ", oc->qtde, oc->idDoc);
            oc = oc->prox;
        }
        printf("\n");
    }

    free(lista);
}


