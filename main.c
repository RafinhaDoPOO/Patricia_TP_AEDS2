#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leitor.h"
#include "patricia.h"
#include "processador.h"
#include "hash.h"
#include <ctype.h>
#include <math.h>

//funcoes auxiliares

void calcular_relevancia_patricia(ArvorePat no, ListaArquivos* lista) {
    if (no == NULL) return;
    if (no->nt == externo) {
        calcular_relevancia(no->No.NoExterno.palavra, lista);
    } else {
        calcular_relevancia_patricia(no->No.noInterno.esq, lista);
        calcular_relevancia_patricia(no->No.noInterno.dir, lista);
    }
}

void calcular_relevancia_hash(HashTable* ht, ListaArquivos* lista) {
    if (!ht || !lista) return;

    for (int i = 0; i < TAM_HASH; i++) {
        HashItem* item = ht->tabela[i];
        while (item) {
            Palavra temp_palavra;
            temp_palavra.texto = item->palavra;
            temp_palavra.ocorrencias = item->listaOcorrencias;

            calcular_relevancia(&temp_palavra, lista);

            item = item->prox;
        }
    }
}



int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("ERRO: Forneca o arquivo de entrada como argumento.\n");
        printf("Uso: %s entrada.txt\n", argv[0]);
        return 1;
    }

    printf("1. Lendo arquivos de '%s'...\n", argv[1]);
    ListaArquivos* lista = ler_arq_entrada(argv[1]);
    if (!lista) return 1;
    ler_conteudo_arquivos(lista);

    // Construção Patricia
    printf("2. Construindo o indice Patricia...\n");
    ArvorePat indice_patricia = construir_indice_patricia(lista);
    if (!indice_patricia) {
        liberar_lista(lista);
        return 1;
    }
    printf("3. Calculando relevancia (TF-IDF) para todo o indice Patricia...\n");
    calcular_relevancia_patricia(indice_patricia, lista);

    // Construção Hash
    printf("4. Construindo o indice Hash...\n");
    HashTable ht;
    for (int i = 0; i < TAM_HASH; i++) {
        ht.tabela[i] = NULL;  // inicializa tabela hash
    }
    construir_indice_hash(&ht, lista);
    printf("5. Calculando relevancia (TF-IDF) para o indice Hash...\n");
    calcular_relevancia_hash(&ht, lista);

    int opcao;
    char palavra_busca[256];

    do {
        printf("\n========== MENU ==========\n");
        printf("1. Imprimir indice Patricia completo\n");
        printf("2. Imprimir indice Hash completo\n");
        printf("3. Buscar palavra na Patricia\n");
        printf("4. Buscar palavra na Hash\n");
        printf("5. Sair\n");
        printf("==========================\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            while (getchar() != '\n');
            opcao = 0;
        }
        while (getchar() != '\n');

        switch (opcao) {
            case 1:
                printf("\n--- Indice Patricia ---\n");
                imprimir_patricia(indice_patricia, lista);
                break;

            case 2:
                printf("\n--- Indice Hash ---\n");
                imprimirIndiceHash(&ht);
                break;

            case 3:
                printf("Digite a palavra para buscar na Patricia: ");
                fgets(palavra_busca, sizeof(palavra_busca), stdin);
                palavra_busca[strcspn(palavra_busca, "\n")] = '\0';
                if (strlen(palavra_busca) > 0) {
                    normalizar_palavra(palavra_busca);
                    Palavra *resultado = buscar_palavra(indice_patricia, palavra_busca);
                    if (resultado) {
                        printf("Palavra '%s' encontrada na Patricia:\n", resultado->texto);
                        Ocorrencia *oc = resultado->ocorrencias;
                        while (oc) {
                            printf(" -> Em %s (ID %d) | Qtd: %d | Peso: %.4f\n",
                                lista->arquivos[oc->idDoc].nome, oc->idDoc, oc->qtd, oc->peso);
                            oc = oc->prox;
                        }
                    } else {
                        printf("Palavra '%s' nao encontrada na Patricia.\n", palavra_busca);
                    }
                }
                break;

            case 4:
                printf("Digite a palavra para buscar na Hash: ");
                fgets(palavra_busca, sizeof(palavra_busca), stdin);
                palavra_busca[strcspn(palavra_busca, "\n")] = '\0';
                if (strlen(palavra_busca) > 0) {
                    normalizar_palavra(palavra_busca);
                    Palavra* resultado = buscar_palavra_hash(&ht, palavra_busca);
                    if (resultado) {
                        printf("Palavra '%s' encontrada na Hash:\n", resultado->texto);
                        Ocorrencia *oc = resultado->ocorrencias;
                        while (oc) {
                            printf(" -> Em %s (ID %d) | Qtd: %d | Peso: %.4f\n",
                                lista->arquivos[oc->idDoc].nome, oc->idDoc, oc->qtd, oc->peso);
                            oc = oc->prox;
                        }
                        free(resultado->texto);
                        free(resultado);
                    } else {
                        printf("Palavra '%s' nao encontrada na Hash.\n", palavra_busca);
                    }
                }
                break;

            case 5:
                printf("Encerrando o programa...\n");
                break;

            default:
                printf("Opcao invalida! Tente novamente.\n");
                break;
        }
    } while (opcao != 5);

    printf("Liberando memoria...\n");
    liberar_indice_patricia(indice_patricia);
    liberar_lista(lista);


    printf("Programa finalizado.\n");
    return 0;
}