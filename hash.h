#ifndef HASH_H
#define HASH_H
#define TAM_HASH 1429  // tamanho pensado para carga=0.7 (pode ser que eu altere esse valor futuramente)
#include "processador.h"

int comparacoes_hash_insercao ;
int comparacoes_hash_busca ; 

typedef struct {
    char *palavra;
    Ocorrencia *ocorrencias;
} EntradaOrdenada;


typedef struct HashItem {
    char *palavra; //chave principal
    Ocorrencia *listaOcorrencias; // lista encadeada de onde essas palavras aparecem
    struct HashItem *prox; //encadear os itens das colisoes 
} HashItem;

typedef struct {
    HashItem *tabela[TAM_HASH]; //cada posiçao pode ter uma lista encadeada
} HashTable;

int funcaoHash(char *palavra);
void inserirPalavra(HashTable *ht, char *palavra, int idDoc);
void imprimirIndiceHash(HashTable *ht);
void construir_indice_hash(HashTable* ht, ListaArquivos* lista);
void imprimirHash_ordemInsercao(HashTable *ht);
Palavra* buscar_palavra_hash(HashTable* ht, const char* palavra);

#endif
