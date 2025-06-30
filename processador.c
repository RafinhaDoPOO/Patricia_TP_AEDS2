#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "processador.h"

#include <ctype.h>
#include <string.h>
#include <stdio.h>

// Função: normaliza a palavra, transformando letras com acento em sem acento,
// cedilha vira c, e mantém apenas letras a–z em minúsculo.
void normalizar_palavra(char *palavra) {
    int i = 0, j = 0;
    size_t len = strlen(palavra);

    while (i < (int)len) {
        unsigned char c = (unsigned char) palavra[i];

        // Trata cedilha em UTF-8: ç = 0xC3 0xA7, Ç = 0xC3 0x87
        if ((unsigned char)palavra[i] == 0xC3 &&
            (i + 1 < (int)len) &&
            ((unsigned char)palavra[i + 1] == 0xA7 || (unsigned char)palavra[i + 1] == 0x87)) {
            palavra[j++] = 'c';
            i += 2;
            continue;
        }

        // Acentos mais comuns (ex: á é 0xC3 0xA1) — vamos mapear manualmente
        if ((unsigned char)palavra[i] == 0xC3 && (i + 1 < (int)len)) {
            unsigned char next = (unsigned char)palavra[i + 1];
            switch (next) {
                case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4:  // à á â ã ä
                case 0x80: case 0x81: case 0x82: case 0x83: case 0x84:  // À Á Â Ã Ä
                    palavra[j++] = 'a'; break;
                case 0xA8: case 0xA9: case 0xAA: case 0xAB:             // è é ê ë
                case 0x88: case 0x89: case 0x8A: case 0x8B:             // È É Ê Ë
                    palavra[j++] = 'e'; break;
                case 0xAC: case 0xAD: case 0xAE: case 0xAF:             // ì í î ï
                case 0x8C: case 0x8D: case 0x8E: case 0x8F:             // Ì Í Î Ï
                    palavra[j++] = 'i'; break;
                case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB6:  // ò ó ô õ ö
                case 0x92: case 0x93: case 0x94: case 0x95: case 0x96:  // Ò Ó Ô Õ Ö
                    palavra[j++] = 'o'; break;
                case 0xB9: case 0xBA: case 0xBB: case 0xBC:             // ù ú û ü
                case 0x99: case 0x9A: case 0x9B: case 0x9C:             // Ù Ú Û Ü
                    palavra[j++] = 'u'; break;
                default:
                    // não reconhecido, ignora o par
                    break;
            }
            i += 2;
            continue;
        }

        // ASCII normal (ex: letras, números, pontuação)
        char lower = (char) tolower(c);
        if (lower >= 'a' && lower <= 'z') {
            palavra[j++] = lower;
        }

        i++;
    }

    palavra[j] = '\0';

}


// Adiciona ou atualiza uma ocorrência para uma palavra em um determinado documento.
void adicionar_ocorrencia(Palavra* palavra, int idDoc) {
    if (!palavra) return;

    Ocorrencia* atual = palavra->ocorrencias;

    // Verifica se já existe ocorrência para o mesmo documento
    while (atual != NULL) {
        if (atual->idDoc == idDoc) {
            atual->qtd++;
            return;
        }
        atual = atual->prox;
    }

    // Se não existe, cria nova ocorrência
    Ocorrencia* nova = malloc(sizeof(Ocorrencia));
    if (!nova) {
        perror("Falha ao alocar memória para Ocorrencia");
        exit(EXIT_FAILURE);
    }
    nova->idDoc = idDoc;
    nova->qtd = 1;
    nova->peso = 0.0;
    nova->prox = NULL;

    // Insere na lista ordenada por idDoc
    Ocorrencia** ptr = &palavra->ocorrencias;
    while (*ptr != NULL && (*ptr)->idDoc < idDoc) {
        ptr = &(*ptr)->prox;
    }

    nova->prox = *ptr;
    *ptr = nova;
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
