#ifndef PATRICIA_H
#define PATRICIA_H

#include "processador.h"

int comparacoes_patricia_insercao ;
int comparacoes_patricia_busca ; 



#define D 8 // Bits em um caractere

typedef enum {
    interno,
    externo
} NoType;

typedef unsigned int Index;
typedef unsigned char Dib;

typedef struct NoPatricia* ArvorePat;
typedef struct NoPatricia {
    NoType nt;
    union {
        struct {
            Index index;
            ArvorePat esq, dir;
        } noInterno;
        struct {
            Palavra* palavra;
        } NoExterno;
    } No;
} NoPatricia;

// Protótipos das funções públicas da árvore Patricia
ArvorePat inserir(ArvorePat* t, const char* texto_palavra, int idDoc);
ArvorePat construir_indice_patricia(ListaArquivos* lista);
Palavra* buscar_palavra(ArvorePat arvore, const char* texto_palavra);
void liberar_indice_patricia(ArvorePat arvore);

void imprimir_patricia(ArvorePat no, ListaArquivos* lista);

#endif
