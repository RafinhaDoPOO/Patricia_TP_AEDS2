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

    liberar_lista(lista);

    return 0;
}
