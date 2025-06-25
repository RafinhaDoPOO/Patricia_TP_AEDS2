#include "patricia.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int comparacoes_patricia_insercao = 0;
int comparacoes_patricia_busca = 0;



// Retorna o i-ésimo bit do texto da palavra (const char*)
Dib bit(Index i, const char* texto_palavra) {
    if (i == 0) {
        return 0; // Índice de bit inválido ou sem efeito
    }

    // Determina qual caractere o bit 'i' pertence (base 0)
    int char_index = (i - 1) / D;

    // Determina qual bit dentro desse caractere (base 0 do MSB)
    int bit_in_char_index = (i - 1) % D;

    // Se o índice do caractere está além do comprimento da string, consideramos o bit como 0
    if (char_index >= strlen(texto_palavra)) {
        return 0; // Implica que strings mais curtas são "acolchoadas" com zeros
    }

    unsigned char caractere = texto_palavra[char_index];

    // Desloca o bit desejado para a posição menos significativa e isola-o
    return (caractere >> (D - 1 - bit_in_char_index)) & 1;
}

// Verifica se um nó é externo
short ehExterno(ArvorePat p) {
    return (p->nt == externo); 
}

// Cria um nó interno
ArvorePat criarNoInterno(Index i, ArvorePat esq, ArvorePat dir) { 
    ArvorePat p = (ArvorePat)malloc(sizeof(NoPatricia)); // Usando o novo nome da struct
    if (p == NULL) {
        perror("Falha ao alocar memória para o nó interno");
        exit(EXIT_FAILURE);
    }
    p->nt = interno; // Usando o novo nome do enumerador
    p->No.noInterno.index = i;
    p->No.noInterno.esq = esq; // Atribui filho esquerdo
    p->No.noInterno.dir = dir; // Atribui filho direito
    return p;
}

// Cria um nó externo
ArvorePat criarNoExterno(Palavra* palavra_dado) { // Agora recebe um Palavra*
    ArvorePat p = (ArvorePat)malloc(sizeof(NoPatricia)); // Usando o novo nome da struct
    if (p == NULL) {
        perror("Falha ao alocar memória para o nó externo");
        exit(EXIT_FAILURE);
    }
    p->nt = externo; // Usando o novo nome do enumerador
    p->No.NoExterno.palavra = palavra_dado; // Armazena o ponteiro para Palavra
    return p;
}

// Insere um novo nó na árvore entre caminhos existentes (Função auxiliar interna)
 ArvorePat inserirEntre(ArvorePat* t, Palavra* nova_palavra_dado, Index i, const char* texto_palavra_para_bit) {
    // texto_palavra_para_bit é a string da nova palavra usada para comparações de bits
    ArvorePat NovoNoExterno = criarNoExterno(nova_palavra_dado); // Cria o novo nó externo com a Palavra*

    if (ehExterno(*t) || i < (*t)->No.noInterno.index) {
        // Se *t é um nó externo ou o índice 'i' é menor que o índice do nó interno atual,
        // então este é o ponto onde a nova palavra se bifurca.
        if (bit(i, texto_palavra_para_bit) == 1) { // Usa o texto da nova palavra para o bit
            return criarNoInterno(i, *t, NovoNoExterno); // Cria um novo nó interno, colocando *t e o NovoNoExterno
        } else {
            return criarNoInterno(i, NovoNoExterno, *t);
        }
    } else {
        // Continua a descida na árvore, pois o ponto de bifurcação está mais abaixo
        if (bit((*t)->No.noInterno.index, texto_palavra_para_bit) == 1) { // Usa o texto da nova palavra para o bit
            (*t)->No.noInterno.dir = inserirEntre(&(*t)->No.noInterno.dir, nova_palavra_dado, i, texto_palavra_para_bit);
        } else {
            (*t)->No.noInterno.esq = inserirEntre(&(*t)->No.noInterno.esq, nova_palavra_dado, i, texto_palavra_para_bit);
        }
        return *t; // Retorna o nó interno atual
    }
}

// Insere uma palavra na árvore Patricia e atualiza suas ocorrências (Função pública)
// Recebe o texto da palavra e o id do documento.
ArvorePat inserir(ArvorePat* t, const char* texto_palavra, int idDoc) {
    ArvorePat p;
    Index i;

    if (*t == NULL) {
        // Árvore vazia, a palavra é a primeira a ser inserida.
        // Cria a estrutura Palavra e adiciona a primeira ocorrência.
        Palavra* nova_palavra = (Palavra*)malloc(sizeof(Palavra));
        if (nova_palavra == NULL) {
            perror("Falha ao alocar memoria para Palavra");
            exit(EXIT_FAILURE);
        }
        nova_palavra->texto = strdup(texto_palavra);
        if (nova_palavra->texto == NULL) {
            perror("Falha ao duplicar texto da Palavra");
            free(nova_palavra); // Libera a estrutura Palavra se strdup falhar
            exit(EXIT_FAILURE);
        }
        nova_palavra->ocorrencias = NULL; // Inicializa a lista de ocorrências vazia
        adicionar_ocorrencia(nova_palavra, idDoc);
        //adicionar_ocorrencia_a_palavra(nova_palavra, idDoc); // Adiciona a primeira ocorrência
        return criarNoExterno(nova_palavra); // Retorna o novo nó externo como a raiz
    } else {
        p = *t;
        // Percorrer a árvore até encontrar um nó externo ou um ponto de divergência.
        while (!ehExterno(p)) {
            comparacoes_patricia_insercao++; // comparando bit para decidir caminho, logo +1

            if (bit(p->No.noInterno.index, texto_palavra) == 1) { // Usa o texto da palavra para comparação de bit
                p = p->No.noInterno.dir;
            } else {
                p = p->No.noInterno.esq;
            }
        }
    }

    comparacoes_patricia_insercao++; // comparando strings no nó externo (comparaçao em relaçao ao if abaixo, sempre vai comparar se a palavra ja existe)

    // Após encontrar um nó externo 't', comparar o texto da nova palavra com a chave armazenada em 'p'.
    if (strcmp(texto_palavra, p->No.NoExterno.palavra->texto) == 0) {
        // A palavra já existe na árvore. Apenas atualiza sua lista de ocorrências.
        adicionar_ocorrencia(p->No.NoExterno.palavra, idDoc);
        return *t; // Retorna a raiz da árvore sem modificá-la (o *t original)
    }

    // Se as palavras são diferentes, encontra o primeiro bit que difere.
    i = 1;
    // O limite máximo de bits é baseado no comprimento da string mais longa entre as duas palavras.
    int max_len_nova = strlen(texto_palavra);
    int max_len_existente = strlen(p->No.NoExterno.palavra->texto);
    int max_bits_possiveis = D * (max_len_nova > max_len_existente ? max_len_nova : max_len_existente);

    while ((i <= max_bits_possiveis) && (bit(i, texto_palavra) == bit(i, p->No.NoExterno.palavra->texto))) {
        comparacoes_patricia_insercao++; // comparando bit a bit para encontrar divergencia

        i++;
    }

    // 'i' é o índice do primeiro bit diferente. Uma nova 'Palavra' precisa ser criada para o nó externo.
    Palavra* nova_palavra_dado = (Palavra*)malloc(sizeof(Palavra));
    if (nova_palavra_dado == NULL) { perror("Falha ao alocar memoria para Palavra (nova)"); exit(EXIT_FAILURE); }
    nova_palavra_dado->texto = strdup(texto_palavra);
    if (nova_palavra_dado->texto == NULL) {
        perror("Falha ao duplicar texto da Palavra (nova)");
        free(nova_palavra_dado);
        exit(EXIT_FAILURE);
    }
    nova_palavra_dado->ocorrencias = NULL; // Inicializa a lista de ocorrências vazia

    adicionar_ocorrencia(nova_palavra_dado, idDoc); // Adiciona a primeira ocorrência

    // Chamar inserirEntre para adicionar o novo nó na posição correta da árvore.
    return inserirEntre(t, nova_palavra_dado, i, texto_palavra);
}
//-----------------------------------------------------------------------------------------------------//

// --- FUNÇÃO DE CONSTRUÇÃO DE ÍNDICE (VERSÃO CORRIGIDA) ---
ArvorePat construir_indice_patricia(ListaArquivos* lista) {
    ArvorePat raiz_patricia = NULL;
    if (!lista) {
        fprintf(stderr, "Erro: Lista de arquivos nula.\n");
        return NULL;
    }

    printf("Iniciando construcao do índice Patricia para %d documentos...\n", lista->quantidade);

    for (int i = 0; i < lista->quantidade; i++) {
        const char* nome_arquivo = lista->arquivos[i].nome;
        const char* conteudo = lista->arquivos[i].conteudo;

        if (!conteudo || strcmp(conteudo, "erro ao ler arquivo") == 0) {
            fprintf(stderr, "Aviso: Pulando arquivo '%s' por erro de leitura.\n", nome_arquivo);
            continue;
        }

        printf("  Processando documento: %s (ID: %d)\n", nome_arquivo, i);

        char* conteudo_copia = strdup(conteudo);
        if (!conteudo_copia) continue;

        char* palavra_token = strtok(conteudo_copia, " \t\n\r.,;:!?()[]{}<>");

        while (palavra_token != NULL) {
            normalizar_palavra(palavra_token);
            if (strlen(palavra_token) > 0) {
                raiz_patricia = inserir(&raiz_patricia, palavra_token, i);
            }
            palavra_token = strtok(NULL, " \t\n\r.,;:!?()[]{}<>");
        }

        free(conteudo_copia);
    }
    printf("Construção do índice Patricia finalizada.\n");
    return raiz_patricia;
}

void imprimir_patricia(ArvorePat no, ListaArquivos* lista) {
    if (no == NULL) {
        return;
    }

    if (no->nt == externo) {
        Palavra* p = no->No.NoExterno.palavra;
        printf("- Palavra: \"%s\"\n", p->texto);
        Ocorrencia* oc = p->ocorrencias;
        while(oc) {
            printf("    -> Em %s (ID %d) | Qtd: %d | Peso: %.4f\n",
                   lista->arquivos[oc->idDoc].nome, oc->idDoc, oc->qtd, oc->peso);
            oc = oc->prox;
        }
    } else {
        imprimir_patricia(no->No.noInterno.esq, lista);
        imprimir_patricia(no->No.noInterno.dir, lista);
    }
}


Palavra* buscar_palavra(ArvorePat arvore, const char* texto_palavra) {
    if (arvore == NULL) {
        return NULL; // Árvore vazia, palavra não encontrada
    }

    ArvorePat p = arvore;

    // Navega pela árvore seguindo os bits da palavra buscada
    while (p->nt == interno) {
        comparacoes_patricia_busca++; // comparação de bit

        if (bit(p->No.noInterno.index, texto_palavra) == 1) {
            p = p->No.noInterno.dir;
        } else {
            p = p->No.noInterno.esq;
        }
    }

    // Compara o texto no nó externo (seja sucesso ou não)
    comparacoes_patricia_busca++; // comparação de string

    if (strcmp(texto_palavra, p->No.NoExterno.palavra->texto) == 0) {
        return p->No.NoExterno.palavra; // Palavra encontrada!
    } else {
        return NULL; // Palavra não encontrada
    }
}

//_________________________________________________________________________--
void liberar_indice_patricia(ArvorePat arvore) {
    if (arvore == NULL) return;

    if (arvore->nt == externo) {
        if (arvore->No.NoExterno.palavra != NULL) {
            free(arvore->No.NoExterno.palavra->texto);
            liberarOcorrenciasDaPalavra(arvore->No.NoExterno.palavra->ocorrencias);
            free(arvore->No.NoExterno.palavra);
        }
    } else {
        liberar_indice_patricia(arvore->No.noInterno.esq);
        liberar_indice_patricia(arvore->No.noInterno.dir);
    }
    free(arvore);
}
