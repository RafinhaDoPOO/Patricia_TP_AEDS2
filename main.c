#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leitor.h"
#include "patricia.h"
#include "processador.h"
#include "hash.h"
#include <ctype.h>
#include <math.h>


//funções auxiliares

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

    // 1. Leitura dos arquivos
    printf("1. Lendo arquivos de '%s'...\n", argv[1]);
    ListaArquivos* lista = ler_arq_entrada(argv[1]);
    if (!lista) return 1;
    ler_conteudo_arquivos(lista);

    // 2. Construção e relevância Patricia
    printf("2. Construindo o indice Patricia...\n");
    comparacoes_patricia_insercao = 0;                    
    ArvorePat indice_patricia = construir_indice_patricia(lista);
    if (!indice_patricia) {
        liberar_lista(lista);
        return 1;
    }
    printf("3. Calculando relevancia (TF-IDF) Patricia...\n");
    calcular_relevancia_patricia(indice_patricia, lista);

    // 3. Construção e relevância Hash
    printf("4. Construindo o indice Hash...\n");
    HashTable ht;
    for (int i = 0; i < TAM_HASH; i++) ht.tabela[i] = NULL;
    comparacoes_hash_insercao = 0;                        
    construir_indice_hash(&ht, lista);
    printf("5. Calculando relevancia (TF-IDF) Hash...\n");
    calcular_relevancia_hash(&ht, lista);


    int opcao, subop;
    char palavra_busca[256];

    do {
        // ----- Menu Principal -----
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
            // Submenu impressão
            printf("\n1) Patricia   2) Hash\n");
            printf("Escolha estrutura para imprimir: ");
            scanf("%d", &subop);
            while (getchar() != '\n');

            if (subop == 1) {
                // Imprime Patricia
                printf("\n--- Indice Patricia ---\n");
                imprimir_patricia(indice_patricia, lista);

            } else if (subop == 2) {
                // Novo submenu: Hash ordenado ou sem ordenação
                printf("\nHash: 1) Ordenado   2) Sem ordenacao\n");
                printf("Escolha o modo de impressao: ");
                scanf("%d", &subop);
                while (getchar() != '\n');

                if (subop == 1) {
                    // Impressão ordenada (alfabética)
                    printf("\n--- Indice Hash (ordenado) ---\n");
                    imprimirIndiceHash(&ht);

                } else if (subop == 2) {
                    // Impressão por buckets, sem ordenar
                    printf("\n--- Indice Hash (Sem ordenacao) ---\n");
                    imprimirHash_ordemInsercao(&ht);

                } else {
                    printf("Opcao invalida!\n");
                }

            } else {
                printf("Opcao invalida!\n");
            }
            break;

            case 2:
                // Submenu busca
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
                        Ocorrencia *oc = res_p->ocorrencias;
                        while (oc) {
                            printf(" -> %s (ID %d) Qtd:%d Peso:%.4f\n",
                                   lista->arquivos[oc->idDoc].nome,
                                   oc->idDoc, oc->qtd, oc->peso);
                            oc = oc->prox;
                        }
                    } else {
                        printf("Nao encontrado na Patricia.\n");
                    }
                } 
                else if (subop == 2) {
                    comparacoes_hash_busca = 0;
                    Palavra *res_h = buscar_palavra_hash(&ht, palavra_busca);
                    if (res_h) {
                        printf("Encontrado na Hash:\n");
                        Ocorrencia *oc = res_h->ocorrencias;
                        while (oc) {
                            printf(" -> %s (ID %d) Qtd:%d Peso:%.4f\n",
                                   lista->arquivos[oc->idDoc].nome,
                                   oc->idDoc, oc->qtd, oc->peso);
                            oc = oc->prox;
                        }
                        free(res_h->texto);
                        free(res_h);
                    } else {
                        printf("Nao encontrado na Hash.\n");
                    }
                } 
                else {
                    printf("Opcao invalida!\n");
                }
                break;

            case 3:
                // Exibir comparacoes de INSERCAO
                printf("\n--- INSERCAO ---\n");
                printf("1) Patricia    2) Hash \nEscolha: ");
                scanf("%d", &subop);
                while (getchar() != '\n');
                if (subop == 1) {
                    printf("Comparacoes insercao Patricia: %d\n", comparacoes_patricia_insercao);
                } else if (subop == 2) {
                    printf("Comparacoes insercao Hash:     %d\n", comparacoes_hash_insercao);
                } else {
                    printf("Opcao invalida!\n");
                }
                break;

            case 4:
                // Exibir comparacoes de BUSCA
                printf("\n--- BUSCA ---\n");
                printf("1) Patricia   2) Hash\nEscolha: ");
                scanf("%d", &subop);
                while (getchar() != '\n');
                if (subop == 1) {
                    printf("Comparacoes busca Patricia: %d\n", comparacoes_patricia_busca);
                } else if (subop == 2) {
                    printf("Comparacoes busca Hash:     %d\n", comparacoes_hash_busca);
                } else {
                    printf("Opcao invalida!\n");
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

    // Libera memória
    printf("Liberando memoria...\n");
    liberar_indice_patricia(indice_patricia);
    liberar_lista(lista);
    printf("Programa finalizado.\n");
    return 0;
}
