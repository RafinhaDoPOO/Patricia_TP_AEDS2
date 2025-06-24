#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "patricia.h"
#include "hash.h"
#include "leitor.h"
#include "processador.h"
#include "menu.h"

// Funcoes auxiliares 
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

void inicializar_sistema(const char *arquivo_entrada) {
    printf("1. Lendo arquivos de '%s'...\n", arquivo_entrada);
    ListaArquivos* lista = ler_arq_entrada(arquivo_entrada);
    if (!lista) {
        fprintf(stderr, "Erro ao carregar lista de arquivos.\n");
        return;
    }
    ler_conteudo_arquivos(lista);

    // Construção da Patricia
    printf("2. Construindo o índice Patricia...\n");
    comparacoes_patricia_insercao = 0;
    ArvorePat indice_patricia = construir_indice_patricia(lista);
    if (!indice_patricia) {
        liberar_lista(lista);
        return;
    }

    printf("3. Calculando relevância (TF-IDF) Patricia...\n");
    calcular_relevancia_patricia(indice_patricia, lista);

    // Construção da Hash
    printf("4. Construindo o índice Hash...\n");
    HashTable ht;
    for (int i = 0; i < TAM_HASH; i++) ht.tabela[i] = NULL;
    comparacoes_hash_insercao = 0;
    construir_indice_hash(&ht, lista);

    printf("5. Calculando relevância (TF-IDF) Hash...\n");
    calcular_relevancia_hash(&ht, lista);

    // Menu principal
    exibir_menu(lista, indice_patricia, &ht);

    // Liberação de memória
    printf("Liberando memória...\n");
    liberar_indice_patricia(indice_patricia);
    liberar_lista(lista);
    printf("Programa finalizado.\n");
}



void exibir_menu(ListaArquivos* lista, ArvorePat indice_patricia, HashTable* ht) {
    int opcao, subop;
    char palavra_busca[256];

    do {
        printf("\n========== MENU ==========\n");
        printf("1. Imprimir indice completo\n");
        printf("2. Buscar palavra\n");
        printf("3. Exibir comparacoes de INSERCAO\n");
        printf("4. Exibir comparacoes de BUSCA\n");
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
                printf("\n1) Patricia   2) Hash\n");
                printf("Escolha estrutura para imprimir: ");
                scanf("%d", &subop);
                while (getchar() != '\n');

                if (subop == 1) {
                    printf("\n--- Indice Patricia ---\n");
                    imprimir_patricia(indice_patricia, lista);
                } else if (subop == 2) {
                    printf("\nHash: 1) Ordenado   2) Sem ordenacao\n");
                    printf("Escolha o modo de impressao: ");
                    scanf("%d", &subop);
                    while (getchar() != '\n');

                    if (subop == 1) {
                        printf("\n--- Indice Hash (ordenado) ---\n");
                        imprimirIndiceHash(ht);
                    } else if (subop == 2) {
                        printf("\n--- Indice Hash (Sem ordenacao) ---\n");
                        imprimirHash_ordemInsercao(ht);
                    } else {
                        printf("Opcao invalida!\n");
                    }
                } else {
                    printf("Opcao invalida!\n");
                }
                break;

            case 2:
                printf("\n1) Patricia   2) Hash\nEscolha estrutura para buscar: ");
                scanf("%d", &subop);
                while (getchar() != '\n');
                printf("Digite a palavra: ");
                fgets(palavra_busca, sizeof(palavra_busca), stdin);
                palavra_busca[strcspn(palavra_busca, "\n")] = '\0';
                if (strlen(palavra_busca) == 0) {
                    printf("Nenhuma palavra informada.\n");
                    break;
                }
                normalizar_palavra(palavra_busca);

                if (subop == 1) {
                    comparacoes_patricia_busca = 0;
                    Palavra *res_p = buscar_palavra(indice_patricia, palavra_busca);
                    if (res_p) {
                        printf("Encontrado na Patricia:\n");
                        for (Ocorrencia* oc = res_p->ocorrencias; oc; oc = oc->prox) {
                            printf(" -> %s (ID %d) Qtd:%d Peso:%.4f\n",
                                   lista->arquivos[oc->idDoc].nome,
                                   oc->idDoc, oc->qtd, oc->peso);
                        }
                    } else {
                        printf("Nao encontrado na Patricia.\n");
                    }
                } else if (subop == 2) {
                    comparacoes_hash_busca = 0;
                    Palavra *res_h = buscar_palavra_hash(ht, palavra_busca);
                    if (res_h) {
                        printf("Encontrado na Hash:\n");
                        for (Ocorrencia* oc = res_h->ocorrencias; oc; oc = oc->prox) {
                            printf(" -> %s (ID %d) Qtd:%d Peso:%.4f\n",
                                   lista->arquivos[oc->idDoc].nome,
                                   oc->idDoc, oc->qtd, oc->peso);
                        }
                        free(res_h->texto);
                        free(res_h);
                    } else {
                        printf("Nao encontrado na Hash.\n");
                    }
                } else {
                    printf("Opcao invalida!\n");
                }
                break;

            case 3:
                printf("\n--- INSERCAO ---\n");
                printf("1) Patricia    2) Hash \nEscolha: ");
                scanf("%d", &subop);
                while (getchar() != '\n');
                if (subop == 1)
                    printf("Comparacoes insercao Patricia: %d\n", comparacoes_patricia_insercao);
                else if (subop == 2)
                    printf("Comparacoes insercao Hash:     %d\n", comparacoes_hash_insercao);
                else
                    printf("Opcao invalida!\n");
                break;

            case 4:
                printf("\n--- BUSCA ---\n");
                printf("1) Patricia   2) Hash\nEscolha: ");
                scanf("%d", &subop);
                while (getchar() != '\n');
                if (subop == 1)
                    printf("Comparacoes busca Patricia: %d\n", comparacoes_patricia_busca);
                else if (subop == 2)
                    printf("Comparacoes busca Hash:     %d\n", comparacoes_hash_busca);
                else
                    printf("Opcao invalida!\n");
                break;

            case 5:
                printf("Encerrando o programa...\n");
                break;

            default:
                printf("Opcao invalida! Tente novamente.\n");
                break;
        }
    } while (opcao != 5);
}
