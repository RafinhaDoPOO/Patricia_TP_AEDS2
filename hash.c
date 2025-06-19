#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hash.h"
#include "processador.h"

int funcaoHash(char *palavra) {   //devolve o indice que a palavra sera armazenada na tabela hash
    unsigned long hash = 0;  // acumula o valor calculado a partir dos caracteres da palavra 
    int c;   // variavel para guradar o valor do caracter atual

    while ((c = *palavra++)) { // enquanto houver caracteres na palavra
        hash = hash * 31 + c; //gera valores diferentes  para evitar colisões ("esplaha")
    }

    return hash % TAM_HASH;
}

void construir_indice_hash(HashTable* ht, ListaArquivos* lista) {
    if (!ht || !lista) return;

    for (int i = 0; i < lista->quantidade; i++) {
        const char* conteudo = lista->arquivos[i].conteudo;

        if (!conteudo || strcmp(conteudo, "erro ao ler arquivo") == 0) {
            fprintf(stderr, "Aviso: Pulando arquivo %s por erro de leitura.\n", lista->arquivos[i].nome);
            continue;
        }

        char* conteudo_copia = strdup(conteudo);
        if (!conteudo_copia) {
            fprintf(stderr, "Erro de alocação de memória para cópia do conteúdo.\n");
            continue;
        }

        char* palavra_token = strtok(conteudo_copia, " \t\n\r.,;:!?()[]{}<>");

        while (palavra_token != NULL) {
            normalizar_palavra(palavra_token);

            if (strlen(palavra_token) > 0) {
                inserirPalavra(ht, palavra_token, i);
            }

            palavra_token = strtok(NULL, " \t\n\r.,;:!?()[]{}<>");
        }

        free(conteudo_copia);
    }
}


// Coloque esta função corrigida em seu arquivo hash.c

void inserirPalavra(HashTable *ht, char *palavra, int idDoc) {
    // A normalização não é necessária aqui se já for feita antes, 
    // mas vamos manter para garantir.
    // normalizar_palavra(palavra); // A sua função construir_indice_hash já faz isso.

    int indice = funcaoHash(palavra);
    HashItem *item_atual = ht->tabela[indice];

    // 1. Procura se a palavra já existe na tabela hash
    while (item_atual != NULL) {
        if (strcmp(item_atual->palavra, palavra) == 0) {
            // A PALAVRA JÁ EXISTE. Agora, atualize as ocorrências.
            Ocorrencia *ocorrencia_atual = item_atual->listaOcorrencias;
            while (ocorrencia_atual != NULL) {
                if (ocorrencia_atual->idDoc == idDoc) {
                    // Ocorrência para este documento já existe, só incrementa a quantidade.
                    ocorrencia_atual->qtd++;
                    return; // Termina a função
                }
                ocorrencia_atual = ocorrencia_atual->prox;
            }

            // Se saiu do loop, é porque a palavra existe, mas não neste documento.
            // Cria uma nova ocorrência para este idDoc.
            Ocorrencia *nova_ocorrencia = malloc(sizeof(Ocorrencia));
            nova_ocorrencia->idDoc = idDoc;
            nova_ocorrencia->qtd = 1;
            nova_ocorrencia->peso = 0.0; // O peso é calculado depois
            nova_ocorrencia->prox = item_atual->listaOcorrencias; // Adiciona no início da lista
            item_atual->listaOcorrencias = nova_ocorrencia;
            return; // Termina a função
        }
        item_atual = item_atual->prox;
    }

    // 2. Se a palavra não foi encontrada, cria um novo HashItem
    HashItem *novo_item = malloc(sizeof(HashItem));
    novo_item->palavra = strdup(palavra);
    novo_item->listaOcorrencias = NULL; // A lista começa vazia

    // Cria a primeira ocorrência para este novo item
    Ocorrencia *primeira_ocorrencia = malloc(sizeof(Ocorrencia));
    primeira_ocorrencia->idDoc = idDoc;
    primeira_ocorrencia->qtd = 1;
    primeira_ocorrencia->peso = 0.0;
    primeira_ocorrencia->prox = NULL; // É o único na lista por enquanto

    // Atribui a lista de ocorrências ao novo item
    novo_item->listaOcorrencias = primeira_ocorrencia;

    // Insere o novo item no início da lista de colisão da tabela hash
    novo_item->prox = ht->tabela[indice];
    ht->tabela[indice] = novo_item;
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
            printf("<%d, %d> ", oc->qtd, oc->idDoc);
            oc = oc->prox;
        }
        printf("\n");
    }

    free(lista);
}


Palavra* buscar_palavra_hash(HashTable* ht, const char* palavra) {
    if (!ht || !palavra) return NULL;

    int indice = funcaoHash((char*)palavra);
    HashItem* item = ht->tabela[indice];

    while (item != NULL) {
        if (strcmp(item->palavra, palavra) == 0) {
            // Converte listaOcorrencias (tipo Ocorrencia*) para Palavra (simula)
            // Como seu HashItem não tem Palavra*, mas só palavra + ocorrencias,
            // vamos construir uma estrutura Palavra temporária para compatibilidade:
            Palavra* p = malloc(sizeof(Palavra));
            p->texto = item->palavra;
            p->ocorrencias = item->listaOcorrencias;
            return p;
        }
        item = item->prox;
    }
    return NULL;
}



