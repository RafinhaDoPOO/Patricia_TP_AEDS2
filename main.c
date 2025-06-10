#include <stdio.h>
#include <stdlib.h>
#include "leitor.h"
#include "processador.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s entrada.txt\n", argv[0]);
        return 1;
    }

    // Lê a lista de arquivos do arquivo de entrada
    ListaArquivos* lista = ler_arq_entrada(argv[1]);
    if (lista == NULL) {
        printf("Erro ao ler a lista de arquivos.\n");
        return 1;
    }

    // Lê o conteúdo dos arquivos listados
    ler_conteudo_arquivos(lista);

    // Cria o processador e inicializa
    Processador* p = malloc(sizeof(Processador));
    if (!p) {
        printf("Erro de alocação do processador\n");
        liberar_lista(lista);
        return 1;
    }
    p->inicio = NULL;

    // Processa o conteúdo dos arquivos
    processar(p, lista);

    // Imprime o índice invertido
    imprimir_processador(p, lista);

    // Libera a memória alocada
    liberar_processador(p);
    liberar_lista(lista);

    return 0;
}
