#include <stdio.h>
#include "menu.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("ERRO: Forneça o arquivo de entrada como argumento.\n");
        printf("Uso: %s entrada.txt\n", argv[0]);
        return 1;
    }

    inicializar_sistema(argv[1]);
    return 0;
}
