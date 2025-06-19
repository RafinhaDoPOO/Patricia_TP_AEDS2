#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leitor.h"
#include "patricia.h"
#include "processador.h"

/*
 * ===================================================================
 * FUNÇÕES AUXILIARES
 * ===================================================================
 */

/**
 * @brief Percorre a árvore recursivamente e chama a função `calcular_relevancia`
 * (do seu processador.c) para cada palavra.
 *
 * Esta função é chamada UMA ÚNICA VEZ, logo após o índice ser construído.
 *
 * @param no O nó atual da árvore.
 * @param lista A lista de arquivos, necessária para o cálculo do IDF.
 */
void percorrer_e_calcular_relevancia(ArvorePat no, ListaArquivos* lista) {
    if (no == NULL) {
        return;
    }

    if (no->nt == externo) {
        // Encontrou uma folha (palavra), chama a sua função já existente.
        calcular_relevancia(no->No.NoExterno.palavra, lista);
    } else {
        // Se não é folha, continua a busca nos filhos.
        percorrer_e_calcular_relevancia(no->No.noInterno.esq, lista);
        percorrer_e_calcular_relevancia(no->No.noInterno.dir, lista);
    }
}

/**
 * @brief Percorre a árvore e APENAS imprime o conteúdo, pois a relevância
 * já foi calculada e armazenada.
 *
 * @param no O nó atual da árvore.
 * @param lista A lista de arquivos para obter os nomes dos documentos.
 */

/*
 * ===================================================================
 * FUNÇÃO PRINCIPAL
 * ===================================================================
 */
int main(int argc, char *argv[]) {
    // --- PASSO 1: Leitura, Construção e Cálculo ÚNICO ---
    if (argc < 2) {
        printf("ERRO: Forneca o arquivo de entrada como argumento.\n");
        printf("Uso: %s entrada.txt\n", argv[0]);
        return 1;
    }

    printf("1. Lendo arquivos de '%s'...\n", argv[1]);
    ListaArquivos* lista = ler_arq_entrada(argv[1]);
    if (!lista) { return 1; }
    ler_conteudo_arquivos(lista);

    printf("2. Construindo o indice Patricia...\n");
    ArvorePat indice = construir_indice_patricia(lista);
    if (!indice) {
        liberar_lista(lista);
        return 1;
    }
    
    // --- OTIMIZAÇÃO PRINCIPAL ---
    // A relevância de todas as palavras é calculada aqui, uma única vez.
    printf("3. Calculando relevancia (TF-IDF) para todo o indice...\n");
    percorrer_e_calcular_relevancia(indice, lista);
    printf("   ... Calculos finalizados. Sistema pronto!\n");

    // --- PASSO 2: Menu Interativo ---
    int opcao;
    char palavra_busca[256];
    do{
        printf("\n================ MENU =================\n");
        printf("1. Imprimir indice completo\n");
        printf("2. Buscar por uma palavra\n");
        printf("3. Sair\n");
        printf("=======================================\n");
        printf("Escolha uma opcao: ");
        
        if (scanf("%d", &opcao) != 1) {
            while (getchar() != '\n');
            opcao = 0;
        }
        while (getchar() != '\n');

        switch (opcao) {
            case 1:
                printf("\n--- Indice Completo com Relevancia Pre-calculada ---\n");
                imprimir_patricia(indice, lista);
                break;

            case 2:
                printf("Digite a palavra para buscar: ");
                fgets(palavra_busca, sizeof(palavra_busca), stdin);
                palavra_busca[strcspn(palavra_busca, "\n")] = '\0';

                if (strlen(palavra_busca) > 0) {
                    normalizar_palavra(palavra_busca);
                    Palavra *resultado = buscar_palavra(indice, palavra_busca);

                    if (resultado) {
                        printf("\nPalavra '%s' encontrada:\n", resultado->texto);
                        Ocorrencia *oc = resultado->ocorrencias;
                        while (oc) {
                            // Apenas exibe o peso que já foi calculado.
                            printf(" -> Em %s (ID %d) | Qtd: %d | Peso: %.4f\n",
                                   lista->arquivos[oc->idDoc].nome, oc->idDoc, oc->qtd, oc->peso);
                            oc = oc->prox;
                        }
                    } else {
                        printf("\nPalavra '%s' nao encontrada no indice.\n", palavra_busca);
                    }
                }
                break;

            case 3:
                printf("Encerrando o programa...\n");
                break;

            default:
                printf("Opcao invalida! Tente novamente.\n");
        }
    } while (opcao != 3);

    // --- PASSO 3: Liberar Memória ---
    printf("Liberando memoria...\n");
    liberar_indice_patricia(indice);
    liberar_lista(lista);
    printf("Programa finalizado.\n");

    return 0;
}