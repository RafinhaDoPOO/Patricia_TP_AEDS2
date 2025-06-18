#ifndef HASH_H
#define HASH_H

#define TAM_HASH 1429  // tamanho pensado para carga=0.7 (pode ser que eu aletre esse valor futuramente)

//struct que aramazena a quantidade de ocorrencias de cada palavra
typedef struct Ocorrencia {
    int idDoc;  //documento que a palavra aparece
    int qtde;   // quantas vezes 
    struct Ocorrencia *prox; // ponteiro para proxima ocorrencia -montando assim uma lista encadeada de ocorrencias-
} Ocorrencia;

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
void inserirOcorrencia(Ocorrencia **lista, int idDoc);
void inserirPalavra(HashTable *ht, char *palavra, int idDoc);
void imprimirIndiceHash(HashTable *ht);
void converterMinusculo(char *palavra);


#endif