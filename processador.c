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




