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

static Palavra* buscar_palavra(Processador* p, const char* texto) {
    Palavra* atual = p->inicio;
    while (atual) {
        if (strcmp(atual->texto, texto) == 0) {
            return atual;
        }
        atual = atual->prox;
    }
    return NULL;
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

static void inserir_palavra(Processador* p, const char* texto, int idDoc) {
    Palavra* palavra = buscar_palavra(p, texto);
    if (palavra) {
        adicionar_ocorrencia(palavra, idDoc);
    } else {
        palavra = malloc(sizeof(Palavra));
        palavra->texto = strdup(texto);
        palavra->ocorrencias = NULL;
        palavra->prox = p->inicio;
        p->inicio = palavra;
        adicionar_ocorrencia(palavra, idDoc);
    }
}

void processar(Processador* p, ListaArquivos* lista) {
    if (!lista || !p) return;

    for (int i = 0; i < lista->quantidade; i++) {
        char* conteudo = lista->arquivos[i].conteudo;
        if (!conteudo) continue;

        char* palavra = strtok(conteudo, " \t\n\r");
        while (palavra) {
            normalizar_palavra(palavra);
            if (strlen(palavra) > 0) {
                inserir_palavra(p, palavra, i); // i é o id do documento
            }
            palavra = strtok(NULL, " \t\n\r");
        }
    }
}

void imprimir_processador(const Processador* p, ListaArquivos* lista) {
    const Palavra* atual = p->inicio;
    while (atual) {
        printf("Palavra: %s\n", atual->texto);
        Ocorrencia* o = atual->ocorrencias;
        while (o) {
            printf("  [%s] %dx\n", lista->arquivos[o->idDoc].nome, o->qtd);
            o = o->prox;
        }
        atual = atual->prox;
    }
}

void liberar_processador(Processador* p) {
    Palavra* atual = p->inicio;
    while (atual) {
        Palavra* temp = atual;
        Ocorrencia* o = atual->ocorrencias;
        while (o) {
            Ocorrencia* tempO = o;
            o = o->prox;
            free(tempO);
        }
        free(temp->texto);
        atual = atual->prox;
        free(temp);
    }
    free(p);
}
