#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leitor.h"

#define TAM_MAX 256
#define TAM_INICIAL 4096  // tamanho fixo inicial para o conteúdo
//_____________________________________________________________________________
ListaArquivos *ler_arq_entrada(const char *nome_entrada) {
    FILE *arquivo = fopen(nome_entrada, "r");
    if (!arquivo) {
        printf("Erro ao abrir arquivo de entrada\n");
        return NULL;
    }

    ListaArquivos *lista = malloc(sizeof(ListaArquivos));
    if (!lista) {
        printf("Erro de alocação para lista\n");
        fclose(arquivo);
        return NULL;
    }

    fscanf(arquivo, "%d\n", &lista->quantidade);
    lista->arquivos = malloc(lista->quantidade * sizeof(Arquivo));
    if (!lista->arquivos) {
        printf("Erro de alocação para arquivos\n");
        free(lista);
        fclose(arquivo);
        return NULL;
    }

    char linha[TAM_MAX];
    for (int i = 0; i < lista->quantidade; i++) {
        if (fgets(linha, sizeof(linha), arquivo) == NULL) {
            printf("Erro ao ler nome do arquivo na linha %d\n", i);
            lista->arquivos[i].nome = NULL;
            lista->arquivos[i].conteudo = NULL;
            continue;
        }

        //Remover a quebra de linha no final da string
        int len = 0;
        while (linha[len] != '\0' && linha[len] != '\n') len++;
        linha[len] = '\0';

        //aloca o nome manualmente no vetor de arquivos
        lista->arquivos[i].nome = malloc(len + 1);
        if (lista->arquivos[i].nome != NULL) {
            for (int j = 0; j <= len; j++) {
                lista->arquivos[i].nome[j] = linha[j];
            }
        }
        lista->arquivos[i].conteudo = NULL;
    }

    fclose(arquivo);
    return lista;
}
//_____________________________________________________________________________
void ler_conteudo_arquivos(ListaArquivos *lista) {
    if (lista == NULL){
         return;
    } 

    for (int i = 0; i < lista->quantidade; i++) {
        FILE *arquivo = fopen(lista->arquivos[i].nome, "r");
        if (!arquivo) {
            printf("Erro ao abrir %s\n", lista->arquivos[i].nome);
            const char *erro = "erro ao ler arquivo";
            int tam = 0;
            while (erro[tam] != '\0') tam++;
            lista->arquivos[i].conteudo = malloc(tam + 1);
            if (lista->arquivos[i].conteudo) {
                for (int j = 0; j <= tam; j++) {
                    lista->arquivos[i].conteudo[j] = erro[j];
                }
            }
            continue;
        }

        //Usa um buffer fixo grande o suficiente para a maioria dos arquivos
        char *buffer = malloc(TAM_INICIAL);
        if (!buffer) {
            printf("Erro de alocação para conteúdo\n");
            fclose(arquivo);
            continue;
        }

        int c, k = 0;
        while ((c = fgetc(arquivo)) != EOF && k < TAM_INICIAL - 1){ // Lê o arquivo caractere por caractere com fgetc ate encontrar o fim do arquivo ou atingir o tamanho máximo permitido do buffer (TAM_INICIAL - 1)
            buffer[k++] = c;
        }
        buffer[k] = '\0';// deixa espaço para adicionar o caractere nulo '\0' no final da string

        lista->arquivos[i].conteudo = buffer;
        fclose(arquivo);
    }
}
//________________________________________________________________________________
void printar_conteudo_arquivos(const ListaArquivos *lista) {
    if (!lista) return;

    for (int i = 0; i < lista->quantidade; i++) {
        printf("Arquivo: %s\n", lista->arquivos[i].nome ? lista->arquivos[i].nome : "sem nome"); //comparação da existencia ou nao de conteudo 
        printf("%s\n", lista->arquivos[i].conteudo ? lista->arquivos[i].conteudo : "sem texto");
        printf("_________________________________________________________________________________\n");
    }
}
//_____________________________________________________________________________
void liberar_lista(ListaArquivos *lista) {
    if(lista == NULL){
        return;
    }
    for (int i = 0; i < lista->quantidade; i++) {
        free(lista->arquivos[i].nome);
        free(lista->arquivos[i].conteudo);
    }
    free(lista->arquivos);
    free(lista);
}
